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
#تعریف_کرو CMDLINE_NOT_بدلا	1
#تعریف_کرو CMDLINE_بدلا		2
#تعریف_کرو GOTO_معمول_منوال	3
#تعریف_کرو PROCESS_اگلا_کلید	4

// The current cmdline_info.  It is initialized in getcmdline() and after that
// used by other functions.  When invoking getcmdline() recursively it needs
// to be saved with save_cmdline() and restored with restore_cmdline().
ساکن cmdline_info_T ccline;

#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح	new_cmdpos;	// position set by set_cmdline_pos()
#اگر_ختم

ساکن صحیح	extra_char = NUL;  // extra character to display when redrawing
				   // the command line
ساکن صحیح	extra_char_سرکاؤ;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
ساکن صحیح	cmd_hkmap = 0;	// Hebrew mapping during command line
#اگر_ختم

ساکن char_u	*getcmdline_int(صحیح firstc, طویل گنتی, صحیح indent, صحیح clear_ccline);
ساکن صحیح	cmdline_charsize(صحیح idx);
ساکن خالی	سیٹ_cmdspos(خالی);
ساکن خالی	سیٹ_cmdspos_cursor(خالی);
ساکن خالی	correct_cmdspos(صحیح idx, صحیح cells);
ساکن خالی	dealloc_cmdbuff(خالی);
ساکن خالی	alloc_cmdbuff(size_t len);
ساکن خالی	draw_cmdline(صحیح آغاز, صحیح len);
ساکن خالی	save_cmdline(cmdline_info_T *ccp);
ساکن خالی	restore_cmdline(cmdline_info_T *ccp);
ساکن صحیح	cmdline_paste(صحیح regname, صحیح literally, صحیح remcr);
ساکن خالی	redrawcmdprompt(خالی);
ساکن صحیح	ccheck_abbr(صحیح);
ساکن صحیح	open_cmdwin(خالی);
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
ساکن صحیح	خالی_خانے_نمونہ_magic(char_u *pat, size_t len, magic_T magic_val);
#اگر_ختم

ساکن صحیح	cedit_کلید = -1;	// key value of 'cedit' option

    ساکن خالی
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
    ساکن خالی
abandon_cmdline(خالی)
{
    dealloc_cmdbuff();
    اگر (پیغام_scrolled == 0)
	compute_cmdrow();
    پیغام("");
    redraw_cmdline = TRUE;
}

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
/*
 * Guess that the pattern matches everything.  Only finds specific cases, such
 * as a trailing \|, which can happen while typing a pattern.
 */
    ساکن صحیح
خالی_خانے_نمونہ(char_u *p, size_t len, صحیح delim)
{
    magic_T	magic_val = MAGIC_ON;

    اگر (len > 0)
	(خالی) skip_regexp_ex(p, delim, magic_isset(), تہی, تہی, &magic_val);
    ورنہ
	واپس TRUE;

    واپس خالی_خانے_نمونہ_magic(p, len, magic_val);
}

    ساکن صحیح
خالی_خانے_نمونہ_magic(char_u *p, size_t len, magic_T magic_val)
{
    // remove trailing \v and the like
    جب (len >= 2 && p[len - 2] == '\\'
			&& vim_strchr((char_u *)"mMvVcCZ", p[len - 1]) != تہی)
	len -= 2;

    // true, if the pattern is empty, or the pattern ends with \| and magic is
    // set (or it ends with '|' and very magic is set)
    واپس len == 0
	|| (
	    len > 1 && p[len - 1] == '|'
	    && (
		(p[len - 2] == '\\' && magic_val == MAGIC_ON) ||
		(p[len - 2] != '\\' && magic_val == MAGIC_ALL)
	    )
	);
}

// Struct to store the viewstate during 'incsearch' highlighting.
قسمتعریف ساخت {
    colnr_T	vs_curswant;
    colnr_T	vs_leftcol;
    colnr_T	vs_skipcol;
    linenr_T	vs_topline;
# ifdef FEAT_DIFF
    صحیح		vs_topfill;
# endif
    linenr_T	vs_botline;
    linenr_T	vs_خالی_خانے_صفیں;
} viewstate_T;

    ساکن خالی
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
    vs->vs_خالی_خانے_صفیں = curwin->w_خالی_خانے_صفیں;
}

    ساکن خالی
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
    curwin->w_خالی_خانے_صفیں = vs->vs_خالی_خانے_صفیں;
}

// Struct to store the state of 'incsearch' highlighting.
قسمتعریف ساخت {
    pos_T	تلاش_آغاز_2;	// where 'incsearch' starts searching
    pos_T	save_cursor;
    صحیح		winid;		// window where this state is valid
    viewstate_T	init_viewstate;
    viewstate_T	old_viewstate;
    pos_T	مطابقت_آغاز;
    pos_T	مطابقت_انتہا;
    صحیح		did_incsearch;
    صحیح		incsearch_postponed;
    optmagic_T	magic_overruled_save;
} incsearch_state_T;

    ساکن خالی
init_incsearch_state(incsearch_state_T *is_state)
{
    is_state->winid = curwin->w_id;
    is_state->مطابقت_آغاز = curwin->w_cursor;
    is_state->did_incsearch = FALSE;
    is_state->incsearch_postponed = FALSE;
    is_state->magic_overruled_save = magic_overruled;
    CLEAR_POS(&is_state->مطابقت_انتہا);
    is_state->save_cursor = curwin->w_cursor;  // may be restored later
    is_state->تلاش_آغاز_2 = curwin->w_cursor;
    save_viewstate(&is_state->init_viewstate);
    save_viewstate(&is_state->old_viewstate);
}

/*
 * First move cursor to end of match, then to the start.  This
 * moves the whole match onto the screen when 'nowrap' is set.
 */
    ساکن خالی
