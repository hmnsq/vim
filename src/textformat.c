/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * textformat.c: text formatting functions
 */

#അന്തര്ഭാവ "vim.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    പൂർണ്ണസംഖ്യ
has_format_option(പൂർണ്ണസംഖ്യ x)
{
    എങ്കിൽ (p_paste)
	തിരികെ FALSE;
    തിരികെ (vim_strchr(curbuf->b_p_fo, x) != ശൂന്യ);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    ശൂന്യം
internal_format(
    പൂർണ്ണസംഖ്യ		textwidth,
    പൂർണ്ണസംഖ്യ		second_indent,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		format_only,
    പൂർണ്ണസംഖ്യ		c) // character to be inserted (can be NUL)
{
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		skip_pos;
    പൂർണ്ണസംഖ്യ		save_char = NUL;
    പൂർണ്ണസംഖ്യ		haveto_redraw = FALSE;
    പൂർണ്ണസംഖ്യ		fo_ins_blank = has_format_option(FO_INS_BLANK);
    പൂർണ്ണസംഖ്യ		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    പൂർണ്ണസംഖ്യ		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    പൂർണ്ണസംഖ്യ		fo_white_par = has_format_option(FO_WHITE_PAR);
    പൂർണ്ണസംഖ്യ		first_വരി = TRUE;
    colnr_T	leader_len;
    പൂർണ്ണസംഖ്യ		no_leader = FALSE;
    പൂർണ്ണസംഖ്യ		do_comments = (flags & INSCHAR_DO_COM);
    പൂർണ്ണസംഖ്യ		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    പൂർണ്ണസംഖ്യ		has_lbr = curwin->w_p_lbr;
    പൂർണ്ണസംഖ്യ		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    എങ്കിൽ (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	എങ്കിൽ (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ആകുമ്പോൾ (!got_int)
    {
	പൂർണ്ണസംഖ്യ	startcol;		// Cursor column at entry
	പൂർണ്ണസംഖ്യ	wantcol;		// column at textwidth border
	പൂർണ്ണസംഖ്യ	foundcol;		// column for start of spaces
	പൂർണ്ണസംഖ്യ	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	പൂർണ്ണസംഖ്യ	orig_col = 0;
	char_u	*saved_text = ശൂന്യ;
	colnr_T	col;
	colnr_T	end_col;
	പൂർണ്ണസംഖ്യ	wcc;			// counter for whitespace chars
	പൂർണ്ണസംഖ്യ	did_do_comment = FALSE;
	പൂർണ്ണസംഖ്യ	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	എങ്കിൽ (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    എങ്കിൽ (virtcol <= (colnr_T)textwidth)
		നിർത്തുക;
	}

	എങ്കിൽ (no_leader)
	    do_comments = FALSE;
	അല്ലെങ്കിൽ എങ്കിൽ (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	എങ്കിൽ (do_comments)
	{
	    char_u *വരി = ml_get_curline();

	    leader_len = get_leader_len(വരി, ശൂന്യ, FALSE, TRUE);
	    എങ്കിൽ (leader_len == 0 && curbuf->b_p_cin)
	    {
		പൂർണ്ണസംഖ്യ		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(വരി);
		എങ്കിൽ (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      വരി + comment_start, ശൂന്യ, FALSE, TRUE);
		    എങ്കിൽ (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	എങ്കിൽ (leader_len == 0)
	    no_leader = TRUE;
	എങ്കിൽ (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    നിർത്തുക;
	എങ്കിൽ ((startcol = curwin->w_cursor.col) == 0)
	    നിർത്തുക;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.col;

	// If startcol is large (a long line), formatting takes too much
	// time. The algorithm is O(n^2), it walks from the end of the
	// line to textwidth border every time for each line break.
	//
	// Ceil to 8 * textwidth to optimize.
	curwin->w_cursor.col = startcol < safe_tw ? startcol : safe_tw;

	foundcol = 0;
	skip_pos = 0;
	first_pass = TRUE;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	ആകുമ്പോൾ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    എങ്കിൽ (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    അല്ലെങ്കിൽ
		cc = gchar_cursor();
	    എങ്കിൽ (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		ആകുമ്പോൾ (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    എങ്കിൽ (wcc < 2)
			wcc++;
		}
		എങ്കിൽ (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    നിർത്തുക;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		എങ്കിൽ (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    തുടരുക;

		// Don't break until after the comment leader
		എങ്കിൽ (curwin->w_cursor.col < leader_len)
		    നിർത്തുക;
		എങ്കിൽ (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    എങ്കിൽ (curwin->w_cursor.col == 0)
			നിർത്തുക;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    എങ്കിൽ (curwin->w_cursor.col <= leader_len)
			നിർത്തുക;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    എങ്കിൽ (WHITECHAR(cc))
			തുടരുക;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		എങ്കിൽ (curwin->w_cursor.col <= (colnr_T)wantcol)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		പൂർണ്ണസംഖ്യ ncc;
		പൂർണ്ണസംഖ്യ allow_break;

		// Break after or before a multi-byte character.
		എങ്കിൽ (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    എങ്കിൽ (curwin->w_cursor.col < leader_len)
			നിർത്തുക;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    എങ്കിൽ (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			എങ്കിൽ (curwin->w_cursor.col <= (colnr_T)wantcol)
			    നിർത്തുക;
		    }
		    curwin->w_cursor.col = col;
		}

		എങ്കിൽ (curwin->w_cursor.col == 0)
		    നിർത്തുക;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		എങ്കിൽ (WHITECHAR(cc))
		    തുടരുക;		// break with space
		// Don't break until after the comment leader.
		എങ്കിൽ (curwin->w_cursor.col < leader_len)
		    നിർത്തുക;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		എങ്കിൽ (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		എങ്കിൽ (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    പൂർണ്ണസംഖ്യ ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    എങ്കിൽ (allow_break)
			നിർത്തുക;
		    എങ്കിൽ (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			എങ്കിൽ (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    നിർത്തുക;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			col = curwin->w_cursor.col;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			എങ്കിൽ (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    നിർത്തുക;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    എങ്കിൽ (curwin->w_cursor.col == 0)
		നിർത്തുക;
	    dec_cursor();
	}

	എങ്കിൽ (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    നിർത്തുക;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	എങ്കിൽ (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	അല്ലെങ്കിൽ
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	ആകുമ്പോൾ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	എങ്കിൽ (startcol < 0)
	    startcol = 0;

	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    എങ്കിൽ (saved_text == ശൂന്യ)
		നിർത്തുക;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    എങ്കിൽ (!fo_white_par)
		backspace_until_column(foundcol);
	}
	അല്ലെങ്കിൽ
	{
	    // put cursor after pos. to break line
	    എങ്കിൽ (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_വരി(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	എങ്കിൽ (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	എങ്കിൽ (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	എങ്കിൽ (first_വരി)
	{
	    എങ്കിൽ (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		എങ്കിൽ (second_indent < 0 && has_format_option(FO_Q_സംഖ്യ))
		    second_indent =
				 get_സംഖ്യ_indent(curwin->w_cursor.lnum - 1);
		എങ്കിൽ (second_indent >= 0)
		{
		    എങ്കിൽ (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    അല്ലെങ്കിൽ
			എങ്കിൽ (leader_len > 0 && second_indent - leader_len > 0)
		    {
			പൂർണ്ണസംഖ്യ i;
			പൂർണ്ണസംഖ്യ padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			വേണ്ടി (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    അല്ലെങ്കിൽ
		    {
			(ശൂന്യം)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_വരി = FALSE;
	}

	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	അല്ലെങ്കിൽ
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    എങ്കിൽ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	വരി_breakcheck();
    }

    എങ്കിൽ (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    എങ്കിൽ (!format_only && haveto_redraw)
    {
	update_topline();
	redraw_curbuf_later(UPD_VALID);
    }
}

/*
 * Blank lines, and lines containing only the comment leader, are left
 * untouched by the formatting.  The function returns TRUE in this
 * case.  It also returns TRUE when a line starts with the end of a comment
 * ('e' in comment flags), so that this line is skipped, and not joined to the
 * previous line.  A new paragraph starts after a blank line, or when the
 * comment leader changes -- webb.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fmt_check_par(
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		*leader_len,
    char_u	**leader_flags,
    പൂർണ്ണസംഖ്യ		do_comments)
{
    char_u	*flags = ശൂന്യ;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    എങ്കിൽ (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    അല്ലെങ്കിൽ
	*leader_len = 0;

    എങ്കിൽ (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ആകുമ്പോൾ (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    തിരികെ (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    എങ്കിൽ (*s == NUL)
	തിരികെ FALSE;
    l = ml_get_len(lnum) - 1;
    തിരികെ VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
same_leader(
    linenr_T lnum,
    പൂർണ്ണസംഖ്യ	    leader1_len,
    char_u  *leader1_flags,
    പൂർണ്ണസംഖ്യ	    leader2_len,
    char_u  *leader2_flags)
{
    പൂർണ്ണസംഖ്യ	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    എങ്കിൽ (leader1_len == 0)
	തിരികെ (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    എങ്കിൽ (leader1_flags != ശൂന്യ)
    {
	char_u	*p;

	വേണ്ടി (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    എങ്കിൽ (*p == COM_FIRST)
		തിരികെ (leader2_len == 0);
	    എങ്കിൽ (*p == COM_END)
		തിരികെ FALSE;
	    എങ്കിൽ (*p == COM_START)
	    {
		പൂർണ്ണസംഖ്യ വരി_len = ml_get_len(lnum);
		എങ്കിൽ (വരി_len <= leader1_len)
		    തിരികെ FALSE;
		എങ്കിൽ (leader2_flags == ശൂന്യ || leader2_len == 0)
		    തിരികെ FALSE;
		വേണ്ടി (p = leader2_flags; *p && *p != ':'; ++p)
		    എങ്കിൽ (*p == COM_നടുവ്)
			തിരികെ TRUE;
		തിരികെ FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    എങ്കിൽ (line1 != ശൂന്യ)
    {
	char_u  *line2;

	വേണ്ടി (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	വേണ്ടി (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    എങ്കിൽ (!VIM_ISWHITE(line2[idx2]))
	    {
		എങ്കിൽ (line1[idx1++] != line2[idx2])
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
		ആകുമ്പോൾ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    തിരികെ (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ശൂന്യ;	// flags for leader of current line
    പൂർണ്ണസംഖ്യ		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    പൂർണ്ണസംഖ്യ		do_comments;		// format comments

    എങ്കിൽ (lnum <= 1)
	തിരികെ TRUE;		// start of the file

    p = ml_get(lnum - 1);
    എങ്കിൽ (*p == NUL)
	തിരികെ TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    എങ്കിൽ (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	തിരികെ TRUE;		// after non-paragraph line

    എങ്കിൽ (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	തിരികെ TRUE;		// "lnum" is not a paragraph line

    എങ്കിൽ (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	തിരികെ TRUE;		// missing trailing space in previous line.

    എങ്കിൽ (has_format_option(FO_Q_സംഖ്യ) && (get_സംഖ്യ_indent(lnum) > 0))
	തിരികെ TRUE;		// numbered item starts in "lnum".

    എങ്കിൽ (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	തിരികെ TRUE;		// change of comment leader.

    തിരികെ FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    ശൂന്യം
auto_format(
    പൂർണ്ണസംഖ്യ		trailblank,	// when TRUE also format with trailing blank
    പൂർണ്ണസംഖ്യ		prev_വരി)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    പൂർണ്ണസംഖ്യ		wasatend;

    എങ്കിൽ (!has_format_option(FO_AUTO))
	തിരികെ;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.col == ml_get_curline_len());
    എങ്കിൽ (*old != NUL && !trailblank && wasatend)
    {
	പൂർണ്ണസംഖ്യ cc;

	dec_cursor();
	cc = gchar_cursor();
	എങ്കിൽ (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	എങ്കിൽ (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    തിരികെ;
	}
	curwin->w_cursor = pos;
    }

    // Also skip formatting when the user just typed whitespace in the
    // middle of the line.  Reformatting would join all paragraph lines and
    // re-wrap, consuming the space at the line break point via
    // OPENLINE_DELSPACES.  By deferring, the next non-whitespace character
    // will be inserted adjacent to the space, keeping it protected from
    // being consumed at a line break.  auto_format() will then reformat
    // properly on the next keystroke.
    എങ്കിൽ (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_തിരുകുക))
    {
	char_u *വരി = ml_get_curline();
	എങ്കിൽ (WHITECHAR(വരി[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    തിരികെ;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    എങ്കിൽ (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ശൂന്യ, FALSE, TRUE) == 0)
	തിരികെ;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    എങ്കിൽ (prev_വരി && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	എങ്കിൽ (u_save_cursor() == FAIL)
	    തിരികെ;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_വരികൾ((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    എങ്കിൽ (curwin->w_cursor.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	coladvance((colnr_T)MAXCOL);
    }
    അല്ലെങ്കിൽ
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    എങ്കിൽ (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	എങ്കിൽ (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    എങ്കിൽ (pnew == ശൂന്യ)
		തിരികെ;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	അല്ലെങ്കിൽ
	    // may remove added space
	    check_auto_format(FALSE);
    }

    check_cursor();
}

/*
 * When an extra space was added to continue a paragraph for auto-formatting,
 * delete it now.  The space must be under the cursor, just after the insert
 * position.
 */
    ശൂന്യം
check_auto_format(
    പൂർണ്ണസംഖ്യ		end_തിരുകുക)	    // TRUE when ending Insert mode
{
    പൂർണ്ണസംഖ്യ		c = ' ';
    പൂർണ്ണസംഖ്യ		cc;

    എങ്കിൽ (!did_add_space)
	തിരികെ;

    cc = gchar_cursor();
    എങ്കിൽ (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!end_തിരുകുക)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	എങ്കിൽ (c != NUL)
	{
	    // The space is no longer at the end of the line, delete it.
	    del_char(FALSE);
	    did_add_space = FALSE;
	}
    }
}

/*
 * Find out textwidth to be used for formatting:
 *	if 'textwidth' option is set, use it
 *	else if 'wrapmargin' option is set, use curwin->w_width - 'wrapmargin'
 *	if invalid value, use 0.
 *	Set default to window width (maximum 79) for "gq" operator.
 */
    പൂർണ്ണസംഖ്യ
comp_textwidth(
    പൂർണ്ണസംഖ്യ		ff)	// force formatting (for "gq" command)
{
    പൂർണ്ണസംഖ്യ		textwidth;

    textwidth = curbuf->b_p_tw;
    എങ്കിൽ (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	എങ്കിൽ (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	എങ്കിൽ (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	എങ്കിൽ (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    എങ്കിൽ (textwidth < 0)
	textwidth = 0;
    എങ്കിൽ (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	എങ്കിൽ (textwidth > 79)
	    textwidth = 79;
    }
    തിരികെ textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    ശൂന്യം
op_format(
    oparg_T	*oap,
    പൂർണ്ണസംഖ്യ		keep_cursor)		// keep cursor on same text char
{
    long	old_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    എങ്കിൽ (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	തിരികെ;
    curwin->w_cursor = oap->start;

    എങ്കിൽ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    എങ്കിൽ (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_വരികൾ(oap->വരി_എണ്ണം, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    എങ്കിൽ (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_വരി_എണ്ണം)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം - old_വരി_എണ്ണം;
    msgmore(old_വരി_എണ്ണം);

    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    എങ്കിൽ (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    എങ്കിൽ (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		എങ്കിൽ (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_വരി_എണ്ണം;
		അല്ലെങ്കിൽ
		    wp->w_old_visual_lnum += old_വരി_എണ്ണം;
	    }
	}
    }
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    ശൂന്യം
op_formatexpr(oparg_T *oap)
{
    എങ്കിൽ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    എങ്കിൽ (fex_format(oap->start.lnum, oap->വരി_എണ്ണം, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    പൂർണ്ണസംഖ്യ
fex_format(
    linenr_T	lnum,
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		c)	// character to be inserted
{
    പൂർണ്ണസംഖ്യ		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    പൂർണ്ണസംഖ്യ		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_എണ്ണം, എണ്ണം);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    എങ്കിൽ (fex == ശൂന്യ)
	തിരികെ 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    എങ്കിൽ (use_sandbox)
	++sandbox;
    r = (പൂർണ്ണസംഖ്യ)eval_to_സംഖ്യ(fex, TRUE);
    എങ്കിൽ (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, ശൂന്യ, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    തിരികെ r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    ശൂന്യം
format_വരികൾ(
    linenr_T	വരി_എണ്ണം,
    പൂർണ്ണസംഖ്യ		avoid_fex)		// don't use 'formatexpr'
{
    പൂർണ്ണസംഖ്യ		max_len;
    പൂർണ്ണസംഖ്യ		is_not_par;		// current line not part of parag.
    പൂർണ്ണസംഖ്യ		next_is_not_par;	// next line not part of paragraph
    പൂർണ്ണസംഖ്യ		is_end_par;		// at end of paragraph
    പൂർണ്ണസംഖ്യ		prev_is_end_par = FALSE;// prev. line not part of parag.
    പൂർണ്ണസംഖ്യ		next_is_start_par = FALSE;
    പൂർണ്ണസംഖ്യ		leader_len = 0;		// leader len of current line
    പൂർണ്ണസംഖ്യ		next_leader_len;	// leader len of next line
    char_u	*leader_flags = ശൂന്യ;	// flags for leader of current line
    char_u	*next_leader_flags = ശൂന്യ; // flags for leader of next line
    പൂർണ്ണസംഖ്യ		do_comments;		// format comments
    പൂർണ്ണസംഖ്യ		do_comments_list = 0;	// format comments with 'n' or '2'
    പൂർണ്ണസംഖ്യ		advance = TRUE;
    പൂർണ്ണസംഖ്യ		second_indent = -1;	// indent for second line (comment
					// aware)
    പൂർണ്ണസംഖ്യ		do_second_indent;
    പൂർണ്ണസംഖ്യ		do_സംഖ്യ_indent;
    പൂർണ്ണസംഖ്യ		do_trail_white;
    പൂർണ്ണസംഖ്യ		first_par_വരി = TRUE;
    പൂർണ്ണസംഖ്യ		smd_save;
    long	എണ്ണം;
    പൂർണ്ണസംഖ്യ		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_വരി = curwin->w_cursor.lnum;
    പൂർണ്ണസംഖ്യ		force_format = FALSE;
    പൂർണ്ണസംഖ്യ		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_സംഖ്യ_indent = has_format_option(FO_Q_സംഖ്യ);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    എങ്കിൽ (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    അല്ലെങ്കിൽ
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    എങ്കിൽ (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    വേണ്ടി (എണ്ണം = വരി_എണ്ണം; എണ്ണം != 0 && !got_int; --എണ്ണം)
    {
	// Advance to next paragraph.
	എങ്കിൽ (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	എങ്കിൽ (എണ്ണം == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    എങ്കിൽ (do_സംഖ്യ_indent)
		next_is_start_par =
			   (get_സംഖ്യ_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	എങ്കിൽ (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	എങ്കിൽ (is_not_par)
	{
	    എങ്കിൽ (വരി_എണ്ണം < 0)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    എങ്കിൽ (first_par_വരി
		    && (do_second_indent || do_സംഖ്യ_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_വരി_എണ്ണം)
	    {
		എങ്കിൽ (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    എങ്കിൽ (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    അല്ലെങ്കിൽ
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (do_സംഖ്യ_indent)
		{
		    എങ്കിൽ (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_സംഖ്യ_indent(curwin->w_cursor.lnum);
		    }
		    അല്ലെങ്കിൽ
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_സംഖ്യ_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    എങ്കിൽ (curwin->w_cursor.lnum >= curbuf->b_ml.ml_വരി_എണ്ണം
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		എങ്കിൽ (next_leader_flags == ശൂന്യ
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    എങ്കിൽ (is_end_par || force_format)
	    {
		എങ്കിൽ (need_set_indent)
		{
		    പൂർണ്ണസംഖ്യ		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    എങ്കിൽ (curwin->w_cursor.lnum == first_വരി)
			indent = get_indent();
		    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_വ്യഞ്ജകം_indent() :
#endif
				 get_c_indent();
			}
			അല്ലെങ്കിൽ
			    indent = get_indent();
		    }
		    (ശൂന്യം)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ആകുമ്പോൾ (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_തിരുകുക;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;

		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), second_indent);

		State = old_State;
		p_smd = smd_save;
		// Cursor and mouse shape shapes may have been updated (e.g. by
		// :normal) in insertchar(), so they need to be updated here.
#ifdef CURSOR_SHAPE
		ui_cursor_shape();
#endif
#ifdef FEAT_MOUSESHAPE
		update_mouseshape(-1);
#endif

		second_indent = -1;
		// at end of par.: need to set indent of next par.
		need_set_indent = is_end_par;
		എങ്കിൽ (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    എങ്കിൽ (വരി_എണ്ണം < 0)
			നിർത്തുക;
		    first_par_വരി = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    എങ്കിൽ (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		എങ്കിൽ (വരി_എണ്ണം < 0 && u_save_cursor() == FAIL)
		    നിർത്തുക;
		എങ്കിൽ (next_leader_len > 0)
		{
		    (ശൂന്യം)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    പൂർണ്ണസംഖ്യ indent = getwhitecols_curline();

		    എങ്കിൽ (indent > 0)
		    {
			(ശൂന്യം)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		എങ്കിൽ (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    നിർത്തുക;
		}
		first_par_വരി = FALSE;
		// If the line is getting long, format it next time
		എങ്കിൽ (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		അല്ലെങ്കിൽ
		    force_format = FALSE;
	    }
	}
	വരി_breakcheck();
    }
}