سیٹ_تلاش_مطابقت(pos_T *t)
{
    t->lnum += تلاش_مطابقت_سطریں;
    t->ستون = تلاش_مطابقت_endcol;
    اگر (t->lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	t->lnum = curbuf->b_ml.ml_سطر_گنتی;
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
parse_نمونہ_and_range(
	pos_T	*incsearch_آغاز,
	صحیح	*تلاش_delim,
	صحیح	*skiplen,
	صحیح	*patlen)
{
    char_u	*cmd, *p, *انتہا;
    cmdmod_T	dummy_cmdmod;
    exarg_T	ea;
    pos_T	save_cursor;
    صحیح		delim_optional = FALSE;
    صحیح		delim;
    صحیح		use_آخری_pat;
    magic_T     magic = 0;
    حرف	*dummy;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    // Default range
    تلاش_پہلا_سطر = 0;
    تلاش_آخری_سطر = MAXLNUM;

    CLEAR_میدان(ea);
    ea.line1 = 1;
    ea.line2 = 1;
    ea.cmd = ccline.cmdbuff;
    ea.addr_type = ADDR_سطریں;

    // Skip over command modifiers
    parse_command_modifiers(&ea, &dummy, &dummy_cmdmod, TRUE);

    // Skip over the range to find the command.
    cmd = skip_range(ea.cmd, TRUE, تہی);

    اگر (vim_strchr((char_u *)"sgvlu", *cmd) == تہی)
	واپس FALSE;

    // Skip over command name to find pattern separator
    برائے (p = cmd; ASCII_ISALPHA(*p); ++p)
	;
    اگر (*skipwhite(p) == NUL)
	واپس FALSE;

    اگر (STRNCMP(cmd, "substitute", p - cmd) == 0
	    || STRNCMP(cmd, "smagic", p - cmd) == 0
	    || STRNCMP(cmd, "snomagic", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "vglobal", p - cmd) == 0)
    {
	اگر (*cmd == 's' && cmd[1] == 'm')
	    magic_overruled = OPTION_MAGIC_ON;
	ورنہ اگر (*cmd == 's' && cmd[1] == 'n')
	    magic_overruled = OPTION_MAGIC_OFF;
    }
    ورنہ اگر (STRNCMP(cmd, "sort", MAX(p - cmd, 3)) == 0
	    || STRNCMP(cmd, "uniq", MAX(p - cmd, 3)) == 0)
    {
	// skip over ! and flags
	اگر (*p == '!')
	    p = skipwhite(p + 1);
	جب (ASCII_ISALPHA(*(p = skipwhite(p))))
	    ++p;
	اگر (*p == NUL)
	    واپس FALSE;
    }
    ورنہ اگر (STRNCMP(cmd, "vimgrep", MAX(p - cmd, 3)) == 0
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
		واپس FALSE;
	}
	اگر (*cmd != 'g')
	    delim_optional = TRUE;
    }
    ورنہ
	واپس FALSE;

    p = skipwhite(p);
    delim = (delim_optional && vim_isIDc(*p)) ? ' ' : *p++;
    *تلاش_delim = delim;

    انتہا = skip_regexp_ex(p, delim, magic_isset(), تہی, تہی, &magic);
    use_آخری_pat = انتہا == p && *انتہا == delim;

    اگر (انتہا == p && !use_آخری_pat)
	واپس FALSE;

    // Skip if the pattern matches everything (e.g., for 'hlsearch')
    اگر (!use_آخری_pat)
    {
	حرف c = *انتہا;
	صحیح خالی_خانے_2;

	*انتہا = NUL;
	خالی_خانے_2 = خالی_خانے_نمونہ_magic(p, (size_t)(انتہا - p), magic);
	*انتہا = c;
	اگر (خالی_خانے_2)
	    واپس FALSE;
    }

    // Found a non-empty pattern or //
    *skiplen = (صحیح)(p - ccline.cmdbuff);
    *patlen = (صحیح)(انتہا - p);

    // Parse the address range
    save_cursor = curwin->w_cursor;
    curwin->w_cursor = *incsearch_آغاز;

    parse_cmd_پتہ(&ea, &dummy, TRUE);

    اگر (ea.addr_گنتی > 0)
    {
	صحیح الٹ_مطابقت = ea.line2 < ea.line1;
	تلاش_پہلا_سطر = الٹ_مطابقت ? ea.line2 : ea.line1;
	تلاش_آخری_سطر = الٹ_مطابقت ? ea.line1 : ea.line2;
    }
    ورنہ اگر (cmd[0] == 's' && cmd[1] != 'o')
	// :s defaults to the current line
	تلاش_پہلا_سطر = تلاش_آخری_سطر = curwin->w_cursor.lnum;

    curwin->w_cursor = save_cursor;
    واپس TRUE;
}

/*
 * Return TRUE when 'incsearch' highlighting is to be done.
 * Sets search_first_line and search_last_line to the address range.
 * May change the last search pattern.
 */
    ساکن صحیح
do_incsearch_highlighting(
	صحیح		    firstc,
	صحیح		    *تلاش_delim,
	incsearch_state_T   *is_state,
	صحیح		    *skiplen,
	صحیح		    *patlen)
{
    صحیح retval = FALSE;

    *skiplen = 0;
    *patlen = ccline.cmdlen;

    اگر (!p_is || cmd_silent)
	واپس FALSE;

    // By default search all lines
    تلاش_پہلا_سطر = 0;
    تلاش_آخری_سطر = MAXLNUM;

    اگر (firstc == '/' || firstc == '?')
    {
	*تلاش_delim = firstc;
	واپس TRUE;
    }

    اگر (firstc != ':')
	واپس FALSE;

    ++emsg_off;
    retval = parse_نمونہ_and_range(&is_state->تلاش_آغاز_2, تلاش_delim,
	    skiplen, patlen);
    --emsg_off;

    واپس retval;
}

    ساکن خالی
اختتام_incsearch_highlighting(
	صحیح gotesc,
	incsearch_state_T *is_state,
	صحیح call_update_screen)
{
    اگر (!is_state->did_incsearch)
	واپس;

    is_state->did_incsearch = FALSE;
    اگر (gotesc)
	curwin->w_cursor = is_state->save_cursor;
    ورنہ
    {
	اگر (!EQUAL_POS(is_state->save_cursor, is_state->تلاش_آغاز_2))
	{
	    // put the '" mark at the original position
	    curwin->w_cursor = is_state->save_cursor;
	    setpcmark();
	}
	curwin->w_cursor = is_state->تلاش_آغاز_2;
    }
    restore_viewstate(&is_state->old_viewstate);
    highlight_مطابقت = FALSE;

    // by default search all lines
    تلاش_پہلا_سطر = 0;
    تلاش_آخری_سطر = MAXLNUM;

    magic_overruled = is_state->magic_overruled_save;

    validate_cursor();	// needed for TAB
    حالت_redraw_all();
    redraw_all_later(UPD_SOME_VALID);
    اگر (call_update_screen)
	update_screen(UPD_SOME_VALID);
}

/*
 * Do 'incsearch' highlighting if desired.
 */
    ساکن خالی
may_do_incsearch_highlighting(
	صحیح		    firstc,
	طویل		    گنتی,
	incsearch_state_T   *is_state)
{
    صحیح		skiplen, patlen;
    صحیح		ملا;  // do_search() result
    pos_T	انتہا_pos;
# ifdef FEAT_RELTIME
    searchit_arg_T sia;
# endif
    صحیح		اگلا_char;
    صحیح		use_آخری_pat;
    صحیح		did_do_incsearch = is_state->did_incsearch;
    صحیح		تلاش_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخری_تلاش_نمونہ();

    اگر (!do_incsearch_highlighting(firstc, &تلاش_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخری_تلاش_نمونہ();
	اختتام_incsearch_highlighting(FALSE, is_state, TRUE);
	اگر (did_do_incsearch && vpeekc() == NUL)
	    // may have skipped a redraw, do it now
	    redrawcmd();
	واپس;
    }

    // If there is a character waiting, search and redraw later.
    اگر (char_avail())
    {
	restore_آخری_تلاش_نمونہ();
	is_state->incsearch_postponed = TRUE;
	واپس;
    }
    is_state->incsearch_postponed = FALSE;

    اگر (تلاش_پہلا_سطر == 0)
	// start at the original cursor position
	curwin->w_cursor = is_state->تلاش_آغاز_2;
    ورنہ اگر (تلاش_پہلا_سطر > curbuf->b_ml.ml_سطر_گنتی)
    {
	// start after the last line
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	curwin->w_cursor.ستون = MAXCOL;
    }
    ورنہ
    {
	// start at the first line in the range
	curwin->w_cursor.lnum = تلاش_پہلا_سطر;
	curwin->w_cursor.ستون = 0;
    }

    // Use the previous pattern for ":s//".
    اگلا_char = ccline.cmdbuff[skiplen + patlen];
    use_آخری_pat = patlen == 0 && skiplen > 0
				   && ccline.cmdbuff[skiplen - 1] == اگلا_char;

    // If there is no pattern, don't do anything.
    اگر (patlen == 0 && !use_آخری_pat)
    {
	ملا = 0;
	سیٹ_no_hlsearch(TRUE); // turn off previous highlight
	redraw_all_later(UPD_SOME_VALID);
    }
    ورنہ
    {
	صحیح تلاش_flags = تلاش_OPT + تلاش_NOOF + تلاش_PEEK;

	cursor_off();	// so the user knows we're busy
	برآمد_flush();
	++emsg_off;	// so it doesn't beep if bad expr
	اگر (!p_hls)
	    تلاش_flags += تلاش_KEEP;
	اگر (تلاش_پہلا_سطر != 0)
	    تلاش_flags += تلاش_آغاز;
	ccline.cmdbuff[skiplen + patlen] = NUL;
# ifdef FEAT_RELTIME
	CLEAR_میدان(sia);
	// Set the time limit to half a second.
	sia.sa_tm = 500;
# endif
	ملا = do_تلاش(تہی, firstc == ':' ? '/' : firstc, تلاش_delim,
				 ccline.cmdbuff + skiplen, patlen, گنتی, تلاش_flags,
# ifdef FEAT_RELTIME
		&sia
# ورنہ
		تہی
# endif
		);
	ccline.cmdbuff[skiplen + patlen] = اگلا_char;
	--emsg_off;

	اگر (curwin->w_cursor.lnum < تلاش_پہلا_سطر
		|| curwin->w_cursor.lnum > تلاش_آخری_سطر)
	{
	    // match outside of address range
	    ملا = 0;
	    curwin->w_cursor = is_state->تلاش_آغاز_2;
	}

	// if interrupted while searching, behave like it failed
	اگر (got_int)
	{
	    (خالی)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    ملا = 0;
	}
	ورنہ اگر (char_avail())
	    // cancelled searching because a char was typed
	    is_state->incsearch_postponed = TRUE;
    }
    اگر (ملا != 0)
	highlight_مطابقت = TRUE;		// highlight position
    ورنہ
	highlight_مطابقت = FALSE;	// remove highlight

    // First restore the old curwin values, so the screen is positioned in the
    // same way as the actual search command.
    restore_viewstate(&is_state->old_viewstate);
    بدلا_cline_bef_curs();
    update_topline();

    اگر (ملا != 0)
    {
	pos_T	    save_pos = curwin->w_cursor;

	is_state->مطابقت_آغاز = curwin->w_cursor;
	سیٹ_تلاش_مطابقت(&curwin->w_cursor);
	validate_cursor();
	انتہا_pos = curwin->w_cursor;
	is_state->مطابقت_انتہا = انتہا_pos;
	curwin->w_cursor = save_pos;
    }
    ورنہ
	انتہا_pos = curwin->w_cursor; // shutup gcc 4

    // Disable 'hlsearch' highlighting if the pattern matches everything.
    // Avoids a flash when typing "foo\|".
    اگر (!use_آخری_pat)
    {
	اگلا_char = ccline.cmdbuff[skiplen + patlen];
	ccline.cmdbuff[skiplen + patlen] = NUL;
	اگر (خالی_خانے_نمونہ(ccline.cmdbuff + skiplen, (size_t)patlen, تلاش_delim)
							       && !no_hlsearch)
	{
	    redraw_all_later(UPD_SOME_VALID);
	    سیٹ_no_hlsearch(TRUE);
	}
	ccline.cmdbuff[skiplen + patlen] = اگلا_char;
    }

    validate_cursor();

    // May redraw the status line to show the cursor position.
    اگر (p_ru && curwin->w_حالت_اونچائی > 0)
	curwin->w_redr_حالت = true;

    update_screen(UPD_SOME_VALID);
    highlight_مطابقت = FALSE;
    restore_آخری_تلاش_نمونہ();

    // Leave it at the end to make CTRL-R CTRL-W work.  But not when beyond the
    // end of the pattern, e.g. for ":s/pat/".
    اگر (ccline.cmdbuff[skiplen + patlen] != NUL)
	curwin->w_cursor = is_state->تلاش_آغاز_2;
    ورنہ اگر (ملا != 0)
	curwin->w_cursor = انتہا_pos;

    پیغام_starthere();
    redrawcmdline();
    is_state->did_incsearch = TRUE;
}

/*
 * May adjust 'incsearch' highlighting for typing CTRL-G and CTRL-T, go to next
 * or previous match.
 * Returns FAIL when jumping to cmdline_not_changed;
 */
    ساکن صحیح
may_adjust_incsearch_highlighting(
	صحیح			firstc,
	طویل			گنتی,
	incsearch_state_T	*is_state,
	صحیح			c)
{
    صحیح		skiplen, patlen;
    pos_T	t;
    char_u	*pat;
    char_u	*dircp = تہی;
    char_u	*searchstr;
    char_u	*strcopy = تہی;
    size_t	searchstrlen;
    size_t	patlen_s;
    soffset_T	انحراف;
    صحیح		تلاش_flags = تلاش_NOOF;
    صحیح		i;
    صحیح		تلاش_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخری_تلاش_نمونہ();

    اگر (!do_incsearch_highlighting(firstc, &تلاش_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخری_تلاش_نمونہ();
	واپس OK;
    }
    اگر (patlen == 0 && ccline.cmdbuff[skiplen] == NUL)
    {
	restore_آخری_تلاش_نمونہ();
	واپس FAIL;
    }

    pat = ccline.cmdbuff + skiplen;
    searchstr = pat;
    searchstrlen = (size_t)patlen;
    patlen_s = (size_t)(ccline.cmdlen - skiplen);

    // do not search for the search end delimiter,
    // unless it is part of the pattern
    (خالی)parse_تلاش_نمونہ_انحراف(&pat, &patlen_s, تلاش_delim,
				    تلاش_OPT, &strcopy, &searchstr,
				    &searchstrlen, &dircp, &انحراف);

    cursor_off();
    برآمد_flush();
    اگر (c == Ctrl_G)
    {
	t = is_state->مطابقت_انتہا;
	اگر (LT_POS(is_state->مطابقت_آغاز, is_state->مطابقت_انتہا))
	    // Start searching at the end of the match not at the beginning of
	    // the next column.
	    (خالی)decl(&t);
	تلاش_flags += تلاش_ستون;
    }
    ورنہ
	t = is_state->مطابقت_آغاز;
    اگر (!p_hls)
	تلاش_flags += تلاش_KEEP;
    ++emsg_off;
    i = searchit(curwin, curbuf, &t, تہی,
		 c == Ctrl_G ? FORWARD : BACKWARD,
		 searchstr, searchstrlen, گنتی, تلاش_flags, RE_تلاش, تہی);
    --emsg_off;
    اگر (dircp != تہی)
	*dircp = تلاش_delim;
    اگر (i)
    {
	pos_T	مطابقت_آغاز = is_state->مطابقت_آغاز;
	pos_T	مطابقت_انتہا = is_state->مطابقت_انتہا;
	طویل	off = انحراف.off;

	is_state->تلاش_آغاز_2 = مطابقت_آغاز;
	اگر (!انحراف.سطر && (انحراف.انتہا || off != 0))
	{
	    اگر (انحراف.انتہا)
	    {
		is_state->تلاش_آغاز_2 = مطابقت_انتہا;
		(خالی)decl(&is_state->تلاش_آغاز_2);
	    }
	    جب (off > 0)
	    {
		اگر (incl(&is_state->تلاش_آغاز_2) == -1)
		    توڑو;
		--off;
	    }
	    جب (off < 0)
	    {
		اگر (decl(&is_state->تلاش_آغاز_2) == -1)
		    توڑو;
		++off;
	    }
	}
	is_state->مطابقت_انتہا = t;
	is_state->مطابقت_آغاز = t;
	اگر (c == Ctrl_T && firstc != '?')
	{
	    // Move just before the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->تلاش_آغاز_2 = t;
	    (خالی)decl(&is_state->تلاش_آغاز_2);
	}
	ورنہ اگر (c == Ctrl_G && firstc == '?')
	{
	    // Move just after the current match, so that when nv_search
	    // finishes the cursor will be put back on the match.
	    is_state->تلاش_آغاز_2 = t;
	    (خالی)incl(&is_state->تلاش_آغاز_2);
	}
	اگر (LT_POS(t, is_state->تلاش_آغاز_2) && c == Ctrl_G)
	{
	    // wrap around
	    is_state->تلاش_آغاز_2 = t;
	    اگر (firstc == '?')
		(خالی)incl(&is_state->تلاش_آغاز_2);
	    ورنہ
		(خالی)decl(&is_state->تلاش_آغاز_2);
	}

	سیٹ_تلاش_مطابقت(&is_state->مطابقت_انتہا);
	curwin->w_cursor = is_state->مطابقت_آغاز;
	بدلا_cline_bef_curs();
	update_topline();
	validate_cursor();
	highlight_مطابقت = TRUE;
	save_viewstate(&is_state->old_viewstate);
	update_screen(UPD_NOT_VALID);
	highlight_مطابقت = FALSE;
	redrawcmdline();
	curwin->w_cursor = is_state->مطابقت_انتہا;
    }
    ورنہ
	vim_beep(BO_ERROR);
    vim_free(strcopy);
    restore_آخری_تلاش_نمونہ();
    واپس FAIL;
}

/*
 * When CTRL-L typed: add character from the match to the pattern.
 * May set "*c" to the added character.
 * Return OK when jumping to cmdline_not_changed.
 */
    ساکن صحیح
may_جوڑو_char_مقصد_تلاش(صحیح firstc, صحیح *c, incsearch_state_T *is_state)
{
    صحیح		skiplen, patlen, تلاش_delim;

    // Parsing range may already set the last search pattern.
    // NOTE: must call restore_last_search_pattern() before returning!
    save_آخری_تلاش_نمونہ();

    اگر (!do_incsearch_highlighting(firstc, &تلاش_delim, is_state,
							    &skiplen, &patlen))
    {
	restore_آخری_تلاش_نمونہ();
	واپس FAIL;
    }
    restore_آخری_تلاش_نمونہ();

    // Add a character from under the cursor for 'incsearch'.
    اگر (is_state->did_incsearch)
    {
	curwin->w_cursor = is_state->مطابقت_انتہا;
	*c = gchar_cursor();
	اگر (*c != NUL)
	{
	    // If 'ignorecase' and 'smartcase' are set and the
	    // command line has no uppercase characters, convert
	    // the character to lowercase.
	    اگر (p_ic && p_scs && !pat_has_uppercase(ccline.cmdbuff + skiplen))
		*c = MB_TOLOWER(*c);
	    اگر (*c == تلاش_delim || vim_strchr((char_u *)(
			     magic_isset() ? "\\~^$.*[" : "\\^$"), *c) != تہی)
	    {
		// put a backslash before special characters
		stuffcharReadbuff(*c);
		*c = '\\';
	    }
	    // add any composing characters
	    اگر (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
	    {
		صحیح save_c = *c;

		جب (mb_char2len(*c) != mb_ptr2len(ml_get_cursor()))
		{
		    curwin->w_cursor.ستون += mb_char2len(*c);
		    *c = gchar_cursor();
		    stuffcharReadbuff(*c);
		}
		*c = save_c;
	    }
	    واپس FAIL;
	}
    }
    واپس OK;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_ARABIC
/*
 * Return TRUE if the command line has an Arabic character at or after "start"
 * for "len" bytes.
 */
    ساکن صحیح
cmdline_has_arabic(صحیح آغاز, صحیح len)
{
    صحیح	    j;
    صحیح	    mb_l;
    صحیح	    u8c;
    char_u  *p;
    صحیح	    u8cc[MAX_MCO];

    اگر (!enc_utf8)
	واپس FALSE;

    برائے (j = آغاز; j < آغاز + len; j += mb_l)
    {
	p = ccline.cmdbuff + j;
	u8c = utfc_ptr2char_len(p, u8cc, آغاز + len - j);
	mb_l = utfc_ptr2len_len(p, آغاز + len - j);
	اگر (ARABIC_CHAR(u8c))
	    واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

    خالی
cmdline_init(خالی)
{
    CLEAR_میدان(ccline);
}

/*
 * Handle CTRL-\ pressed in Command-line mode:
 * - CTRL-\ CTRL-N goes to Normal mode
 * - CTRL-\ CTRL-G goes to Insert mode when 'insertmode' is set
 * - CTRL-\ e prompts for an expression.
 */
    ساکن صحیح
cmdline_handle_ctrl_bsl(صحیح c, صحیح *gotesc)
{
    ++no_mapping;
    ++allow_کلیدیں;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدیں;

    // CTRL-\ e doesn't work when obtaining an expression, unless it
    // is in a mapping.
    اگر (c != Ctrl_N && c != Ctrl_G && (c != 'e'
		|| (ccline.cmdfirstc == '=' && کلید_Typed)
#اگر_تعریف_شدہ FEAT_EVAL
		|| cmdline_ستارہ > 0
#اگر_ختم
		))
    {
	vungetc(c);
	واپس PROCESS_اگلا_کلید;
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (c == 'e')
    {
	/*
	 * Replace the command line with the result of an expression.
	 * This will call getcmdline() recursively in get_expr_register().
	 */
	اگر (ccline.cmdpos == ccline.cmdlen)
	    new_cmdpos = 99999;	// keep it at the end
	ورنہ
	    new_cmdpos = ccline.cmdpos;

	c = get_اظہاریہ_رجسٹر();
	اگر (c == '=')
	{
	    char_u	*p = تہی;

	    // Evaluate the expression.  Set "textlock" to avoid nasty things
	    // like going to another buffer.
	    ++textlock;
	    p = get_اظہاریہ_سطر();
	    --textlock;

	    اگر (p != تہی)
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
		    ورنہ
			ccline.cmdpos = new_cmdpos;

		    کلید_Typed = FALSE;	// Don't do p_wc completion.
		    redrawcmd();
		    واپس CMDLINE_بدلا;
		}
		vim_free(p);
	    }
	}
	beep_flush();
	got_int = FALSE;	// don't abandon the command line
	did_emsg = FALSE;
	emsg_on_display = FALSE;
	redrawcmd();
	واپس CMDLINE_NOT_بدلا;
    }
#اگر_ختم

    اگر (c == Ctrl_G && p_im && restart_edit == 0)
	restart_edit = 'a';
    *gotesc = TRUE;	// will free ccline.cmdbuff after putting it
			// in history
    واپس GOTO_معمول_منوال;
}

/*
 * Completion for 'wildchar' or 'wildcharm' key.
 * - hitting <ESC> twice means: abandon command line.
 * - wildcard expansion is only done when the 'wildchar' key is really
 *   typed, not when it comes from a macro
 * Returns CMDLINE_CHANGED if command line is changed or CMDLINE_NOT_CHANGED.
 */
    ساکن صحیح
cmdline_wildchar_complete(
	صحیح		c,
	صحیح		escape,
	صحیح		*did_wild_list,
	صحیح		*wim_اشاریہ_p,
	expand_T	*xp,
	صحیح		*gotesc,
	صحیح		redraw_if_menu_خالی_خانے,
	pos_T		*pre_incsearch_pos)
{
    صحیح		wim_اشاریہ = *wim_اشاریہ_p;
    صحیح		res;
    صحیح		cmdpos_before;
    صحیح		options = WILD_NO_BEEP;
    صحیح		wim_noselect = p_wmnu && (wim_flags[0] & WIM_NOSELECT);
    صحیح		wim_noinsert = p_wmnu && (wim_flags[0] & WIM_NOINSERT);

    اگر (wim_flags[wim_اشاریہ] & WIM_BUFLASTUSED)
	options |= WILD_BUFLASTUSED;
    اگر (xp->xp_numfiles > 0)   // typed p_wc at least twice
    {
	// If "list" is present, list matches unless already listed
	اگر (xp->xp_numfiles > 1
		&& !*did_wild_list
		&& (wim_flags[wim_اشاریہ] & WIM_LIST))
	{
	    (خالی)showmatches(xp, FALSE, TRUE,
		    p_wmnu ? wim_flags[wim_اشاریہ] : 0);
	    redrawcmd();
	    *did_wild_list = TRUE;
	}
	اگر (wim_flags[wim_اشاریہ] & WIM_LONGEST)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ورنہ اگر (wim_flags[wim_اشاریہ] & WIM_بھرا)
	    res = nextwild(xp, WILD_اگلا, options, escape);
	ورنہ
	    res = OK;	    // don't insert 'wildchar' now
    }
    ورنہ		    // typed p_wc first time
    {
	صحیح wim_longest = (wim_flags[0] & WIM_LONGEST);
	صحیح wim_list = (wim_flags[0] & WIM_LIST);
	صحیح wim_بھرا = (wim_flags[0] & WIM_بھرا);

	wim_اشاریہ = 0;
	اگر (c == p_wc || c == p_wcm || c == K_WILD)
	{
	    options |= WILD_MAY_EXPAND_نمونہ;
	    اگر (c == K_WILD)
		options |= WILD_FUNC_TRIGGER;
	    اگر (pre_incsearch_pos)
		xp->xp_pre_incsearch_pos = *pre_incsearch_pos;
	    ورنہ
		xp->xp_pre_incsearch_pos = curwin->w_cursor;
	}
	cmdpos_before = ccline.cmdpos;

	// if 'wildmode' first contains "longest", get longest
	// common part
	اگر (wim_longest)
	    res = nextwild(xp, WILD_LONGEST, options, escape);
	ورنہ
	{
	    اگر (wim_noselect || (wim_list && !wim_بھرا))
		options |= WILD_NOSELECT;
	    اگر (wim_noinsert)
		options |= WILD_NOINSERT;
	    res = nextwild(xp, WILD_EXPAND_KEEP, options, escape);
	}

	// Remove popup window if no completion items are available
	اگر (redraw_if_menu_خالی_خانے && xp->xp_numfiles <= 0)
	{
	    update_screen(0);
	    redrawcmd();  // Ensure initial pasted text appears on cmdline
	}

	// if interrupted while completing, behave like it failed
	اگر (got_int)
	{
	    (خالی)vpeekc();	// remove <C-C> from input stream
	    got_int = FALSE;	// don't abandon the command line
	    (خالی)ExpandOne(xp, تہی, تہی, 0, WILD_FREE);
	    xp->xp_context = EXPAND_NOTHING;
	    *wim_اشاریہ_p = wim_اشاریہ;
	    واپس CMDLINE_بدلا;
	}

	// Display matches
	اگر (res == OK && xp->xp_numfiles > ((wim_noselect || wim_noinsert) ? 0 : 1))
	{
	    اگر (wim_longest)
	    {
		صحیح ملا_longest_سابقہ = (ccline.cmdpos != cmdpos_before);
		صحیح show_menu = p_wmnu && wim_بھرا;

		اگر (wim_list || show_menu)
		    (خالی)showmatches(xp, show_menu, wim_list, WIM_NOSELECT);
		ورنہ اگر (!ملا_longest_سابقہ)
		{
		    صحیح wim_list_اگلا = (wim_flags[1] & WIM_LIST);
		    صحیح wim_بھرا_اگلا = (wim_flags[1] & WIM_بھرا);
		    صحیح wim_noselect_اگلا = (wim_flags[1] & WIM_NOSELECT);
		    صحیح wim_noinsert_اگلا = (wim_flags[1] & WIM_NOINSERT);
		    اگر (wim_list_اگلا || (p_wmnu && (wim_بھرا_اگلا
				    || wim_noselect_اگلا || wim_noinsert_اگلا)))
		    {
			اگر (wim_بھرا_اگلا && !wim_noselect_اگلا && !wim_noinsert_اگلا)
			    nextwild(xp, WILD_اگلا, options, escape);
			ورنہ
			    (خالی)showmatches(xp, p_wmnu && (wim_noselect_اگلا
					|| wim_noinsert_اگلا), wim_list_اگلا,
				    p_wmnu ? wim_flags[1] : 0);

			اگر (wim_list_اگلا)
			    *did_wild_list = TRUE;
		    }
		}
	    }
	    ورنہ
	    {
		صحیح show_menu = p_wmnu && (wim_بھرا || wim_noselect
							    || wim_noinsert);

		اگر (wim_list || show_menu)
		    (خالی)showmatches(xp, show_menu, wim_list,
			    p_wmnu ? wim_flags[0] : 0);
		ورنہ
		    vim_beep(BO_WILD);
	    }

	    redrawcmd();
	    اگر (wim_list)
		*did_wild_list = TRUE;
	}
	ورنہ اگر (xp->xp_numfiles == -1)
	    xp->xp_context = EXPAND_NOTHING;
    }
    اگر (wim_اشاریہ < 3)
	++wim_اشاریہ;
    اگر (c == ESC)
	*gotesc = TRUE;

    *wim_اشاریہ_p = wim_اشاریہ;
    واپس (res == OK) ? CMDLINE_بدلا : CMDLINE_NOT_بدلا;
}

/*
 * Handle backspace, delete and CTRL-W keys in the command-line mode.
 * Returns:
 *  CMDLINE_NOT_CHANGED - if the command line is not changed
 *  CMDLINE_CHANGED - if the command line is changed
 *  GOTO_NORMAL_MODE - go back to normal mode
 */
    ساکن صحیح
cmdline_erase_chars(
	صحیح c,
	صحیح indent
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	, incsearch_state_T *isp
#اگر_ختم
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
	ccline.cmdpos += mb_off_اگلا(ccline.cmdbuff,
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
		جب (p > ccline.cmdbuff && vim_isspace(*p))
		    p = mb_prevptr(ccline.cmdbuff, p);
		i = mb_get_class(p);
		جب (p > ccline.cmdbuff && mb_get_class(p) == i)
		    p = mb_prevptr(ccline.cmdbuff, p);
		اگر (mb_get_class(p) != i)
		    p += (*mb_ptr2len)(p);
	    }
	}
	ورنہ اگر (c == Ctrl_W)
	{
	    جب (p > ccline.cmdbuff && vim_isspace(p[-1]))
		--p;
	    اگر (p > ccline.cmdbuff)
	    {
		i = vim_iswordc(p[-1]);
		جب (p > ccline.cmdbuff && !vim_isspace(p[-1])
			&& vim_iswordc(p[-1]) == i)
		    --p;
	    }
	}
	ورنہ
	    --p;
	ccline.cmdpos = (صحیح)(p - ccline.cmdbuff);
	ccline.cmdlen -= j - ccline.cmdpos;
	i = ccline.cmdpos;
	جب (i < ccline.cmdlen)
	    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

	// Truncate at the end, required for multi-byte chars.
	ccline.cmdbuff[ccline.cmdlen] = NUL;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	اگر (ccline.cmdlen == 0)
	{
	    isp->تلاش_آغاز_2 = isp->save_cursor;
	    // save view settings, so that the screen
	    // won't be restored at the wrong position
	    isp->old_viewstate = isp->init_viewstate;
	}
#اگر_ختم
	redrawcmd();
    }
    ورنہ اگر (ccline.cmdlen == 0 && c != Ctrl_W
	    && ccline.cmdprompt == تہی && indent == 0)
    {
	// In ex and debug mode it doesn't make sense to return.
	اگر (exmode_فعال
#اگر_تعریف_شدہ FEAT_EVAL
		|| ccline.cmdfirstc == '>'
#اگر_ختم
	   )
	    واپس CMDLINE_NOT_بدلا;

	dealloc_cmdbuff();	// no commandline to return

	اگر (!cmd_silent)
	{
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (cmdmsg_rl)
		پیغام_ستون = cmdline_چوڑائی;
	    ورنہ
#اگر_ختم
		پیغام_ستون = 0;
	    پیغام_putchar(' ');		// delete ':'
	}
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	isp->تلاش_آغاز_2 = isp->save_cursor;
#اگر_ختم
	redraw_cmdline = TRUE;
	واپس GOTO_معمول_منوال;
    }
    واپس CMDLINE_بدلا;
}

/*
 * Handle the CTRL-^ key in the command-line mode and toggle the use of the
 * language :lmap mappings and/or Input Method.
 */
    ساکن خالی
cmdline_toggle_langmap(طویل *b_im_ptr)
{
    اگر (map_مقصد_exists_منوال((char_u *)"", منوال_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, toggle use of mappings.
	State ^= منوال_LANGMAP;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
	im_سیٹ_فعال(FALSE);	// Disable input method
#اگر_ختم
	اگر (b_im_ptr != تہی)
	{
	    اگر (State & منوال_LANGMAP)
		*b_im_ptr = B_IMODE_LMAP;
	    ورنہ
		*b_im_ptr = B_IMODE_NONE;
	}
    }
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    ورنہ
    {
	// There are no ":lmap" mappings, toggle IM.  When
	// 'imdisable' is set don't try getting the status, it's
	// always off.
	اگر ((p_imdisable && b_im_ptr != تہی)
		? *b_im_ptr == B_IMODE_IM : im_get_حالت())
	{
	    im_سیٹ_فعال(FALSE);	// Disable input method
	    اگر (b_im_ptr != تہی)
		*b_im_ptr = B_IMODE_NONE;
	}
	ورنہ
	{
	    im_سیٹ_فعال(TRUE);	// Enable input method
	    اگر (b_im_ptr != تہی)
		*b_im_ptr = B_IMODE_IM;
	}
    }
#اگر_ختم
    اگر (b_im_ptr != تہی)
    {
	اگر (b_im_ptr == &curbuf->b_p_iminsert)
	    سیٹ_iminsert_global();
	ورنہ
	    سیٹ_imsearch_global();
    }
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();	// may show different cursor shape
#اگر_ختم
#اگر defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines later.
    حالت_redraw_curbuf();
#اگر_ختم
}

/*
 * Handle the CTRL-R key in the command-line mode and insert the contents of a
 * numbered or named register.
 */
    ساکن صحیح
cmdline_داخل_کرو_reg(صحیح *gotesc غیر_مستعمل)
{
    صحیح		i;
    صحیح		c;
    صحیح		literally = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		save_new_cmdpos = new_cmdpos;
#اگر_ختم

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;	// disallow scrolling here
#اگر_ختم
    putcmdline('"', TRUE);
    ++no_mapping;
    ++allow_کلیدیں;
    i = c = plain_vgetc();	// CTRL-R <char>
    اگر (i == Ctrl_O)
	i = Ctrl_R;		// CTRL-R CTRL-O == CTRL-R CTRL-R
    اگر (i == Ctrl_R)
	c = plain_vgetc();	// CTRL-R CTRL-R <char>
    extra_char = NUL;
    --no_mapping;
    --allow_کلیدیں;
#اگر_تعریف_شدہ FEAT_EVAL
    /*
     * Insert the result of an expression.
     */
    new_cmdpos = -1;
    اگر (c == '=')
    {
	اگر (ccline.cmdfirstc == '='  // can't do this recursively
		|| cmdline_ستارہ > 0) // or when typing a password
	{
	    beep_flush();
	    c = ESC;
	}
	ورنہ
	    c = get_اظہاریہ_رجسٹر();
    }
#اگر_ختم
    اگر (c != ESC)	    // use ESC to cancel inserting register
    {
	literally = i == Ctrl_R
#اگر_تعریف_شدہ FEAT_CLIPBOARD
			|| (clip_ستارہ.دستیاب && c == '*')
			|| (clip_plus.دستیاب && c == '+')
#اگر_ختم
			;
	cmdline_paste(c, literally, FALSE);

#اگر_تعریف_شدہ FEAT_EVAL
	// When there was a serious error abort getting the
	// command line.
	اگر (aborting())
	{
	    *gotesc = TRUE;  // will free ccline.cmdbuff after
	    // putting it in history
	    واپس GOTO_معمول_منوال;
	}
#اگر_ختم
	کلید_Typed = FALSE;	// Don't do p_wc completion.
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (new_cmdpos >= 0)
	{
	    // set_cmdline_pos() was used
	    اگر (new_cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	    ورنہ
		ccline.cmdpos = new_cmdpos;
	}
#اگر_ختم
    }
#اگر_تعریف_شدہ FEAT_EVAL
    new_cmdpos = save_new_cmdpos;
#اگر_ختم

    // remove the double quote
    redrawcmd();

    // With "literally": the command line has already changed.
    // Else: the text has been stuffed, but the command line didn't change yet.
    واپس literally ? CMDLINE_بدلا : CMDLINE_NOT_بدلا;
}

/*
 * Handle the Left and Right mouse clicks in the command-line mode.
 */
    ساکن خالی
cmdline_بایاں_دایاں_mouse(صحیح c, صحیح *ignore_drag_release)
{
    اگر (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
	*ignore_drag_release = TRUE;
    ورنہ
	*ignore_drag_release = FALSE;
#اگر_تعریف_شدہ FEAT_GUI
    // When GUI is active, also move when 'mouse' is empty
    اگر (!gui.اندر_use)
#اگر_ختم
	اگر (!mouse_has(MOUSE_COMMAND))
	    واپس;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    اگر (mouse_صف < cmdline_صف && (clip_ستارہ.دستیاب || clip_plus.دستیاب))
    {
	صحیح	    button, is_click, is_drag;

	/*
	 * Handle modeless selection.
	 */
	button = get_mouse_button(KEY2TERMCAP1(c),
		&is_click, &is_drag);
	اگر (mouse_model_popup() && button == MOUSE_بایاں
		&& (mod_نقاب & MOD_نقاب_سرکاؤ))
	{
	    // Translate shift-left to right button.
	    button = MOUSE_دایاں;
	    mod_نقاب &= ~MOD_نقاب_سرکاؤ;
	}
	clip_modeless(button, is_click, is_drag);
	واپس;
    }
#اگر_ختم

    سیٹ_cmdspos();
    برائے (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
	    ++ccline.cmdpos)
    {
	صحیح	i;

	i = cmdline_charsize(ccline.cmdpos);
	اگر (mouse_صف <= cmdline_صف + ccline.cmdspos / cmdline_چوڑائی
		&& mouse_ستون < ccline.cmdspos % cmdline_چوڑائی + i)
	    توڑو;
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
    ساکن صحیح
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
	واپس CMDLINE_NOT_بدلا;

    // save current command string so it can be restored later
    اگر (lookfor == تہی)
    {
	اگر ((lookfor = vim_strnsave(ccline.cmdbuff, ccline.cmdlen)) == تہی)
	    واپس CMDLINE_NOT_بدلا;
	lookfor[ccline.cmdpos] = NUL;
	lookforlen = ccline.cmdpos;
    }

    برائے (;;)
    {
	// one step backwards
	اگر (c == K_اوپر || c == K_S_اوپر || c == Ctrl_P
		|| c == K_PAGEUP || c == K_KPAGEUP)
	{
	    اگر (hiscnt == get_hislen())	// first time
		hiscnt = *get_hisidx(histype);
	    ورنہ اگر (hiscnt == 0 && *get_hisidx(histype)
		    != get_hislen() - 1)
		hiscnt = get_hislen() - 1;
	    ورنہ اگر (hiscnt != *get_hisidx(histype) + 1)
		--hiscnt;
	    ورنہ			// at top of list
	    {
		hiscnt = orig_hiscnt;
		توڑو;
	    }
	}
	ورنہ    // one step forwards
	{
	    // on last entry, clear the line
	    اگر (hiscnt == *get_hisidx(histype))
	    {
		hiscnt = get_hislen();
		توڑو;
	    }

	    // not on a history line, nothing to do
	    اگر (hiscnt == get_hislen())
		توڑو;
	    اگر (hiscnt == get_hislen() - 1)   // wrap around
		hiscnt = 0;
	    ورنہ
		++hiscnt;
	}
	اگر (hiscnt < 0 || get_histentry(histype)[hiscnt].hisstr
		== تہی)
	{
	    hiscnt = orig_hiscnt;
	    توڑو;
	}
	اگر ((c != K_اوپر && c != K_نیچے_کو)
		|| hiscnt == orig_hiscnt
		|| STRNCMP(get_histentry(histype)[hiscnt].hisstr,
		    lookfor, lookforlen) == 0)
	    توڑو;
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
	ورنہ
	{
	    p = get_histentry(histype)[hiscnt].hisstr;
	    plen = get_histentry(histype)[hiscnt].hisstrlen;
	}

	اگر (histype == HIST_تلاش
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
	    برائے (i = 0; i <= 1; ++i)
	    {
		len = 0;
		برائے (j = 0; p[j] != NUL; ++j)
		{
		    // Replace old sep with new sep, unless it is
		    // escaped.
		    اگر (p[j] == old_firstc
			    && (j == 0 || p[j - 1] != '\\'))
		    {
			اگر (i > 0)
			    ccline.cmdbuff[len] = firstc;
		    }
		    ورنہ
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
		    اگر (ccline.cmdbuff == تہی)
		    {
			res = GOTO_معمول_منوال;
			جاؤ مکمل;
		    }
		}
	    }
	    ccline.cmdbuff[len] = NUL;
	    ccline.cmdpos = ccline.cmdlen = (صحیح)len;
	}
	ورنہ
	{
	    alloc_cmdbuff(plen);
	    اگر (ccline.cmdbuff == تہی)
	    {
		res = GOTO_معمول_منوال;
		جاؤ مکمل;
	    }
	    STRCPY(ccline.cmdbuff, p);
	    ccline.cmdpos = ccline.cmdlen = (صحیح)plen;
	}

	redrawcmd();
	res = CMDLINE_بدلا;
	جاؤ مکمل;
    }
    beep_flush();
    res = CMDLINE_NOT_بدلا;

مکمل:
    *curcmdstr = lookfor;
    *curcmdstrlen = lookforlen;
    *hiscnt_p = hiscnt;
    واپس res;
}

/*
 * Initialize the current command-line info.
 */
    ساکن صحیح
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
    اگر (ccline.cmdbuff == تہی)
    {
	ccline.cmdbufflen = 0;
	واپس FAIL;
    }
    ccline.cmdbuff[0] = NUL;
    sb_متن_آغاز_cmdline();

    // autoindent for :insert and :append
    اگر (firstc <= 0)
    {
	vim_memset(ccline.cmdbuff, ' ', indent);
	ccline.cmdbuff[indent] = NUL;
	ccline.cmdpos = indent;
	ccline.cmdspos = indent;
	ccline.cmdlen = indent;
    }

    واپس OK;
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
    طویل	  گنتی,	// only used for incremental search
    صحیح		  indent,	// indent for inside conditionals
    getline_opt_T do_concat غیر_مستعمل)
{
    واپس getcmdline_int(firstc, گنتی, indent, TRUE);
}

    ساکن char_u *
getcmdline_int(
    صحیح		firstc,
    طویل	گنتی غیر_مستعمل,	// only used for incremental search
    صحیح		indent,		// indent for inside conditionals
    صحیح		clear_ccline)	// clear ccline first
{
    ساکن صحیح	گہرائی = 0;	    // call depth
    صحیح		c = 0;
    صحیح		i;
    صحیح		j;
    صحیح		gotesc = FALSE;		// TRUE when <ESC> just typed
    صحیح		do_abbr;		// when TRUE check for abbr.
    char_u	*lookfor = تہی;	// string to match
    size_t	lookforlen = 0;
    صحیح		hiscnt;			// current history line in use
    صحیح		histype;		// history type to be used
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    incsearch_state_T	is_state;
#اگر_ختم
    صحیح		did_wild_list = FALSE;	// did wild_list() recently
    صحیح		wim_اشاریہ = 0;		// index in wim_flags[]
    صحیح		res;
    صحیح		save_پیغام_scroll = پیغام_scroll;
    صحیح		save_State = State;	// remember State when called
    صحیح		some_کلید_typed = FALSE;	// one of the keys was typed
    // mouse drag and release events are ignored, unless they are
    // preceded with a mouse down event
    صحیح		ignore_drag_release = TRUE;
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		break_ctrl_c = FALSE;
#اگر_ختم
    expand_T	xpc;
    طویل	*b_im_ptr = تہی;
    buf_T	*b_im_ptr_buf = تہی;	// buffer where b_im_ptr is valid
    cmdline_info_T save_ccline;
    صحیح		did_save_ccline = FALSE;
    صحیح		cmdline_type;
    صحیح		wild_type = 0;
    صحیح		event_cmdlineleavepre_triggered = FALSE;
    char_u	*پچھلا_cmdbuff = تہی;
    صحیح		did_hist_navigate = FALSE;

    // one recursion level deeper
    ++گہرائی;

    اگر (ccline.cmdbuff != تہی)
    {
	// Being called recursively.  Since ccline is global, we need to save
	// the current buffer and restore it when returning.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }
    اگر (clear_ccline)
	CLEAR_میدان(ccline);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    // start without Hebrew mapping for a command line
    اگر (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    init_incsearch_state(&is_state);
#اگر_ختم

    اگر (init_ccline(firstc, indent) != OK)
	جاؤ theend;	// out of memory

    اگر (گہرائی == 50)
    {
	// Somehow got into a loop recursively calling getcmdline(), bail out.
	emsg(_(e_command_too_recursive));
	جاؤ theend;
    }

    ExpandInit(&xpc);
    ccline.xpc = &xpc;
    clear_cmdline_orig();

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    ورنہ
	cmdmsg_rl = FALSE;
#اگر_ختم

    redir_off = TRUE;		// don't redirect the typed command
    اگر (!cmd_silent)
    {
	i = پیغام_scrolled;
	پیغام_scrolled = 0;		// avoid wait_return() message
	gotocmdline(TRUE);
	پیغام_scrolled += i;
	redrawcmdprompt();		// draw prompt or indent
	سیٹ_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;
#اگر_غیر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    xpc.xp_shell = FALSE;
#اگر_ختم

#اگر defined(FEAT_EVAL)
    اگر (ccline.داخلہ_fn)
    {
	xpc.xp_context = ccline.xp_context;
	xpc.xp_نمونہ = ccline.cmdbuff;
	xpc.xp_arg = ccline.xp_arg;
    }
#اگر_ختم

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    پیغام_scroll = FALSE;

    State = منوال_CMDLINE;

    اگر (firstc == '/' || firstc == '?' || firstc == '@')
    {
	// Use ":lmap" mappings for search pattern and input().
	اگر (curbuf->b_p_imsearch == B_IMODE_USE_داخل_کرو)
	    b_im_ptr = &curbuf->b_p_iminsert;
	ورنہ
	    b_im_ptr = &curbuf->b_p_imsearch;
	b_im_ptr_buf = curbuf;
	اگر (*b_im_ptr == B_IMODE_LMAP)
	    State |= منوال_LANGMAP;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
	im_سیٹ_فعال(*b_im_ptr == B_IMODE_IM);
#اگر_ختم
    }
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    ورنہ اگر (p_imcmdline)
	im_سیٹ_فعال(TRUE);
#اگر_ختم

    setmouse();
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم

    // When inside an autocommand for writing "exiting" may be set and
    // terminal mode set to cooked.  Need to set raw mode here then.
    settmode(TMODE_RAW);

    // Trigger CmdlineEnter autocommands.
    cmdline_type = firstc == NUL ? '-' : firstc;
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINEENTER);
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (!debug_منوال)
	may_trigger_modechanged();
#اگر_ختم

    init_history();
    hiscnt = get_hislen();	// set hiscnt to impossible history value
    histype = hist_char2type(firstc);

#اگر_تعریف_شدہ FEAT_DIGRAPHS
    do_digraph(-1);		// init digraph typeahead
#اگر_ختم

    // If something above caused an error, reset the flags, we do want to type
    // and execute commands. Display may be messed up a bit.
    اگر (did_emsg)
	redrawcmd();

#اگر_تعریف_شدہ FEAT_STL_OPT
    // Redraw the statusline in case it uses the current mode using the mode()
    // function.
    اگر (!cmd_silent && پیغام_scrolled == 0)
    {
	صحیح	ملا_one = FALSE;
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    اگر (*p_stl != NUL || *wp->w_p_stl != NUL)
	    {
		wp->w_redr_حالت = true;
		ملا_one = TRUE;
	    }

	اگر (*p_tal != NUL)
	{
	    redraw_tabline = TRUE;
	    ملا_one = TRUE;
	}

	اگر (ملا_one)
	    redraw_statuslines();
    }
#اگر_ختم

    did_emsg = FALSE;
    got_int = FALSE;

    /*
     * Collect the command string, handling editing keys.
     */
    برائے (;;)
    {
	صحیح	trigger_cmdlinechanged = TRUE;
	صحیح	انتہا_wildmenu;
	صحیح	پچھلا_cmdpos = ccline.cmdpos;
	صحیح	skip_pum_redraw = FALSE;

	VIM_CLEAR(پچھلا_cmdbuff);

	redir_off = TRUE;	// Don't redirect the typed command.
				// Repeated, because a ":redir" inside
				// completion may switch it on.
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	// allow scrolling here
#اگر_ختم
	quit_مزید = FALSE;	// reset after CTRL-D which had a more-prompt

	did_emsg = FALSE;	// There can't really be a reason why an error
				// that occurs while typing a command should
				// cause the command not to be executed.

	اگر (ex_معمول_busy == 0 && stuff_خالی_خانے() && typebuf.tb_len == 0)
	    // There is no pending input from sources other than user input, so
	    // Vim is going to wait for the user to type a key.  Consider the
	    // command line typed even if next key will trigger a mapping.
	    some_کلید_typed = TRUE;

	// Trigger SafeState if nothing is pending.
	may_trigger_safestate(xpc.xp_numfiles <= 0);

	اگر (ccline.cmdbuff != تہی)
	{
	    پچھلا_cmdbuff = vim_strsave(ccline.cmdbuff);
	    اگر (پچھلا_cmdbuff == تہی)
		جاؤ returncmd;
	}

	// Defer screen update to avoid pum flicker during wildtrigger()
	اگر (c == K_WILD && firstc != '@')
	    skip_pum_redraw = TRUE;

	// Get a character.  Ignore K_IGNORE and K_NOP, they should not do
	// anything, such as stop completion.
	کرو
	{
	    cursorcmd();		// set the cursor on the right spot
	    c = محفوظ_vgetc();
	} جب (c == K_IGNORE || c == K_NOP);

	// If the cmdline was replaced externally (e.g. by setcmdline()
	// during an <expr> mapping), clean up the wildmenu completion
	// state to avoid using stale completion data.
	اگر (ccline.cmdbuff_replaced && xpc.xp_numfiles > 0)
	{
	    اگر (cmdline_pum_فعال())
		cmdline_pum_remove(&ccline, FALSE);
	    (خالی)ExpandOne(&xpc, تہی, تہی, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    xpc.xp_context = EXPAND_NOTHING;
	    wim_اشاریہ = 0;
	    wildmenu_cleanup(&ccline);
	}
	ccline.cmdbuff_replaced = FALSE;

	// Skip wildmenu during history navigation via Up/Down keys
	اگر (c == K_WILD && did_hist_navigate)
	{
	    did_hist_navigate = FALSE;
	    جاری;
	}

	اگر (c == K_COMMAND || c == K_SCRIPT_COMMAND)
	{
	    صحیح	    clen = ccline.cmdlen;
	    صحیح	    cc_گنتی = aucmd_cmdline_بدلا_گنتی;

	    اگر (do_cmdkey_command(c, DOCMD_NOWAIT) == OK)
	    {
		// Do not trigger CmdlineChanged below if:
		// - the length of the command line didn't change
		// - the <Cmd> mapping already triggered the event
		اگر (clen == ccline.cmdlen
				    || cc_گنتی != aucmd_cmdline_بدلا_گنتی)
		    trigger_cmdlinechanged = FALSE;
		جاؤ cmdline_بدلا;
	    }
	}

	اگر (کلید_Typed)
	{
	    some_کلید_typed = TRUE;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    اگر (cmd_hkmap)
		c = hkmap(c);
	    اگر (cmdmsg_rl && !کلید_Stuffed)
	    {
		// Invert horizontal movements and operations.  Only when
		// typed by the user directly, not when the result of a
		// mapping.
		انتخاب (c)
		{
		    حالت K_دایاں:   c = K_بایاں; توڑو;
		    حالت K_S_دایاں: c = K_S_بایاں; توڑو;
		    حالت K_C_دایاں: c = K_C_بایاں; توڑو;
		    حالت K_بایاں:    c = K_دایاں; توڑو;
		    حالت K_S_بایاں:  c = K_S_دایاں; توڑو;
		    حالت K_C_بایاں:  c = K_C_دایاں; توڑو;
		}
	    }
#اگر_ختم
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	اگر ((c == Ctrl_C
#اگر_تعریف_شدہ UNIX
		|| c == intr_char
#اگر_ختم
				)
#اگر defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#اگر_ختم
#اگر_تعریف_شدہ FEAT_EVAL
		// do clear got_int in Ex mode to avoid infinite Ctrl-C loop
		&& (!break_ctrl_c || exmode_فعال)
#اگر_ختم
		&& !global_busy)
	    got_int = FALSE;

	// free old command line when finished moving around in the history
	// list
	اگر (lookfor != تہی
		&& c != K_S_نیچے_کو && c != K_S_اوپر
		&& c != K_نیچے_کو && c != K_اوپر
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_بایاں && c != K_دایاں
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
	اگر ((cmdline_pum_فعال() || wild_menu_showing || did_wild_list)
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
		    توڑو;
	    }
	}

	// Trigger CmdlineLeavePre autocommand
	اگر (کلید_Typed && (c == '\n' || c == '\r' || c == K_KENTER || c == ESC
#اگر_تعریف_شدہ UNIX
		    || c == intr_char
#اگر_ختم
		    || c == Ctrl_C))
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    سیٹ_vim_var_char(c);  // Set v:char
#اگر_ختم
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
	    event_cmdlineleavepre_triggered = TRUE;
#اگر defined(FEAT_تلاش_EXTRA)
	    اگر ((c == ESC || c == Ctrl_C) && (wim_flags[0] & WIM_LIST))
		سیٹ_no_hlsearch(TRUE);
#اگر_ختم
	}

	// The wildmenu is cleared if the pressed key is not used for
	// navigating the wild menu (i.e. the key is not 'wildchar' or
	// 'wildcharm' or Ctrl-N or Ctrl-P or Ctrl-A or Ctrl-L).
	// If the popup menu is displayed, then PageDown and PageUp keys are
	// also used to navigate the menu, the mouse scroll wheel keys scroll
	// the info popup, and CTRL-SHIFT-<Up>/<Down> scroll it with the
	// keyboard.
	انتہا_wildmenu = (!کلید_is_wc
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A && c != Ctrl_L);
	انتہا_wildmenu = انتہا_wildmenu && (!cmdline_pum_فعال() ||
			    (c != K_PAGEDOWN && c != K_PAGEUP
			     && c != K_KPAGEDOWN && c != K_KPAGEUP
			     && c != K_MOUSEDOWN && c != K_MOUSEUP
			     && c != K_MOUSELEFT && c != K_MOUSERIGHT
			     && !((c == K_S_اوپر || c == K_S_نیچے_کو)
				 && (mod_نقاب & MOD_نقاب_CTRL))));

	// free expanded names when finished walking through matches
	اگر (انتہا_wildmenu)
	{
	    اگر (cmdline_pum_فعال())
	    {
		skip_pum_redraw = skip_pum_redraw && !کلید_is_wc
		    && !VIM_ISWHITE(c)
		    && (vim_isprintc(c)
			|| c == K_BS || c == Ctrl_H || c == K_DEL
			|| c == K_KDEL || c == Ctrl_W || c == Ctrl_U);
		cmdline_pum_remove(&ccline, skip_pum_redraw);
	    }
	    اگر (xpc.xp_numfiles != -1)
		(خالی)ExpandOne(&xpc, تہی, تہی, 0, WILD_FREE);
	    did_wild_list = FALSE;
	    اگر (!p_wmnu || (c != K_اوپر && c != K_نیچے_کو))
		xpc.xp_context = EXPAND_NOTHING;
	    wim_اشاریہ = 0;
	    wildmenu_cleanup(&ccline);
	}

	اگر (p_wmnu)
	    c = wildmenu_process_کلید(&ccline, c, &xpc);

	// CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	// mode when 'insertmode' is set, CTRL-\ e prompts for an expression.
	اگر (c == Ctrl_BSL)
	{
	    res = cmdline_handle_ctrl_bsl(c, &gotesc);
	    اگر (res == CMDLINE_بدلا)
		جاؤ cmdline_بدلا;
	    ورنہ اگر (res == CMDLINE_NOT_بدلا)
		جاؤ cmdline_not_بدلا;
	    ورنہ اگر (res == GOTO_معمول_منوال)
		جاؤ returncmd;		// back to cmd mode
	    c = Ctrl_BSL;		// backslash key not processed by
					// cmdline_handle_ctrl_bsl()
	}

	اگر (c == cedit_کلید || c == K_CMDWIN)
	{
	    // TODO: why is ex_normal_busy checked here?
	    اگر ((c == K_CMDWIN || ex_معمول_busy == 0) && got_int == FALSE)
	    {
		/*
		 * Open a window to edit the command line (and history).
		 */
		c = open_cmdwin();
		some_کلید_typed = TRUE;
	    }
	}
#اگر_تعریف_شدہ FEAT_DIGRAPHS
	ورنہ
	    c = do_digraph(c);
#اگر_ختم

	اگر (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!کلید_Typed || vim_strchr(p_cpo, CPO_ESC) != تہی)))
	{
	    // In Ex mode a backslash escapes a newline.
	    اگر (exmode_فعال
		    && c != ESC
		    && ccline.cmdpos == ccline.cmdlen
		    && ccline.cmdpos > 0
		    && ccline.cmdbuff[ccline.cmdpos - 1] == '\\')
	    {
		اگر (c == K_KENTER)
		    c = '\n';
	    }
	    ورنہ
	    {
		gotesc = FALSE;	// Might have typed ESC previously, don't
				// truncate the cmdline now.
		اگر (ccheck_abbr(c + ABBR_OFF))
		    جاؤ cmdline_بدلا;
		اگر (!cmd_silent)
		{
		    windgoto(پیغام_صف, cmdline_ستون_off);
		    برآمد_flush();
		}
		توڑو;
	    }
	}

	// Completion for 'wildchar', 'wildcharm', and wildtrigger()
	اگر ((c == p_wc && !gotesc && کلید_Typed) || c == p_wcm || c == K_WILD)
	{
	    اگر (c == K_WILD)
		++emsg_silent;  // Silence the bell
	    res = cmdline_wildchar_complete(c, firstc != '@', &did_wild_list,
		    &wim_اشاریہ, &xpc, &gotesc, c == K_WILD,
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		    &is_state.تلاش_آغاز_2
#ورنہ
		    تہی
#اگر_ختم
		    );
	    اگر (c == K_WILD)
		--emsg_silent;
	    اگر (res == CMDLINE_بدلا)
		جاؤ cmdline_بدلا;
	    اگر (c == K_WILD)
		جاؤ cmdline_not_بدلا;
	}

	gotesc = FALSE;

	// <S-Tab> goes to last match, in a clumsy way
	اگر (c == K_S_TAB && کلید_Typed)
	{
	    اگر (nextwild(&xpc, WILD_EXPAND_KEEP, 0, firstc != '@') == OK)
	    {
		اگر (xpc.xp_numfiles > 1
		    && ((!did_wild_list && (wim_flags[wim_اشاریہ] & WIM_LIST))
			    || p_wmnu))
		{
		    // Trigger the popup menu when wildoptions=pum
		    صحیح list = wim_flags[wim_اشاریہ] & WIM_LIST;

		    showmatches(&xpc, p_wmnu && !list, list,
			    p_wmnu ? wim_flags[0] : 0);
		}
		اگر (nextwild(&xpc, WILD_پچھلا, 0, firstc != '@') == OK
			&& nextwild(&xpc, WILD_پچھلا, 0, firstc != '@') == OK)
		    جاؤ cmdline_بدلا;
	    }
	}

	اگر (c == NUL || c == K_ZERO)	    // NUL is stored as NL
	    c = NL;

	do_abbr = TRUE;		// default: check for abbreviation

	// If already used to cancel/accept wildmenu, don't process the key
	// further.
	اگر (wild_type == WILD_CANCEL || wild_type == WILD_APPLY)
	{
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	    // Apply search highlighting
	    اگر (is_state.winid != curwin->w_id)
		init_incsearch_state(&is_state);
	    اگر (کلید_Typed || vpeekc() == NUL)
		may_do_incsearch_highlighting(firstc, گنتی, &is_state);
#اگر_ختم
	    wild_type = 0;
	    جاؤ cmdline_not_بدلا;
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
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		    , &is_state
#اگر_ختم
		    );
	    اگر (res == CMDLINE_NOT_بدلا)
		جاؤ cmdline_not_بدلا;
	    ورنہ اگر (res == GOTO_معمول_منوال)
		جاؤ returncmd;		// back to cmd mode
	    جاؤ cmdline_بدلا;

	حالت K_INS:
	حالت K_KINS:
		ccline.overstrike = !ccline.overstrike;
#اگر_تعریف_شدہ CURSOR_SHAPE
		ui_cursor_shape();	// may show different cursor shape
#اگر_ختم
		may_trigger_modechanged();
		حالت_redraw_curbuf();
		redraw_statuslines();
		جاؤ cmdline_not_بدلا;

	حالت Ctrl_HAT:
		cmdline_toggle_langmap(
				    buf_valid(b_im_ptr_buf) ? b_im_ptr : تہی);
		جاؤ cmdline_not_بدلا;

//	case '@':   only in very old vi
	حالت Ctrl_U:
		// delete all characters left of the cursor
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		جب (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		// Truncate at the end, required for multi-byte chars.
		ccline.cmdbuff[ccline.cmdlen] = NUL;
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		اگر (ccline.cmdlen == 0)
		    is_state.تلاش_آغاز_2 = is_state.save_cursor;
#اگر_ختم
		redrawcmd();
		جاؤ cmdline_بدلا;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
	حالت Ctrl_Y:
		// Copy the modeless selection, if there is one.
		اگر (clip_ستارہ.state != SELECT_CLEARED)
		{
		    اگر (clip_ستارہ.state == SELECT_مکمل)
			clip_copy_modeless_selection(TRUE);
		    جاؤ cmdline_not_بدلا;
		}
		توڑو;
#اگر_ختم

	حالت ESC:	// get here if p_wc != ESC or when ESC typed twice
	حالت Ctrl_C:
		// In exmode it doesn't make sense to return.  Except when
		// ":normal" runs out of characters.
		اگر (exmode_فعال
			       && (ex_معمول_busy == 0 || typebuf.tb_len > 0))
		    جاؤ cmdline_not_بدلا;

		gotesc = TRUE;		// will free ccline.cmdbuff after
					// putting it in history
		جاؤ returncmd;		// back to cmd mode

	حالت Ctrl_R:			// insert register
		res = cmdline_داخل_کرو_reg(&gotesc);
		اگر (res == GOTO_معمول_منوال)
		    جاؤ returncmd;
		اگر (res == CMDLINE_بدلا)
		    جاؤ cmdline_بدلا;
		جاؤ cmdline_not_بدلا;

	حالت Ctrl_D:
		اگر (showmatches(&xpc, FALSE, TRUE, p_wmnu ? wim_flags[0] : 0)
			== EXPAND_NOTHING)
		    توڑو;	// Use ^D as normal char instead

		redrawcmd();
		جاری;	// don't do incremental search now

	حالت K_دایاں:
	حالت K_S_دایاں:
	حالت K_C_دایاں:
		کرو
		{
		    اگر (ccline.cmdpos >= ccline.cmdlen)
			توڑو;
		    i = cmdline_charsize(ccline.cmdpos);
		    اگر (کلید_Typed && ccline.cmdspos + i >= cmdline_چوڑائی * صفیں)
			توڑو;
		    ccline.cmdspos += i;
		    اگر (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len)(ccline.cmdbuff
							     + ccline.cmdpos);
		    ورنہ
			++ccline.cmdpos;
		}
		جب ((c == K_S_دایاں || c == K_C_دایاں
			       || (mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		اگر (has_mbyte)
		    سیٹ_cmdspos_cursor();
		جاؤ cmdline_not_بدلا;

	حالت K_بایاں:
	حالت K_S_بایاں:
	حالت K_C_بایاں:
		اگر (ccline.cmdpos == 0)
		    جاؤ cmdline_not_بدلا;
		کرو
		{
		    --ccline.cmdpos;
		    اگر (has_mbyte)	// move to first byte of char
			ccline.cmdpos -= (*mb_سر_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		جب (ccline.cmdpos > 0
			&& (c == K_S_بایاں || c == K_C_بایاں
			       || (mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_CTRL)))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		اگر (has_mbyte)
		    سیٹ_cmdspos_cursor();
		جاؤ cmdline_not_بدلا;

	حالت K_IGNORE:
		// Ignore mouse event or open_cmdwin() result.
		جاؤ cmdline_not_بدلا;

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالت K_F4:
	    اگر (mod_نقاب == MOD_نقاب_ALT)
	    {
		redrawcmd();	    // somehow the cmdline is cleared
		جاؤ cmdline_not_بدلا;
	    }
	    توڑو;
#اگر_ختم

	حالت K_MIDDLEDRAG:
	حالت K_MIDDLERELEASE:
		جاؤ cmdline_not_بدلا;	// Ignore mouse

	حالت K_MIDDLEMOUSE:
#اگر_تعریف_شدہ FEAT_GUI
		// When GUI is active, also paste when 'mouse' is empty
		اگر (!gui.اندر_use)
#اگر_ختم
		    اگر (!mouse_has(MOUSE_COMMAND))
			جاؤ cmdline_not_بدلا;   // Ignore mouse
#اگر_تعریف_شدہ FEAT_CLIPBOARD
		اگر (clip_ستارہ.دستیاب)
		    cmdline_paste('*', TRUE, TRUE);
		ورنہ
#اگر_ختم
		    cmdline_paste(0, TRUE, TRUE);
		redrawcmd();
		جاؤ cmdline_بدلا;

#اگر_تعریف_شدہ FEAT_DND
	حالت K_DROP:
		cmdline_paste('~', TRUE, FALSE);
		redrawcmd();
		جاؤ cmdline_بدلا;
#اگر_ختم

	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:
		// Ignore drag and release events when the button-down wasn't
		// seen before.
		اگر (ignore_drag_release)
		    جاؤ cmdline_not_بدلا;
		// FALLTHROUGH
	حالت K_LEFTMOUSE:
	حالت K_RIGHTMOUSE:
		cmdline_بایاں_دایاں_mouse(c, &ignore_drag_release);
		جاؤ cmdline_not_بدلا;

	// Mouse scroll wheel: scroll the completion info popup when the mouse
	// is on top of it, otherwise ignored here.
	حالت K_MOUSEDOWN:
	حالت K_MOUSEUP:
	حالت K_MOUSELEFT:
	حالت K_MOUSERIGHT:
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		اگر (cmdline_pum_فعال())
		    cmdline_mousescroll(c == K_MOUSEDOWN ? MSCR_نیچے_کو
				      : c == K_MOUSEUP ? MSCR_اوپر
				      : c == K_MOUSELEFT ? MSCR_بایاں
				      : MSCR_دایاں);
#اگر_ختم
		جاؤ cmdline_not_بدلا;

	// Alternate buttons ignored here
	حالت K_X1MOUSE:
	حالت K_X1DRAG:
	حالت K_X1RELEASE:
	حالت K_X2MOUSE:
	حالت K_X2DRAG:
	حالت K_X2RELEASE:
	حالت K_MOUSEMOVE:
		جاؤ cmdline_not_بدلا;

#اگر_تعریف_شدہ FEAT_GUI
	حالت K_LEFTMOUSE_NM:	// mousefocus click, ignored
	حالت K_LEFTRELEASE_NM:
		جاؤ cmdline_not_بدلا;

	حالت K_VER_SCROLLBAR:
		اگر (پیغام_scrolled == 0)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		جاؤ cmdline_not_بدلا;

	حالت K_HOR_SCROLLBAR:
		اگر (پیغام_scrolled == 0)
		{
		    do_mousescroll_horiz(scrollbar_قدر);
		    redrawcmd();
		}
		جاؤ cmdline_not_بدلا;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	حالت K_TABLINE:
	حالت K_TABMENU:
		// Don't want to change any tabs here.  Make sure the same tab
		// is still selected.
		اگر (gui_use_tabline())
		    gui_mch_سیٹ_curtab(tabpage_اشاریہ(curtab));
		جاؤ cmdline_not_بدلا;
#اگر_ختم

	حالت K_SELECT:	    // end of Select mode mapping - ignore
		جاؤ cmdline_not_بدلا;

	حالت Ctrl_B:	    // begin of command line
	حالت K_HOME:
	حالت K_KHOME:
	حالت K_S_HOME:
	حالت K_C_HOME:
		ccline.cmdpos = 0;
		سیٹ_cmdspos();
		جاؤ cmdline_not_بدلا;

	حالت Ctrl_E:	    // end of command line
	حالت K_انتہا:
	حالت K_KEND:
	حالت K_S_انتہا:
	حالت K_C_انتہا:
		ccline.cmdpos = ccline.cmdlen;
		سیٹ_cmdspos_cursor();
		جاؤ cmdline_not_بدلا;

	حالت Ctrl_A:	    // all matches
		اگر (cmdline_pum_فعال())
		    // As Ctrl-A completes all the matches, close the popup
		    // menu (if present)
		    cmdline_pum_cleanup(&ccline);

		اگر (nextwild(&xpc, WILD_ALL, 0, firstc != '@') == FAIL)
		    توڑو;
		xpc.xp_context = EXPAND_NOTHING;
		did_wild_list = FALSE;
		جاؤ cmdline_بدلا;

	حالت Ctrl_L:
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
		اگر (may_جوڑو_char_مقصد_تلاش(firstc, &c, &is_state) == OK)
		    جاؤ cmdline_not_بدلا;
#اگر_ختم

		// completion: longest common part
		اگر (nextwild(&xpc, WILD_LONGEST, 0, firstc != '@') == FAIL)
		    توڑو;
		جاؤ cmdline_بدلا;

	حالت Ctrl_N:	    // next match
	حالت Ctrl_P:	    // previous match
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-P/N scroll the info popup one line.
		اگر (cmdline_pum_فعال() && (mod_نقاب & MOD_نقاب_سرکاؤ))
		{
		    اگر (popup_scroll_info(c == Ctrl_P ? -1 : 1, false))
			cmdline_pum_display();
		    جاؤ cmdline_not_بدلا;
		}
#اگر_ختم
		اگر (xpc.xp_numfiles > 0)
		{
		    wild_type = (c == Ctrl_P) ? WILD_پچھلا : WILD_اگلا;
		    اگر (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			توڑو;
		    جاؤ cmdline_بدلا;
		}
		// FALLTHROUGH
	حالت K_اوپر:
	حالت K_نیچے_کو:
	حالت K_S_اوپر:
	حالت K_S_نیچے_کو:
	حالت K_PAGEUP:
	حالت K_KPAGEUP:
	حالت K_PAGEDOWN:
	حالت K_KPAGEDOWN:
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up>/<Down> scroll the info popup a line,
		// CTRL-SHIFT-<PageUp>/<PageDown> a page.  Shift is folded into
		// K_S_UP/K_S_DOWN but stays in mod_mask for the Page keys.
		اگر (cmdline_pum_فعال()
			&& ((c == K_S_اوپر || c == K_S_نیچے_کو)
			    ? (mod_نقاب & MOD_نقاب_CTRL)
			    : ((c == K_PAGEUP || c == K_KPAGEUP
				    || c == K_PAGEDOWN || c == K_KPAGEDOWN)
				&& (mod_نقاب & MOD_نقاب_CTRL)
				&& (mod_نقاب & MOD_نقاب_سرکاؤ))))
		{
		    صحیح	اوپر = c == K_S_اوپر || c == K_PAGEUP
							|| c == K_KPAGEUP;

		    اگر (popup_scroll_info(اوپر ? -1 : 1,
					     c != K_S_اوپر && c != K_S_نیچے_کو))
			cmdline_pum_display();
		    جاؤ cmdline_not_بدلا;
		}
#اگر_ختم
		اگر (cmdline_pum_فعال()
			&& (c == K_PAGEUP || c == K_PAGEDOWN ||
			    c == K_KPAGEUP || c == K_KPAGEDOWN))
		{
		    // If the popup menu is displayed, then PageUp and PageDown
		    // are used to scroll the menu.
		    wild_type = WILD_PAGEUP;
		    اگر (c == K_PAGEDOWN || c == K_KPAGEDOWN)
			wild_type = WILD_PAGEDOWN;
		    اگر (nextwild(&xpc, wild_type, 0, firstc != '@') == FAIL)
			توڑو;
		    جاؤ cmdline_بدلا;
		}
		ورنہ
		{
		    res = cmdline_browse_history(c, firstc, &lookfor, &lookforlen, histype,
			    &hiscnt, &xpc);
		    اگر (res == CMDLINE_بدلا)
		    {
			did_hist_navigate = TRUE;
			جاؤ cmdline_بدلا;
		    }
		    ورنہ اگر (res == GOTO_معمول_منوال)
			جاؤ returncmd;
		}
		جاؤ cmdline_not_بدلا;

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	حالت Ctrl_G:	    // next match
	حالت Ctrl_T:	    // previous match
		اگر (may_adjust_incsearch_highlighting(
					  firstc, گنتی, &is_state, c) == FAIL)
		    جاؤ cmdline_not_بدلا;
		توڑو;
#اگر_ختم

	حالت Ctrl_V:
	حالت Ctrl_Q:
		{
		    ignore_drag_release = TRUE;
		    putcmdline('^', TRUE);

		    // Get next (two) character(s).  Do not change any
		    // modifyOtherKeys ESC sequence to a normal key for
		    // CTRL-SHIFT-V.
		    c = get_literal(mod_نقاب & MOD_نقاب_سرکاؤ);

		    do_abbr = FALSE;	    // don't do abbreviation now
		    extra_char = NUL;
		    // may need to remove ^ when composing char was typed
		    اگر (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		    {
			draw_cmdline(ccline.cmdpos,
						ccline.cmdlen - ccline.cmdpos);
			پیغام_putchar(' ');
			cursorcmd();
		    }
		}

		توڑو;

#اگر_تعریف_شدہ FEAT_DIGRAPHS
	حالت Ctrl_K:
		ignore_drag_release = TRUE;
		putcmdline('?', TRUE);
# ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    // disallow scrolling here
# endif
		c = get_digraph(TRUE);
		extra_char = NUL;
		اگر (c != NUL)
		    توڑو;

		redrawcmd();
		جاؤ cmdline_not_بدلا;
#اگر_ختم // FEAT_DIGRAPHS

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	حالت Ctrl__:	    // CTRL-_: switch language mode
		اگر (!p_ari)
		    توڑو;
		cmd_hkmap = !cmd_hkmap;
		جاؤ cmdline_not_بدلا;
#اگر_ختم

	حالت K_PS:
		bracketed_paste(PASTE_CMDLINE, FALSE, تہی);
		جاؤ cmdline_بدلا;

	طےشدہ:
#اگر_تعریف_شدہ UNIX
		اگر (c == intr_char)
		{
		    gotesc = TRUE;	// will free ccline.cmdbuff after
					// putting it in history
		    جاؤ returncmd;	// back to Normal mode
		}
#اگر_ختم
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		اگر (!IS_SPECIAL(c))
		    mod_نقاب = 0x0;
		توڑو;
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
	    جاؤ cmdline_بدلا;

	/*
	 * put the character in the command line
	 */
	اگر (IS_SPECIAL(c) || mod_نقاب != 0)
	    put_on_cmdline(get_special_کلید_name(c, mod_نقاب), -1, TRUE);
	ورنہ
	{
	    اگر (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	// exclude composing chars
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    ورنہ
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	جاؤ cmdline_بدلا;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_بدلا:
	// Trigger CursorMovedC autocommands.
	اگر (ccline.cmdpos != پچھلا_cmdpos)
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);
	    پچھلا_cmdpos = ccline.cmdpos;
	}

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	اگر (!is_state.incsearch_postponed)
	    جاری;
#اگر_ختم

cmdline_بدلا:
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	// If the window changed incremental search state is not valid.
	اگر (is_state.winid != curwin->w_id)
	    init_incsearch_state(&is_state);
	اگر (xpc.xp_context == EXPAND_NOTHING && (کلید_Typed || vpeekc() == NUL))
	    may_do_incsearch_highlighting(firstc, گنتی, &is_state);
#اگر_ختم
	// Trigger CmdlineChanged autocommands.
	اگر (trigger_cmdlinechanged
		&& (ccline.cmdpos != پچھلا_cmdpos
		    || (پچھلا_cmdbuff != تہی &&
			STRCMP(پچھلا_cmdbuff, ccline.cmdbuff) != 0)))
	{
	    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINECHANGED);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // Show popup updates from the autocmd without a manual :redraw.
	    اگر (popup_need_redraw())
	    {
		update_screen(0);
		redrawcmd();
	    }
#اگر_ختم
	}

	// Trigger CursorMovedC autocommands.
	اگر (ccline.cmdpos != پچھلا_cmdpos)
	    trigger_cmd_autocmd(cmdline_type, EVENT_CURSORMOVEDC);

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
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
#اگر_ختم
    }

returncmd:
    // Trigger CmdlineLeavePre autocommands if not already triggered.
    اگر (!event_cmdlineleavepre_triggered)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	سیٹ_vim_var_char(c);  // Set v:char
#اگر_ختم
	trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVEPRE);
    }

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#اگر_ختم

    // We could have reached here without having a chance to clean up wild menu
    // if certain special keys like <Esc> or <C-\> were used as wildchar. Make
    // sure to still clean up to avoid memory corruption.
    اگر (cmdline_pum_فعال())
	cmdline_pum_remove(&ccline, FALSE);
    wildmenu_cleanup(&ccline);
    did_wild_list = FALSE;
    wim_اشاریہ = 0;

    ExpandCleanup(&xpc);
    ccline.xpc = تہی;
    clear_cmdline_orig();

#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
    اختتام_incsearch_highlighting(gotesc, &is_state, FALSE);
#اگر_ختم

    اگر (ccline.cmdbuff != تہی)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	اگر (ccline.cmdlen && firstc != NUL
		&& (some_کلید_typed || histype == HIST_تلاش))
	{
	    جوڑو_مقصد_history(histype, ccline.cmdbuff, ccline.cmdlen, TRUE,
				       histype == HIST_تلاش ? firstc : NUL);
	    اگر (firstc == ':')
	    {
		vim_free(new_آخری_cmdline);
		new_آخری_cmdline = vim_strnsave(ccline.cmdbuff, ccline.cmdlen);
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
    پیغام_check();
    پیغام_scroll = save_پیغام_scroll;
    redir_off = FALSE;

    // When the command line was typed, no need for a wait-return prompt.
    اگر (some_کلید_typed)
	need_wait_return = FALSE;

    // Trigger CmdlineLeave autocommands.
#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_char(c);  // Set v:char
#اگر_ختم
    trigger_cmd_autocmd(cmdline_type, EVENT_CMDLINELEAVE);

    State = save_State;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (!debug_منوال)
	may_trigger_modechanged();
#اگر_ختم

#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    اگر (b_im_ptr != تہی && buf_valid(b_im_ptr_buf)
						  && *b_im_ptr != B_IMODE_LMAP)
	im_save_حالت(b_im_ptr);
    im_سیٹ_فعال(FALSE);
#اگر_ختم
    setmouse();
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
    sb_متن_انتہا_cmdline();

theend:
    {
	char_u *p = ccline.cmdbuff;

	--گہرائی;
	اگر (did_save_ccline)
	    restore_cmdline(&save_ccline);
	ورنہ
	    ccline.cmdbuff = تہی;

	vim_free(پچھلا_cmdbuff);
	واپس p;
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
    صحیح			پیغام_ستون_save = پیغام_ستون;
    صحیح			پیغام_silent_save = پیغام_silent;

    اگر (ccline.cmdbuff != تہی)
    {
	// Save the values of the current cmdline and restore them below.
	save_cmdline(&save_ccline);
	did_save_ccline = TRUE;
    }

    CLEAR_میدان(ccline);
    ccline.cmdprompt = prompt;
    ccline.cmdattr = attr;
# ifdef FEAT_EVAL
    ccline.xp_context = xp_context;
    ccline.xp_arg = xp_arg;
    ccline.داخلہ_fn = (firstc == '@');
# endif
    پیغام_silent = 0;
    s = getcmdline_int(firstc, 1L, 0, FALSE);

    اگر (did_save_ccline)
	restore_cmdline(&save_ccline);

    پیغام_silent = پیغام_silent_save;
    // Restore msg_col, the prompt from input() may have changed it.
    // But only if called recursively and the commandline is therefore being
    // restored to an old one; if not, the input() prompt stays on the screen,
    // so we need its modified msg_col left intact.
    اگر (ccline.cmdbuff != تہی)
	پیغام_ستون = پیغام_ستون_save;

    واپس s;
}
#اگر_ختم

/*
 * Read the 'wildmode' option, fill wim_flags[].
 */
    صحیح
check_opt_wim(خالی)
{
    char_u	new_wim_flags[4];
    char_u	*p;
    صحیح		i;
    صحیح		idx = 0;

    برائے (i = 0; i < 4; ++i)
	new_wim_flags[i] = 0;

    برائے (p = p_wim; *p; ++p)
    {
	// Note: Keep this in sync with p_wim_values.
	برائے (i = 0; ASCII_ISALPHA(p[i]); ++i)
	    ;
	اگر (p[i] != NUL && p[i] != ',' && p[i] != ':')
	    واپس FAIL;
	اگر (i == 7 && STRNCMP(p, "longest", 7) == 0)
	    new_wim_flags[idx] |= WIM_LONGEST;
	ورنہ اگر (i == 4 && STRNCMP(p, "full", 4) == 0)
	    new_wim_flags[idx] |= WIM_بھرا;
	ورنہ اگر (i == 4 && STRNCMP(p, "list", 4) == 0)
	    new_wim_flags[idx] |= WIM_LIST;
	ورنہ اگر (i == 8 && STRNCMP(p, "lastused", 8) == 0)
	    new_wim_flags[idx] |= WIM_BUFLASTUSED;
	ورنہ اگر (i == 8 && STRNCMP(p, "noselect", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOSELECT;
	ورنہ اگر (i == 8 && STRNCMP(p, "noinsert", 8) == 0)
	    new_wim_flags[idx] |= WIM_NOINSERT;
	ورنہ
	    واپس FAIL;
	p += i;
	اگر (*p == NUL)
	    توڑو;
	اگر (*p == ',')
	{
	    اگر (idx == 3)
		واپس FAIL;
	    ++idx;
	}
    }

    // fill remaining entries with last flag
    جب (idx < 3)
    {
	new_wim_flags[idx + 1] = new_wim_flags[idx];
	++idx;
    }

    // only when there are no errors, wim_flags[] is changed
    برائے (i = 0; i < 4; ++i)
	wim_flags[i] = new_wim_flags[i];
    واپس OK;
}

/*
 * Return TRUE when the text must not be changed and we can't switch to
 * another window or buffer.  TRUE when editing the command line, evaluating
 * 'balloonexpr', etc.
 */
    صحیح
متن_locked(خالی)
{
    اگر (cmdwin_type != 0)
	واپس TRUE;
    واپس textlock != 0;
}

/*
 * Give an error message for a command that isn't allowed while the cmdline
 * window is open or editing the cmdline in another way.
 */
    خالی
متن_locked_پیغام(خالی)
{
    emsg(_(get_متن_locked_پیغام()));
}

    حرف *
get_متن_locked_پیغام(خالی)
{
    اگر (cmdwin_type != 0)
	واپس e_invalid_اندر_cmdline_کھڑکی;
    واپس e_not_allowed_مقصد_change_متن_or_change_کھڑکی;
}

/*
 * Check for text, window or buffer locked.
 * Give an error message and return TRUE if something is locked.
 */
    صحیح
متن_or_buf_locked(خالی)
{
    اگر (متن_locked())
    {
	متن_locked_پیغام();
	واپس TRUE;
    }
    واپس curbuf_locked();
}

/*
 * Check if "curbuf_lock" or "allbuf_lock" is set and return TRUE when it is
 * and give an error message.
 */
    صحیح
curbuf_locked(خالی)
{
    اگر (curbuf_تالا > 0)
    {
	emsg(_(e_not_allowed_مقصد_edit_another_بفر_now));
	واپس TRUE;
    }
    واپس allbuf_locked();
}

/*
 * Check if "allbuf_lock" is set and return TRUE when it is and give an error
 * message.
 */
    صحیح
allbuf_locked(خالی)
{
    اگر (allbuf_تالا > 0)
    {
	emsg(_(e_not_allowed_مقصد_change_بفر_information_now));
	واپس TRUE;
    }
    واپس FALSE;
}

    ساکن صحیح
cmdline_charsize(صحیح idx)
{
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    اگر (cmdline_ستارہ > 0)	    // showing '*', always 1 position
	واپس 1;
#اگر_ختم
    واپس ptr2cells(ccline.cmdbuff + idx);
}

/*
 * Compute the offset of the cursor on the command line for the prompt and
 * indent.
 */
    ساکن خالی
سیٹ_cmdspos(خالی)
{
    اگر (ccline.cmdfirstc != NUL)
	ccline.cmdspos = 1 + ccline.cmdindent;
    ورنہ
	ccline.cmdspos = 0 + ccline.cmdindent;
}

/*
 * Compute the screen position for the cursor on the command line.
 */
    ساکن خالی
سیٹ_cmdspos_cursor(خالی)
{
    صحیح		i, m, c;

    سیٹ_cmdspos();
    اگر (کلید_Typed)
    {
	m = cmdline_چوڑائی * صفیں;
	اگر (m < 0)	// overflow, cmdline_width or Rows at weird value
	    m = MAXCOL;
    }
    ورنہ
	m = MAXCOL;
    برائے (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
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
	    توڑو;
	}
	اگر (has_mbyte)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i) - 1;
    }
}

/*
 * Check if the character at "idx", which is "cells" wide, is a multi-byte
 * character that doesn't fit, so that a ">" must be displayed.
 */
    ساکن خالی
correct_cmdspos(صحیح idx, صحیح cells)
{
    اگر ((*mb_ptr2len)(ccline.cmdbuff + idx) > 1
		&& (*mb_ptr2cells)(ccline.cmdbuff + idx) > 1
		&& ccline.cmdspos % cmdline_چوڑائی + cells > cmdline_چوڑائی)
	ccline.cmdspos++;
}

/*
 * Get an Ex command line for the ":" command.
 */
    char_u *
getexline(
    صحیح		c,		// normally ':', NUL for ":append"
    خالی	*cookie غیر_مستعمل,
    صحیح		indent,		// indent for inside conditionals
    getline_opt_T options)
{
    // When executing a register, remove ':' that's in front of each line.
    اگر (exec_from_reg && vpeekc() == ':')
	(خالی)vgetc();
    واپس getcmdline(c, 1L, indent, options);
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
    خالی	*cookie غیر_مستعمل,
    صحیح		indent,		// indent for inside conditionals
    getline_opt_T options غیر_مستعمل)
{
    garray_T	سطر_ga;
    char_u	*pend;
    صحیح		startcol = 0;
    صحیح		c1 = 0;
    صحیح		escaped = FALSE;	// CTRL-V typed
    صحیح		vcol = 0;
    char_u	*p;
    صحیح		پچھلا_char;
    صحیح		len;

    // Switch cursor on now.  This avoids that it happens after the "\n", which
    // confuses the system function that computes tabstops.
    cursor_on();

    // always start in column 0; write a newline if necessary
    compute_cmdrow();
    اگر ((پیغام_ستون || پیغام_didout) && promptc != '?')
	پیغام_putchar('\n');
    اگر (promptc == ':')
    {
	// indent that is only displayed, not in the line itself
	اگر (p_prompt)
	    پیغام_putchar(':');
	جب (indent-- > 0)
	    پیغام_putchar(' ');
	startcol = پیغام_ستون;
    }

    ga_init2(&سطر_ga, 1, 30);

    // autoindent for :insert and :append is in the line itself
    اگر (promptc <= 0)
    {
	vcol = indent;
	جب (indent >= 8)
	{
	    ga_append(&سطر_ga, TAB);
	    پیغام_puts("        ");
	    indent -= 8;
	}
	جب (indent-- > 0)
	{
	    ga_append(&سطر_ga, ' ');
	    پیغام_putchar(' ');
	}
    }
    ++no_mapping;
    ++allow_کلیدیں;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    جب (!got_int)
    {
	طویل    sw;
	char_u *s;

	// May request the keyboard protocol state now.
	may_send_t_RK();

	اگر (ga_grow(&سطر_ga, 40) == FAIL)
	    توڑو;

	/*
	 * Get one character at a time.
	 */
	پچھلا_char = c1;

	// Check for a ":normal" command and no more characters left.
	اگر (ex_معمول_busy > 0 && typebuf.tb_len == 0)
	    c1 = '\n';
	ورنہ
	    c1 = vgetc();

	/*
	 * Handle line editing.
	 * Previously this was left to the system, putting the terminal in
	 * cooked mode, but then CTRL-D and CTRL-T can't be used properly.
	 */
	اگر (got_int)
	{
	    پیغام_putchar('\n');
	    توڑو;
	}

	اگر (c1 == K_PS)
	{
	    bracketed_paste(PASTE_EX, FALSE, &سطر_ga);
	    جاؤ redraw;
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
			p = (char_u *)سطر_ga.ga_ڈیٹا;
			p[سطر_ga.ga_len] = NUL;
			len = (*mb_سر_off)(p, p + سطر_ga.ga_len - 1) + 1;
			سطر_ga.ga_len -= len;
		    }
		    ورنہ
			--سطر_ga.ga_len;
		    جاؤ redraw;
		}
		جاری;
	    }

	    اگر (c1 == Ctrl_U)
	    {
		پیغام_ستون = startcol;
		پیغام_clr_eos();
		سطر_ga.ga_len = 0;
		جاؤ redraw;
	    }

	    اگر (c1 == Ctrl_T)
	    {
		sw = get_sw_قدر(curbuf);
		p = (char_u *)سطر_ga.ga_ڈیٹا;
		p[سطر_ga.ga_len] = NUL;
		indent = get_indent_str(p, 8, FALSE);
		indent += sw - indent % sw;
جوڑو_indent:
		جب (get_indent_str(p, 8, FALSE) < indent)
		{
		    (خالی)ga_grow(&سطر_ga, 2);  // one more for the NUL
		    p = (char_u *)سطر_ga.ga_ڈیٹا;
		    s = skipwhite(p);
		    mch_memmove(s + 1, s, سطر_ga.ga_len - (s - p) + 1);
		    *s = ' ';
		    ++سطر_ga.ga_len;
		}
redraw:
		// redraw the line
		پیغام_ستون = startcol;
		vcol = 0;
		p = (char_u *)سطر_ga.ga_ڈیٹا;
		p[سطر_ga.ga_len] = NUL;
		جب (p < (char_u *)سطر_ga.ga_ڈیٹا + سطر_ga.ga_len)
		{
		    اگر (*p == TAB)
		    {
			کرو
			    پیغام_putchar(' ');
			جب (++vcol % 8);
			++p;
		    }
		    ورنہ
		    {
			len = mb_ptr2len(p);
			پیغام_outtrans_len(p, len);
			vcol += ptr2cells(p);
			p += len;
		    }
		}
		پیغام_clr_eos();
		windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
		جاری;
	    }

	    اگر (c1 == Ctrl_D)
	    {
		// Delete one shiftwidth.
		p = (char_u *)سطر_ga.ga_ڈیٹا;
		اگر (پچھلا_char == '0' || پچھلا_char == '^')
		{
		    اگر (پچھلا_char == '^')
			ex_keep_indent = TRUE;
		    indent = 0;
		    p[--سطر_ga.ga_len] = NUL;
		}
		ورنہ
		{
		    p[سطر_ga.ga_len] = NUL;
		    indent = get_indent_str(p, 8, FALSE);
		    اگر (indent > 0)
		    {
			--indent;
			indent -= indent % get_sw_قدر(curbuf);
		    }
		}
		جب (get_indent_str(p, 8, FALSE) > indent)
		{
		    s = skipwhite(p);
		    mch_memmove(s - 1, s, سطر_ga.ga_len - (s - p) + 1);
		    --سطر_ga.ga_len;
		}
		جاؤ جوڑو_indent;
	    }

	    اگر (c1 == Ctrl_V || c1 == Ctrl_Q)
	    {
		escaped = TRUE;
		جاری;
	    }

	    // Ignore special key codes: mouse movement, K_IGNORE, etc.
	    اگر (IS_SPECIAL(c1))
		جاری;
	}

	اگر (IS_SPECIAL(c1))
	    c1 = '?';
	اگر (has_mbyte)
	    len = (*mb_char2bytes)(c1,
				  (char_u *)سطر_ga.ga_ڈیٹا + سطر_ga.ga_len);
	ورنہ
	{
	    len = 1;
	    ((char_u *)سطر_ga.ga_ڈیٹا)[سطر_ga.ga_len] = c1;
	}
	اگر (c1 == '\n')
	    پیغام_putchar('\n');
	ورنہ اگر (c1 == TAB)
	{
	    // Don't use chartabsize(), 'ts' can be different
	    کرو
		پیغام_putchar(' ');
	    جب (++vcol % 8);
	}
	ورنہ
	{
	    پیغام_outtrans_len(
		     ((char_u *)سطر_ga.ga_ڈیٹا) + سطر_ga.ga_len, len);
	    vcol += char2cells(c1);
	}
	سطر_ga.ga_len += len;
	escaped = FALSE;

	windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
	pend = (char_u *)(سطر_ga.ga_ڈیٹا) + سطر_ga.ga_len;

	// We are done when a NL is entered, but not when it comes after a
	// backslash in prompt mode.
	اگر (سطر_ga.ga_len > 0 && pend[-1] == '\n'
		&& (سطر_ga.ga_len <= 1 || pend[-2] != '\\' || !promptc))
	{
	    --سطر_ga.ga_len;
	    --pend;
	    *pend = NUL;
	    توڑو;
	}
    }

    --no_mapping;
    --allow_کلیدیں;

    // make following messages go to the next line
    پیغام_didout = FALSE;
    پیغام_ستون = 0;
    اگر (پیغام_صف < صفیں - 1)
	++پیغام_صف;
    emsg_on_display = FALSE;		// don't want ui_delay()

    اگر (got_int)
	ga_clear(&سطر_ga);

    واپس (char_u *)سطر_ga.ga_ڈیٹا;
}

/*
 * Return TRUE if ccline.overstrike is on.
 */
    صحیح
cmdline_overstrike(خالی)
{
    واپس ccline.overstrike;
}

#اگر defined(MCH_CURSOR_SHAPE) || defined(FEAT_GUI) || defined(FEAT_MOUSESHAPE)
/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    صحیح
cmdline_at_انتہا(خالی)
{
    واپس (ccline.cmdpos >= ccline.cmdlen);
}
#اگر_ختم

#اگر defined(FEAT_XIM) && (defined(FEAT_GUI_GTK))
/*
 * Return the virtual column number at the current cursor position.
 * This is used by the IM code to obtain the start of the preedit string.
 */
    colnr_T
cmdline_getvcol_cursor(خالی)
{
    اگر (ccline.cmdbuff == تہی || ccline.cmdpos > ccline.cmdlen)
	واپس MAXCOL;

    اگر (has_mbyte)
    {
	colnr_T	ستون;
	صحیح	i = 0;

	برائے (ستون = 0; i < ccline.cmdpos; ++ستون)
	    i += (*mb_ptr2len)(ccline.cmdbuff + i);

	واپس ستون;
    }
    ورنہ
	واپس ccline.cmdpos;
}
#اگر_ختم

#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
/*
 * If part of the command line is an IM preedit string, redraw it with
 * IM feedback attributes.  The cursor position is restored after drawing.
 */
    ساکن خالی
redrawcmd_preedit(خالی)
{
    اگر ((State & منوال_CMDLINE)
	    && xic != تہی
	    // && im_get_status()  doesn't work when using SCIM
	    && !p_imdisable
	    && im_is_preediting())
    {
	صحیح	cmdpos = 0;
	صحیح	cmdspos;
	صحیح	old_صف;
	صحیح	old_ستون;
	colnr_T	ستون;

	old_صف = پیغام_صف;
	old_ستون = پیغام_ستون;
	cmdspos = ((ccline.cmdfirstc != NUL) ? 1 : 0) + ccline.cmdindent;

	اگر (has_mbyte)
	{
	    برائے (ستون = 0; ستون < preedit_آغاز_ستون
			  && cmdpos < ccline.cmdlen; ++ستون)
	    {
		cmdspos += (*mb_ptr2cells)(ccline.cmdbuff + cmdpos);
		cmdpos  += (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    }
	}
	ورنہ
	{
	    cmdspos += preedit_آغاز_ستون;
	    cmdpos  += preedit_آغاز_ستون;
	}

	پیغام_صف = cmdline_صف + (cmdspos / cmdline_چوڑائی);
	پیغام_ستون = cmdspos % cmdline_چوڑائی;
	اگر (پیغام_صف >= صفیں)
	    پیغام_صف = صفیں - 1;

	برائے (ستون = 0; cmdpos < ccline.cmdlen; ++ستون)
	{
	    صحیح char_len;
	    صحیح char_attr;

	    char_attr = im_get_feedback_attr(ستون);
	    اگر (char_attr < 0)
		توڑو; // end of preedit string

	    اگر (has_mbyte)
		char_len = (*mb_ptr2len)(ccline.cmdbuff + cmdpos);
	    ورنہ
		char_len = 1;

	    پیغام_outtrans_len_attr(ccline.cmdbuff + cmdpos, char_len, char_attr);
	    cmdpos += char_len;
	}

	پیغام_صف = old_صف;
	پیغام_ستون = old_ستون;
    }
}
#اگر_ختم // FEAT_XIM && FEAT_GUI_GTK

/*
 * Deallocate a command line buffer, updating the buffer size and length.
 */
    ساکن خالی
dealloc_cmdbuff(خالی)
{
    VIM_CLEAR(ccline.cmdbuff);
    ccline.cmdlen = ccline.cmdbufflen = 0;
}

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 */
    ساکن خالی
alloc_cmdbuff(size_t len)
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    اگر (len < 80)
	len = 100;
    ورنہ
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
	واپس OK;			// no need to resize

    // Keep a copy of the original cmdbuff and it's size so
    // they can be restored/used later.
    p = ccline.cmdbuff;
    plen = ccline.cmdbufflen;

    alloc_cmdbuff(len);			// will get some more
    اگر (ccline.cmdbuff == تہی)		// out of memory
    {
	ccline.cmdbuff = p;		// keep the old one
	ccline.cmdbufflen = plen;
	واپس FAIL;
    }
    // There isn't always a NUL after the command, but it may need to be
    // there, thus copy up to the NUL and add a NUL.
    mch_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    ccline.cmdbuff[ccline.cmdlen] = NUL;

    اگر (ccline.xpc != تہی
	    && ccline.xpc->xp_نمونہ != تہی
	    && ccline.xpc->xp_context != EXPAND_NOTHING
	    && ccline.xpc->xp_context != EXPAND_UNSUCCESSFUL)
    {
	صحیح i = (صحیح)(ccline.xpc->xp_نمونہ - p);

	// If xp_pattern points inside the old cmdbuff it needs to be adjusted
	// to point into the newly allocated memory.
	اگر (i >= 0 && i <= ccline.cmdlen)
	    ccline.xpc->xp_نمونہ = ccline.cmdbuff + i;
    }

    vim_free(p);

    واپس OK;
}

#اگر defined(FEAT_ARABIC)
ساکن char_u	*arshape_buf = تہی;

# اگر defined(EXITFREE)
    خالی
free_arshape_buf(خالی)
{
    vim_free(arshape_buf);
}
# endif
#اگر_ختم

/*
 * Draw part of the cmdline at the current cursor position.  But draw stars
 * when cmdline_star is TRUE.
 */
    ساکن خالی
draw_cmdline(صحیح آغاز, صحیح len)
{
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
    صحیح		i;

    اگر (cmdline_ستارہ > 0)
	برائے (i = 0; i < len; ++i)
	{
	    پیغام_putchar('*');
	    اگر (has_mbyte)
		i += (*mb_ptr2len)(ccline.cmdbuff + آغاز + i) - 1;
	}
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ARABIC
	اگر (p_arshape && !p_tbidi && cmdline_has_arabic(آغاز, len))
    {
	ساکن صحیح	buflen = 0;
	char_u		*p;
	صحیح		j;
	صحیح		newlen = 0;
	صحیح		mb_l;
	صحیح		pc, pc1 = 0;
	صحیح		پچھلا_c = 0;
	صحیح		پچھلا_c1 = 0;
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
	    اگر (arshape_buf == تہی)
		واپس;	// out of memory
	}

	اگر (utf_iscomposing(utf_ptr2char(ccline.cmdbuff + آغاز)))
	{
	    // Prepend a space to draw the leading composing char on.
	    arshape_buf[0] = ' ';
	    newlen = 1;
	}

	برائے (j = آغاز; j < آغاز + len; j += mb_l)
	{
	    p = ccline.cmdbuff + j;
	    u8c = utfc_ptr2char_len(p, u8cc, آغاز + len - j);
	    mb_l = utfc_ptr2len_len(p, آغاز + len - j);
	    اگر (ARABIC_CHAR(u8c))
	    {
		// Do Arabic shaping.
		اگر (cmdmsg_rl)
		{
		    // displaying from right to left
		    pc = پچھلا_c;
		    pc1 = پچھلا_c1;
		    پچھلا_c1 = u8cc[0];
		    اگر (j + mb_l >= آغاز + len)
			nc = NUL;
		    ورنہ
			nc = utf_ptr2char(p + mb_l);
		}
		ورنہ
		{
		    // displaying from left to right
		    اگر (j + mb_l >= آغاز + len)
			pc = NUL;
		    ورنہ
		    {
			صحیح	pcc[MAX_MCO];

			pc = utfc_ptr2char_len(p + mb_l, pcc,
						      آغاز + len - j - mb_l);
			pc1 = pcc[0];
		    }
		    nc = پچھلا_c;
		}
		پچھلا_c = u8c;

		u8c = arabic_shape(u8c, تہی, &u8cc[0], pc, pc1, nc);

		newlen += (*mb_char2bytes)(u8c, arshape_buf + newlen);
		اگر (u8cc[0] != 0)
		{
		    newlen += (*mb_char2bytes)(u8cc[0], arshape_buf + newlen);
		    اگر (u8cc[1] != 0)
			newlen += (*mb_char2bytes)(u8cc[1],
							arshape_buf + newlen);
		}
	    }
	    ورنہ
	    {
		پچھلا_c = u8c;
		mch_memmove(arshape_buf + newlen, p, mb_l);
		newlen += mb_l;
	    }
	}

	پیغام_outtrans_len(arshape_buf, newlen);
    }
    ورنہ
#اگر_ختم
	پیغام_outtrans_len(ccline.cmdbuff + آغاز, len);
}

/*
 * Put a character on the command line.  Shifts the following text to the
 * right when "shift" is TRUE.  Used for CTRL-V, CTRL-K, etc.
 * "c" must be printable (fit in one display cell)!
 */
    خالی
putcmdline(صحیح c, صحیح سرکاؤ)
{
    اگر (cmd_silent)
	واپس;
    پیغام_no_مزید = TRUE;
    پیغام_putchar(c);
    اگر (سرکاؤ)
	draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
    پیغام_no_مزید = FALSE;
    cursorcmd();
    extra_char = c;
    extra_char_سرکاؤ = سرکاؤ;
}

/*
 * Undo a putcmdline(c, FALSE).
 */
    خالی
unputcmdline(خالی)
{
    اگر (cmd_silent)
	واپس;
    پیغام_no_مزید = TRUE;
    اگر (ccline.cmdlen == ccline.cmdpos)
	پیغام_putchar(' ');
    ورنہ اگر (has_mbyte)
	draw_cmdline(ccline.cmdpos,
			       (*mb_ptr2len)(ccline.cmdbuff + ccline.cmdpos));
    ورنہ
	draw_cmdline(ccline.cmdpos, 1);
    پیغام_no_مزید = FALSE;
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
    ورنہ
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
	ورنہ
	{
	    اگر (has_mbyte)
	    {
		// Count nr of characters in the new string.
		m = 0;
		برائے (i = 0; i < len; i += (*mb_ptr2len)(str + i))
		    ++m;
		// Count nr of bytes in cmdline that are overwritten by these
		// characters.
		برائے (i = ccline.cmdpos; i < ccline.cmdlen && m > 0;
				 i += (*mb_ptr2len)(ccline.cmdbuff + i))
		    --m;
		اگر (i < ccline.cmdlen)
		{
		    mch_memmove(ccline.cmdbuff + ccline.cmdpos + len,
			    ccline.cmdbuff + i, (size_t)(ccline.cmdlen - i));
		    ccline.cmdlen += ccline.cmdpos + len - i;
		}
		ورنہ
		    ccline.cmdlen = ccline.cmdpos + len;
	    }
	    ورنہ اگر (ccline.cmdpos + len > ccline.cmdlen)
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
	    جب (ccline.cmdpos > 0 && utf_iscomposing(c))
	    {
		i = (*mb_سر_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		ccline.cmdpos -= i;
		len += i;
		c = utf_ptr2char(ccline.cmdbuff + ccline.cmdpos);
	    }
#اگر_تعریف_شدہ FEAT_ARABIC
	    اگر (i == 0 && ccline.cmdpos > 0 && arabic_maycombine(c))
	    {
		// Check the previous character for Arabic combining pair.
		i = (*mb_سر_off)(ccline.cmdbuff,
				      ccline.cmdbuff + ccline.cmdpos - 1) + 1;
		اگر (arabic_combine(utf_ptr2char(ccline.cmdbuff
						     + ccline.cmdpos - i), c))
		{
		    ccline.cmdpos -= i;
		    len += i;
		}
		ورنہ
		    i = 0;
	    }
#اگر_ختم
	    اگر (i != 0)
	    {
		// Also backup the cursor position.
		i = ptr2cells(ccline.cmdbuff + ccline.cmdpos);
		ccline.cmdspos -= i;
		پیغام_ستون -= i;
		اگر (پیغام_ستون < 0)
		{
		    پیغام_ستون += cmdline_چوڑائی;
		    --پیغام_صف;
		}
	    }
	}

	اگر (redraw && !cmd_silent)
	{
	    پیغام_no_مزید = TRUE;
	    i = cmdline_صف;
	    cursorcmd();
	    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
	    // Avoid clearing the rest of the line too often.
	    اگر (cmdline_صف != i || ccline.overstrike)
		پیغام_clr_eos();
	    پیغام_no_مزید = FALSE;
	}
	اگر (کلید_Typed)
	{
	    m = cmdline_چوڑائی * صفیں;
	    اگر (m < 0)	// overflow, cmdline_width or Rows at weird value
		m = MAXCOL;
	}
	ورنہ
	    m = MAXCOL;
	برائے (i = 0; i < len; ++i)
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
	پیغام_check();
    واپس retval;
}

ساکن cmdline_info_T	پچھلا_ccline;
ساکن صحیح		پچھلا_ccline_مستعمل = FALSE;

/*
 * Save ccline, because obtaining the "=" register may execute "normal :cmd"
 * and overwrite it.  But get_cmdline_str() may need it, thus make it
 * available globally in prev_ccline.
 */
    ساکن خالی
save_cmdline(cmdline_info_T *ccp)
{
    اگر (!پچھلا_ccline_مستعمل)
    {
	CLEAR_میدان(پچھلا_ccline);
	پچھلا_ccline_مستعمل = TRUE;
    }
    *ccp = پچھلا_ccline;
    پچھلا_ccline = ccline;
    ccline.cmdbuff = تہی;  // signal that ccline is not in use
}

/*
 * Restore ccline after it has been saved with save_cmdline().
 */
    ساکن خالی
restore_cmdline(cmdline_info_T *ccp)
{
    ccline = پچھلا_ccline;
    پچھلا_ccline = *ccp;
}

/*
 * Paste a yank register into the command line.
 * Used by CTRL-R command in command-line mode.
 * insert_reg() can't be used here, because special characters from the
 * register contents will be interpreted as commands.
 *
 * Return FAIL for failure, OK otherwise.
 */
    ساکن صحیح
cmdline_paste(
    صحیح regname,
    صحیح literally,	// Insert text literally instead of "as typed"
    صحیح remcr)		// remove trailing CR
{
    طویل		i;
    char_u		*arg;
    char_u		*p;
    صحیح			allocated;

    // check for valid regname; also accept special characters for CTRL-R in
    // the command line
    اگر (regname != Ctrl_F && regname != Ctrl_P && regname != Ctrl_W
	    && regname != Ctrl_A && regname != Ctrl_L
	    && !valid_yank_reg(regname, FALSE))
	واپس FAIL;

    // A register containing CTRL-R can cause an endless loop.  Allow using
    // CTRL-C to break the loop.
    سطر_breakcheck();
    اگر (got_int)
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_CLIPBOARD
    regname = may_get_selection(regname);
#اگر_ختم

    // Need to set "textlock" to avoid nasty things like going to another
    // buffer when evaluating an expression.
    ++textlock;
    i = get_spec_reg(regname, &arg, &allocated, TRUE);
    --textlock;

    اگر (i)
    {
	// Got the value of a special register in "arg".
	اگر (arg == تہی)
	    واپس FAIL;

	// When 'incsearch' is set and CTRL-R CTRL-W used: skip the duplicate
	// part of the word.
	p = arg;
	اگر (p_is && regname == Ctrl_W)
	{
	    char_u  *w;
	    صحیح	    len;

	    // Locate start of last word in the cmd buffer.
	    برائے (w = ccline.cmdbuff + ccline.cmdpos; w > ccline.cmdbuff; )
	    {
		اگر (has_mbyte)
		{
		    len = (*mb_سر_off)(ccline.cmdbuff, w - 1) + 1;
		    اگر (!vim_iswordc(mb_ptr2char(w - len)))
			توڑو;
		    w -= len;
		}
		ورنہ
		{
		    اگر (!vim_iswordc(w[-1]))
			توڑو;
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
	واپس OK;
    }

    واپس cmdline_paste_reg(regname, literally, remcr);
}

/*
 * Put a string on the command line.
 * When "literally" is TRUE, insert literally.
 * When "literally" is FALSE, insert as typed, but don't leave the command
 * line.
 */
    خالی
cmdline_paste_str(char_u *s, صحیح literally)
{
    صحیح		c, cv;

    اگر (literally)
	put_on_cmdline(s, -1, TRUE);
    ورنہ
	جب (*s != NUL)
	{
	    cv = *s;
	    اگر (cv == Ctrl_V && s[1])
		++s;
	    اگر (has_mbyte)
		c = mb_cptr2char_adv(&s);
	    ورنہ
		c = *s++;
	    اگر (cv == Ctrl_V || c == ESC || c == Ctrl_C
		    || c == CAR || c == NL || c == Ctrl_L
#اگر_تعریف_شدہ UNIX
		    || c == intr_char
#اگر_ختم
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
    خالی
redrawcmdline(خالی)
{
    redrawcmdline_ex(TRUE);
}

/*
 * When "do_compute_cmdrow" is TRUE the command line is redrawn at the bottom.
 * If FALSE cmdline_row is used, which should redraw in the same place.
 */
    خالی
redrawcmdline_ex(صحیح do_compute_cmdrow)
{
    اگر (cmd_silent)
	واپس;
    need_wait_return = FALSE;
    اگر (do_compute_cmdrow)
	compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

    ساکن خالی
redrawcmdprompt(خالی)
{
    صحیح		i;

    اگر (cmd_silent)
	واپس;
    اگر (ccline.cmdfirstc != NUL)
	پیغام_putchar(ccline.cmdfirstc);
    اگر (ccline.cmdprompt != تہی)
    {
	پیغام_puts_attr((حرف *)ccline.cmdprompt, ccline.cmdattr);
	ccline.cmdindent = پیغام_ستون + (پیغام_صف - cmdline_صف) * cmdline_چوڑائی;
	// do the reverse of set_cmdspos()
	اگر (ccline.cmdfirstc != NUL)
	    --ccline.cmdindent;
    }
    ورنہ
	برائے (i = ccline.cmdindent; i > 0; --i)
	    پیغام_putchar(' ');
}

/*
 * Redraw what is currently on the command line.
 */
    خالی
redrawcmd(خالی)
{
    صحیح save_اندر_echowindow = اندر_echowindow;

    اگر (cmd_silent)
	واپس;

    // when 'incsearch' is set there may be no command line while redrawing
    اگر (ccline.cmdbuff == تہی)
    {
	windgoto(cmdline_صف, cmdline_ستون_off);
	پیغام_clr_eos();
	واپس;
    }

    // Do not put this in the message window.
    اندر_echowindow = FALSE;

    sb_متن_restart_cmdline();
    پیغام_آغاز();
    // Reset lines_left so a wrapped cmdline isn't truncated by msg_no_more.
    پیغام_starthere();
    redrawcmdprompt();

    // Don't use more prompt, truncate the cmdline if it doesn't fit.
    پیغام_no_مزید = TRUE;
    draw_cmdline(0, ccline.cmdlen);
    پیغام_clr_eos();
    پیغام_no_مزید = FALSE;

    سیٹ_cmdspos_cursor();
    اگر (extra_char != NUL)
	putcmdline(extra_char, extra_char_سرکاؤ);

    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    پیغام_scroll = FALSE;		// next message overwrites cmdline

    // Typing ':' at the more prompt may set skip_redraw.  We don't want this
    // in cmdline mode
    skip_redraw = FALSE;

    اندر_echowindow = save_اندر_echowindow;
}

    خالی
compute_cmdrow(خالی)
{
    // ignore "msg_scrolled" in update_screen(), it will be reset soon.
    اگر (exmode_فعال || (پیغام_scrolled != 0 && !updating_screen))
	cmdline_صف = صفیں - 1;
    ورنہ
	cmdline_صف = W_WINROW(lastwin) + lastwin->w_اونچائی
						    + lastwin->w_حالت_اونچائی;
}

    خالی
cursorcmd(خالی)
{
    اگر (cmd_silent)
	واپس;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
    {
	پیغام_صف = cmdline_صف  + (ccline.cmdspos / (cmdline_چوڑائی - 1));
	پیغام_ستون = cmdline_چوڑائی - (ccline.cmdspos % (cmdline_چوڑائی - 1)) - 1;
	اگر (پیغام_صف <= 0)
	    پیغام_صف = صفیں - 1;
    }
    ورنہ
#اگر_ختم
    {
	پیغام_صف = cmdline_صف + (ccline.cmdspos / cmdline_چوڑائی);
	پیغام_ستون = ccline.cmdspos % cmdline_چوڑائی;
	اگر (پیغام_صف >= صفیں)
	    پیغام_صف = صفیں - 1;
    }

    windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    اگر (p_imst == IM_ON_THE_SPOT)
	redrawcmd_preedit();
#اگر_ختم
#اگر_تعریف_شدہ MCH_CURSOR_SHAPE
    mch_update_cursor();
#اگر_ختم
}

    خالی
gotocmdline(صحیح clr)
{
    پیغام_آغاز();
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (cmdmsg_rl)
	پیغام_ستون = cmdline_چوڑائی - 1;
    ورنہ
#اگر_ختم
	پیغام_ستون = 0;	    // always start in column 0
    اگر (clr)		    // clear the bottom line(s)
	پیغام_clr_eos();	    // will reset clear_cmdline
    windgoto(cmdline_صف, cmdline_ستون_off);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
    ساکن صحیح
ccheck_abbr(صحیح c)
{
    صحیح spos = 0;

    اگر (p_paste || no_abbr)	    // no abbreviations or in paste mode
	واپس FALSE;

    // Do not consider '<,'> be part of the mapping, skip leading whitespace.
    // Actually accepts any mark.
    جب (VIM_ISWHITE(ccline.cmdbuff[spos]) && spos < ccline.cmdlen)
	spos++;
    اگر (ccline.cmdlen - spos > 5
	    && ccline.cmdbuff[spos] == '\''
	    && ccline.cmdbuff[spos + 2] == ','
	    && ccline.cmdbuff[spos + 3] == '\'')
	spos += 5;
    ورنہ
	// check abbreviation from the beginning of the commandline
	spos = 0;

    واپس check_abbr(c, ccline.cmdbuff, ccline.cmdpos, spos);
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
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    char_u	buf[20];
    صحیح		j = 0;

    // Don't escape '[', '{' and '!' if they are in 'isfname' and for the
    // ":buffer" command.
    برائے (p = what == VSE_بفر ? بفر_ESC_CHARS : راستہ_ESC_CHARS;
								*p != NUL; ++p)
	اگر ((*p != '[' && *p != '{' && *p != '!') || !vim_isfilec(*p))
	    buf[j++] = *p;
    buf[j] = NUL;
    p = vim_strsave_escaped(fname, buf);
#ورنہ
    p = vim_strsave_escaped(fname, what == VSE_SHELL ? SHELL_ESC_CHARS
		    : what == VSE_بفر ? بفر_ESC_CHARS : راستہ_ESC_CHARS);
    اگر (what == VSE_SHELL && csh_like_shell() && p != تہی)
    {
	char_u	    *s;

	// For csh and similar shells need to put two backslashes before '!'.
	// One is taken by Vim, one by the shell.
	s = vim_strsave_escaped(p, (char_u *)"!");
	vim_free(p);
	p = s;
    }
#اگر_ختم

    // '>' and '+' are special at the start of some commands, e.g. ":edit" and
    // ":write".  "cd -" has a special meaning.
    اگر (p != تہی && (*p == '>' || *p == '+' || (*p == '-' && p[1] == NUL)))
	escape_fname(&p);

    واپس p;
}

/*
 * Put a backslash before the file name in "pp", which is in allocated memory.
 */
    خالی
escape_fname(char_u **pp)
{
    char_u	*p;

    p = alloc(STRLEN(*pp) + 2);
    اگر (p == تہی)
	واپس;

    p[0] = '\\';
    STRCPY(p + 1, *pp);
    vim_free(*pp);
    *pp = p;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    خالی
tilde_replace(
    char_u  *orig_pat,
    صحیح	    عدد_files,
    char_u  **files)
{
    صحیح	    i;
    char_u  *p;

    اگر (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	برائے (i = 0; i < عدد_files; ++i)
	{
	    p = home_replace_save(تہی, files[i]);
	    اگر (p != تہی)
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
get_cmdline_info(خالی)
{
    واپس &ccline;
}

/*
 * Get pointer to the command line info to use. save_cmdline() may clear
 * ccline and put the previous value in prev_ccline.
 */
    ساکن cmdline_info_T *
get_ccline_ptr(خالی)
{
    اگر ((State & منوال_CMDLINE) == 0)
	واپس تہی;
    اگر (ccline.cmdbuff != تہی)
	واپس &ccline;
    اگر (پچھلا_ccline_مستعمل && پچھلا_ccline.cmdbuff != تہی)
	واپس &پچھلا_ccline;
    واپس تہی;
}

/*
 * Get the current command-line type.
 * Returns ':' or '/' or '?' or '@' or '>' or '-'
 * Only works when the command line is being edited.
 * Returns NUL when something is wrong.
 */
    ساکن صحیح
get_cmdline_type(خالی)
{
    cmdline_info_T *p = get_ccline_ptr();

    اگر (p == تہی)
	واپس NUL;
    اگر (p->cmdfirstc == NUL)
	واپس
#اگر_تعریف_شدہ FEAT_EVAL
	    (p->داخلہ_fn) ? '@' :
#اگر_ختم
	    '-';
    واپس p->cmdfirstc;
}

#اگر defined(FEAT_EVAL)
/*
 * Get the current command line in allocated memory.
 * Only works when the command line is being edited.
 * Returns NULL when something is wrong.
 */
    ساکن char_u *
get_cmdline_str(خالی)
{
    cmdline_info_T *p;

    اگر (cmdline_ستارہ > 0)
	واپس تہی;
    p = get_ccline_ptr();
    اگر (p == تہی)
	واپس تہی;
    واپس vim_strnsave(p->cmdbuff, p->cmdlen);
}

/*
 * Get the current command-line completion pattern.
 */
    ساکن char_u *
get_cmdline_completion_نمونہ(خالی)
{
    cmdline_info_T *p;
    صحیح		xp_context;

    اگر (cmdline_ستارہ > 0)
	واپس تہی;

    p = get_ccline_ptr();
    اگر (p == تہی || p->xpc == تہی)
	واپس تہی;

    xp_context = p->xpc->xp_context;
    اگر (xp_context == EXPAND_NOTHING)
    {
	سیٹ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    اگر (xp_context == EXPAND_UNSUCCESSFUL)
	واپس تہی;

    char_u *compl_pat = p->xpc->xp_نمونہ;

    اگر (compl_pat == تہی)
	واپس تہی;

    واپس vim_strsave(compl_pat);
}

/*
 * Get the command-line completion type.
 */
    ساکن char_u *
get_cmdline_completion(خالی)
{
    cmdline_info_T	*p;
    صحیح			xp_context;

    اگر (cmdline_ستارہ > 0)
	واپس تہی;

    p = get_ccline_ptr();
    اگر (p == تہی || p->xpc == تہی)
	واپس تہی;

    xp_context = p->xpc->xp_context;
    اگر (xp_context == EXPAND_NOTHING)
    {
	سیٹ_expand_context(p->xpc);
	xp_context = p->xpc->xp_context;
	p->xpc->xp_context = EXPAND_NOTHING;
    }
    اگر (xp_context == EXPAND_UNSUCCESSFUL)
	واپس تہی;

    واپس cmdcomplete_type_مقصد_str(xp_context, p->xpc->xp_arg);
}

/*
 * "getcmdcomplpat()" function
 */
    خالی
f_getcmdcomplpat(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion_نمونہ();
}

/*
 * "getcmdcompltype()" function
 */
    خالی
f_getcmdcompltype(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_completion();
}

/*
 * "getcmdline()" function
 */
    خالی
f_getcmdline(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = get_cmdline_str();
}

/*
 * "getcmdpos()" function
 */
    خالی
f_getcmdpos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_نمبر = p != تہی ? p->cmdpos + 1 : 0;
}

/*
 * "getcmdprompt()" function
 */
    خالی
f_getcmdprompt(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = p != تہی && p->cmdprompt != تہی ?
					    vim_strsave(p->cmdprompt) : تہی;
}

/*
 * "getcmdscreenpos()" function
 */
    خالی
f_getcmdscreenpos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    cmdline_info_T *p = get_ccline_ptr();

    rettv->vval.v_نمبر = p != تہی ? cmdline_ستون_off + p->cmdspos + 1 : 0;
}

/*
 * "getcmdtype()" function
 */
    خالی
f_getcmdtype(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = alloc(2);
    اگر (rettv->vval.v_string == تہی)
	واپس;

    rettv->vval.v_string[0] = get_cmdline_type();
    rettv->vval.v_string[1] = NUL;
}

// Set the command line str to "str".
// Returns 1 when failed, 0 when OK.
    ساکن صحیح
سیٹ_cmdline_str(char_u *str, صحیح pos)
{
    cmdline_info_T  *p = get_ccline_ptr();
    صحیح		    len;

    اگر (p == تہی)
	واپس 1;

    len = (صحیح)STRLEN(str);
    اگر (realloc_cmdbuff(len + 1) != OK)
	واپس 1;
    p->cmdlen = len;
    STRCPY(p->cmdbuff, str);

    p->cmdpos = pos < 0 || pos > p->cmdlen ? p->cmdlen : pos;
    new_cmdpos = p->cmdpos;
    p->cmdbuff_replaced = TRUE;

    redrawcmd();

    // Trigger CmdlineChanged autocommands.
    trigger_cmd_autocmd(get_cmdline_type(), EVENT_CMDLINECHANGED);

    واپس 0;
}

/*
 * Set the command line byte position to "pos".  Zero is the first position.
 * Only works when the command line is being edited.
 * Returns 1 when failed, 0 when OK.
 */
    ساکن صحیح
سیٹ_cmdline_pos(
    صحیح		pos)
{
    cmdline_info_T *p = get_ccline_ptr();

    اگر (p == تہی)
	واپس 1;

    // The position is not set directly but after CTRL-\ e or CTRL-R = has
    // changed the command line.
    اگر (pos < 0)
	new_cmdpos = 0;
    ورنہ
	new_cmdpos = pos;

    واپس 0;
}

// "setcmdline()" function
    خالی
f_setcmdline(typval_T *argvars, typval_T *rettv)
{
    صحیح pos = -1;

    اگر (check_for_string_arg(argvars, 0) == FAIL
	    || check_for_opt_نمبر_arg(argvars, 1) == FAIL)
	واپس;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	صحیح error = FALSE;

	pos = (صحیح)tv_get_نمبر_chk(&argvars[1], &error) - 1;
	اگر (error)
	    واپس;
	اگر (pos < 0)
	{
	    emsg(_(e_دلیل_must_be_مثبت));
	    واپس;
	}
    }

    // Use tv_get_string() to handle a NULL string like an empty string.
    rettv->vval.v_نمبر = سیٹ_cmdline_str(tv_get_string(&argvars[0]), pos);
}

/*
 * "setcmdpos()" function
 */
    خالی
f_setcmdpos(typval_T *argvars, typval_T *rettv)
{
    صحیح		pos;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    pos = (صحیح)tv_get_نمبر(&argvars[0]) - 1;
    اگر (pos >= 0)
	rettv->vval.v_نمبر = سیٹ_cmdline_pos(pos);
}
#اگر_ختم

/*
 * Return the first character of the current command line.
 */
    صحیح
get_cmdline_firstc(خالی)
{
    واپس ccline.cmdfirstc;
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
    صحیح		پہلا = FALSE;
    varnumber_T	عدد;

    *str = skipwhite(*str);
    اگر (**str == '-' || vim_isdigit(**str))  // parse "from" part of range
    {
	vim_str2nr(*str, تہی, &len, 0, &عدد, تہی, 0, FALSE, تہی);
	*str += len;
	//حد_سے_تجاوز
	اگر (عدد > زیادہ_سے_زیادہ_عدد)
	    واپس FAIL;

	*num1 = (صحیح)عدد;
	پہلا = TRUE;
    }
    *str = skipwhite(*str);
    اگر (**str == ',')			// parse "to" part of range
    {
	*str = skipwhite(*str + 1);
	vim_str2nr(*str, تہی, &len, 0, &عدد, تہی, 0, FALSE, تہی);
	اگر (len > 0)
	{
	    *str = skipwhite(*str + len);
	    //حد_سے_تجاوز
	    اگر (عدد > زیادہ_سے_زیادہ_عدد)
		واپس FAIL;

	    *num2 = (صحیح)عدد;
	}
	ورنہ اگر (!پہلا)		// no number given at all
	    واپس FAIL;
    }
    ورنہ اگر (پہلا)			// only one number given
	*num2 = *num1;
    واپس OK;
}

/*
 * Check value of 'cedit' and set cedit_key.
 * Returns NULL if value is OK, error message otherwise.
 */
    حرف *
did_سیٹ_cedit(optset_T *دلائل غیر_مستعمل)
{
    صحیح n;

    اگر (*p_cedit == NUL)
	cedit_کلید = -1;
    ورنہ
    {
	n = string_مقصد_کلید(p_cedit, FALSE);
	اگر (n == 0 || vim_isprintc(n))
	    واپس e_invalid_دلیل;
	cedit_کلید = n;
    }
    واپس تہی;
}

/*
 * Open a window on the current command line and history.  Allow editing in
 * the window.  Returns when the window is closed.
 * Returns:
 *	CR	 if the command is to be executed
 *	Ctrl_C	 if it is to be abandoned
 *	K_IGNORE if editing continues
 */
    ساکن صحیح
open_cmdwin(خالی)
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
    صحیح			save_exmode = exmode_فعال;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    صحیح			save_cmdmsg_rl = cmdmsg_rl;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
    صحیح			save_کلید_Typed;
#اگر_ختم
    صحیح			newbuf_حالت;
    صحیح			cmdwin_valid;

    // Can't do this when text or buffer is locked.
    // Can't do this recursively.  Can't do it when typing a password.
    اگر (متن_or_buf_locked()
	    || cmdwin_type != 0
#اگر defined(FEAT_CRYPT) || defined(FEAT_EVAL)
	    || cmdline_ستارہ > 0
#اگر_ختم
	    )
    {
	beep_flush();
	واپس K_IGNORE;
    }
    سیٹ_bufref(&old_curbuf, curbuf);

    // Save current window sizes.
    win_سائز_save(&winsizes);

    // When using completion in Insert mode with <C-R>=<C-F> one can open the
    // command line window, but we don't want the popup menu then.
    pum_undisplay();

    // don't use a new tab page
    cmdmod.cmod_tab = 0;
    cmdmod.cmod_flags |= CMOD_NOSWAPFILE;

    // Create a window for the command-line buffer.
    اگر (win_بانٹو((صحیح)p_cwh, WSP_BOT) == FAIL)
    {
	beep_flush();
	ga_clear(&winsizes);
	واپس K_IGNORE;
    }
    // win_split() autocommands may have messed with the old window or buffer.
    // Treat it as abandoning this command-line.
    اگر (!win_valid(old_curwin) || curwin == old_curwin
	    || !bufref_valid(&old_curbuf)
	    || old_curwin->w_بفر != old_curbuf.br_buf)
    {
	beep_flush();
	ga_clear(&winsizes);
	واپس Ctrl_C;
    }
    // Don't let quitting the More prompt make this fail.
    got_int = FALSE;

    // Set "cmdwin_..." variables before any autocommands may mess things up.
    cmdwin_type = get_cmdline_type();
    cmdwin_win = curwin;

    // Create empty command-line buffer.  Be especially cautious of BufLeave
    // autocommands from do_ecmd(), as cmdwin restrictions do not apply to them!
    newbuf_حالت = do_ecmd(0, تہی, تہی, تہی, ECMD_ONE, ECMD_HIDE,
					تہی);
    cmdwin_valid = win_valid(cmdwin_win);
    اگر (newbuf_حالت == FAIL || !cmdwin_valid || curwin != cmdwin_win ||
	    !win_valid(old_curwin) || !bufref_valid(&old_curbuf) ||
	    old_curwin->w_بفر != old_curbuf.br_buf)
    {
	اگر (newbuf_حالت == OK)
	    سیٹ_bufref(&bufref, curbuf);
	اگر (cmdwin_valid && !آخری_کھڑکی())
	    win_close(cmdwin_win, TRUE);

	// win_close() autocommands may have already deleted the buffer.
	اگر (newbuf_حالت == OK && bufref_valid(&bufref) &&
		bufref.br_buf != curbuf)
	    close_بفر(تہی, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	cmdwin_type = 0;
	cmdwin_win = تہی;
	beep_flush();
	ga_clear(&winsizes);
	واپس Ctrl_C;
    }
    cmdwin_buf = curbuf;

    سیٹ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    curbuf->b_p_ma = TRUE;
#اگر_تعریف_شدہ FEAT_FOLDING
    curwin->w_p_fen = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    curwin->w_p_rl = cmdmsg_rl;
    cmdmsg_rl = FALSE;
#اگر_ختم
    RESET_BINDING(curwin);

    // Don't allow switching to another buffer.
    ++curbuf_تالا;

    // Showing the prompt may have set need_wait_return, reset it.
    need_wait_return = FALSE;

    histtype = hist_char2type(cmdwin_type);
    اگر (histtype == HIST_CMD || histtype == HIST_DEBUG)
    {
	اگر (p_wc == TAB)
	{
	    // Make Tab start command-line completion: CTRL-X CTRL-V
	    جوڑو_map((char_u *)"<buffer> <Tab> <C-X><C-V>", منوال_داخل_کرو, TRUE);
	    جوڑو_map((char_u *)"<buffer> <Tab> a<C-X><C-V>", منوال_معمول, TRUE);

	    // Make S-Tab work like CTRL-P in command-line completion
	    جوڑو_map((char_u *)"<buffer> <S-Tab> <C-P>", منوال_داخل_کرو, TRUE);
	}
	سیٹ_option_قدر_give_err((char_u *)"ft",
					       0L, (char_u *)"vim", OPT_LOCAL);
    }
    --curbuf_تالا;

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
	    کرو
	    {
		اگر (++i == get_hislen())
		    i = 0;
		اگر (get_histentry(histtype)[i].hisstr != تہی)
		    ml_append(lnum++, get_histentry(histtype)[i].hisstr,
							   (colnr_T)0, FALSE);
	    }
	    جب (i != *get_hisidx(histtype));
	}
    }

    // Replace the empty last line with the current command-line and put the
    // cursor there.
    ml_replace(curbuf->b_ml.ml_سطر_گنتی, ccline.cmdbuff, TRUE);
    curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    curwin->w_cursor.ستون = ccline.cmdpos;
    بدلا_سطر_abv_curs();
    invalidate_botline();
    redraw_later(UPD_SOME_VALID);

    // No Ex mode here!
    exmode_فعال = 0;

    State = منوال_معمول;
    check_cursor();
    setmouse();
    clear_showcmd();

    // Reset here so it can be set by a CmdwinEnter autocommand.
    cmdwin_نتیجہ = 0;

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

#اگر_تعریف_شدہ FEAT_FOLDING
    save_کلید_Typed = کلید_Typed;
#اگر_ختم

    // Trigger CmdwinLeave autocommands.
    trigger_cmd_autocmd(cmdwin_type, EVENT_CMDWINLEAVE);

#اگر_تعریف_شدہ FEAT_FOLDING
    // Restore KeyTyped in case it is modified by autocommands
    کلید_Typed = save_کلید_Typed;
#اگر_ختم

    cmdwin_type = 0;
    cmdwin_buf = تہی;
    cmdwin_win = تہی;
    exmode_فعال = save_exmode;

    // Safety check: The old window or buffer was changed or deleted: It's a bug
    // when this happens!
    اگر (!win_valid(old_curwin) || !bufref_valid(&old_curbuf)
	    || old_curwin->w_بفر != old_curbuf.br_buf)
    {
	cmdwin_نتیجہ = Ctrl_C;
	emsg(_(e_فعال_کھڑکی_or_بفر_بدلا_or_deleted));
    }
    ورنہ
    {
#اگر defined(FEAT_EVAL)
	// autocmds may abort script processing
	اگر (aborting() && cmdwin_نتیجہ != K_IGNORE)
	    cmdwin_نتیجہ = Ctrl_C;
#اگر_ختم
	// Set the new command line from the cmdline buffer.
	dealloc_cmdbuff();

	اگر (cmdwin_نتیجہ == K_XF1 || cmdwin_نتیجہ == K_XF2) // :qa[!] typed
	{
	    char_u  *p = (char_u *)"qa";	    // assume cmdwin_result == K_XF2
	    size_t  plen = 2;

	    اگر (cmdwin_نتیجہ == K_XF1)
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
		cmdwin_نتیجہ = CAR;
	    }
	    ورنہ
	    {
		// First need to cancel what we were doing.
		stuffcharReadbuff(':');
		stuffReadbuff((char_u *)p);
		stuffcharReadbuff(CAR);
	    }
	}
	ورنہ اگر (cmdwin_نتیجہ == Ctrl_C)
	{
	    // :q or :close, don't execute any command
	    // and don't modify the cmd window.
	    ccline.cmdbuff = تہی;
	}
	ورنہ
	{
	    ccline.cmdlen = ml_get_curline_len();
	    ccline.cmdbufflen = ccline.cmdlen + 1;
	    ccline.cmdbuff = vim_strnsave(ml_get_curline(), ccline.cmdlen);
	}

	اگر (ccline.cmdbuff == تہی)
	{
	    ccline.cmdbuff = vim_strnsave((char_u *)"", 0);
	    ccline.cmdlen = 0;
	    ccline.cmdbufflen = 1;
	    ccline.cmdpos = 0;
	    cmdwin_نتیجہ = Ctrl_C;
	}
	ورنہ
	{
	    ccline.cmdpos = curwin->w_cursor.ستون;
	    // If the cursor is on the last character, it probably should be
	    // after it.
	    اگر (ccline.cmdpos == ccline.cmdlen - 1
		    || ccline.cmdpos > ccline.cmdlen)
		ccline.cmdpos = ccline.cmdlen;
	}

#اگر_تعریف_شدہ FEAT_CONCEAL
	// Avoid command-line window first character being concealed.
	curwin->w_p_cole = 0;
#اگر_ختم
	// First go back to the original window.
	wp = curwin;
	سیٹ_bufref(&bufref, curbuf);

	skip_win_fix_cursor = TRUE;
	win_goto(old_curwin);

	// win_goto() may trigger an autocommand that already closes the
	// cmdline window.
	اگر (win_valid(wp) && wp != curwin)
	    win_close(wp, TRUE);

	// win_close() may have already wiped the buffer when 'bh' is
	// set to 'wipe', autocommands may have closed other windows
	اگر (bufref_valid(&bufref) && bufref.br_buf != curbuf)
	    close_بفر(تہی, bufref.br_buf, DOBUF_WIPE, FALSE, FALSE, FALSE);

	// Restore window sizes.
	win_سائز_restore(&winsizes);
	skip_win_fix_cursor = FALSE;

	اگر (cmdwin_نتیجہ == K_IGNORE)
	{
	    // It can be confusing that the cmdwin still shows, redraw the
	    // screen.
	    update_screen(UPD_VALID);
	    سیٹ_cmdspos_cursor();
	    redrawcmd();
	}
    }

    ga_clear(&winsizes);
    restart_edit = save_restart_edit;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    cmdmsg_rl = save_cmdmsg_rl;
#اگر_ختم

    State = save_State;
    may_trigger_modechanged();
    setmouse();

    واپس cmdwin_نتیجہ;
}

/*
 * Return TRUE if in the cmdwin, not editing the command line.
 */
    صحیح
is_اندر_cmdwin(خالی)
{
    واپس cmdwin_type != 0 && get_cmdline_type() == NUL;
}

/*
 * Used for commands that either take a simple command string argument, or:
 *	cmd << endmarker
 *	  {script}
 *	endmarker
 * Returns a pointer to allocated memory with {script} or NULL.
 */
    char_u *
script_get(exarg_T *eap غیر_مستعمل, char_u *cmd غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    list_T	*l;
    listitem_T	*li;
    char_u	*s;
    garray_T	ga;

    اگر (cmd[0] != '<' || cmd[1] != '<' || eap->ea_getline == تہی)
	واپس تہی;
    cmd += 2;

    l = heredoc_get(eap, cmd, TRUE, FALSE);
    اگر (l == تہی)
	واپس تہی;

    ga_init2(&ga, 1, 0x400);

    FOR_ALL_LIST_اشیاء(l, li)
    {
	s = tv_get_string(&li->li_tv);
	ga_concat(&ga, s);
	ga_append(&ga, '\n');
    }
    ga_append(&ga, NUL);

    list_free(l);
    واپس (char_u *)ga.ga_ڈیٹا;
#ورنہ
    واپس تہی;
#اگر_ختم
}

#اگر defined(FEAT_EVAL)
/*
 * This function is used by f_input() and f_inputdialog() functions. The third
 * argument to f_input() specifies the type of completion to use at the
 * prompt. The third argument to f_inputdialog() specifies the value to return
 * when the user cancels the prompt.
 */
    خالی
get_user_داخلہ(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		inputdialog,
    صحیح		secret)
{
    char_u	*prompt;
    char_u	*p = تہی;
    صحیح		c;
    char_u	buf[NUMBUFLEN];
    صحیح		cmd_silent_save = cmd_silent;
    char_u	*defstr = (char_u *)"";
    صحیح		xp_type = EXPAND_NOTHING;
    char_u	*xp_arg = تہی;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    اگر (داخلہ_busy)
	واپس;  // this doesn't work recursively.

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && check_for_opt_string_arg(argvars, 2) == FAIL)))
	واپس;

    prompt = tv_get_string_chk(&argvars[0]);

# ifdef NO_CONSOLE_داخلہ
    // While starting up, there is no place to enter text. When running tests
    // with --not-a-term we assume feedkeys() will be used.
    اگر (no_console_داخلہ() && !is_not_a_رکن())
	واپس;
# endif

    cmd_silent = FALSE;		// Want to see the prompt.
    اگر (prompt != تہی)
    {
	// Only the part of the message after the last NL is considered as
	// prompt for the command line
	p = vim_strrchr(prompt, '\n');
	اگر (p == تہی)
	    p = prompt;
	ورنہ
	{
	    ++p;
	    c = *p;
	    *p = NUL;
	    پیغام_آغاز();
	    پیغام_clr_eos();
	    پیغام_puts_attr((حرف *)prompt, get_echo_attr());
	    پیغام_didout = FALSE;
	    پیغام_starthere();
	    *p = c;
	}
	cmdline_صف = پیغام_صف;

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	{
	    defstr = tv_get_string_buf_chk(&argvars[1], buf);
	    اگر (defstr != تہی)
		stuffReadbuffSpec(defstr);

	    اگر (!inputdialog && argvars[2].v_type != VAR_UNKNOWN)
	    {
		char_u	*xp_name;
		صحیح	xp_namelen;
		طویل	argt = 0;

		// input() with a third argument: completion
		rettv->vval.v_string = تہی;

		xp_name = tv_get_string_buf_chk(&argvars[2], buf);
		اگر (xp_name == تہی)
		    واپس;

		xp_namelen = (صحیح)STRLEN(xp_name);

		اگر (parse_compl_arg(xp_name, xp_namelen, &xp_type, &argt,
							     &xp_arg) == FAIL)
		    واپس;
	    }
	}

	اگر (defstr != تہی)
	{
	    صحیح save_ex_معمول_busy = ex_معمول_busy;
	    صحیح save_vgetc_busy = vgetc_busy;
	    صحیح save_داخلہ_busy = داخلہ_busy;

	    داخلہ_busy |= vgetc_busy;
	    ex_معمول_busy = 0;
	    vgetc_busy = 0;
	    rettv->vval.v_string =
		getcmdline_prompt(secret ? NUL : '@', p, get_echo_attr(),
							      xp_type, xp_arg);
	    ex_معمول_busy = save_ex_معمول_busy;
	    vgetc_busy = save_vgetc_busy;
	    داخلہ_busy = save_داخلہ_busy;
	}
	اگر (inputdialog && rettv->vval.v_string == تہی
		&& argvars[1].v_type != VAR_UNKNOWN
		&& argvars[2].v_type != VAR_UNKNOWN)
	    rettv->vval.v_string = vim_strsave(tv_get_string_buf(
							   &argvars[2], buf));

	vim_free(xp_arg);

	// since the user typed this, no need to wait for return
	need_wait_return = FALSE;
	پیغام_didout = FALSE;
    }
    cmd_silent = cmd_silent_save;
}
#اگر_ختم

/*
 * "wildtrigger()" function
 */
    خالی
f_wildtrigger(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
    اگر (!(State & منوال_CMDLINE) || char_avail() || wild_menu_showing
	    || cmdline_pum_فعال())
	واپس;

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
