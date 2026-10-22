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

#அந்தர்பாவ "vim.h"

நிலையான முழு	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    முழு
has_format_option(முழு x)
{
    எனில் (p_paste)
	திரும்பு FALSE;
    திரும்பு (vim_strchr(curbuf->b_p_fo, x) != வெற்று);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    வெறுமை
internal_format(
    முழு		textwidth,
    முழு		second_indent,
    முழு		flags,
    முழு		format_only,
    முழு		c) // character to be inserted (can be NUL)
{
    முழு		cc;
    முழு		skip_pos;
    முழு		save_char = NUL;
    முழு		haveto_redraw = FALSE;
    முழு		fo_ins_blank = has_format_option(FO_INS_BLANK);
    முழு		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    முழு		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    முழு		fo_white_par = has_format_option(FO_WHITE_PAR);
    முழு		first_வரி = TRUE;
    colnr_T	leader_len;
    முழு		no_leader = FALSE;
    முழு		do_comments = (flags & INSCHAR_DO_COM);
    முழு		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    முழு		has_lbr = curwin->w_p_lbr;
    முழு		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    எனில் (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	எனில் (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    போது (!got_int)
    {
	முழு	startcol;		// Cursor column at entry
	முழு	wantcol;		// column at textwidth border
	முழு	foundcol;		// column for start of spaces
	முழு	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	முழு	orig_col = 0;
	char_u	*saved_text = வெற்று;
	colnr_T	col;
	colnr_T	end_col;
	முழு	wcc;			// counter for whitespace chars
	முழு	did_do_comment = FALSE;
	முழு	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	எனில் (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    எனில் (virtcol <= (colnr_T)textwidth)
		நிறுத்து;
	}

	எனில் (no_leader)
	    do_comments = FALSE;
	இல்லையெனில் எனில் (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	எனில் (do_comments)
	{
	    char_u *வரி = ml_get_curline();

	    leader_len = get_leader_len(வரி, வெற்று, FALSE, TRUE);
	    எனில் (leader_len == 0 && curbuf->b_p_cin)
	    {
		முழு		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(வரி);
		எனில் (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      வரி + comment_start, வெற்று, FALSE, TRUE);
		    எனில் (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	இல்லையெனில்
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	எனில் (leader_len == 0)
	    no_leader = TRUE;
	எனில் (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    நிறுத்து;
	எனில் ((startcol = curwin->w_cursor.col) == 0)
	    நிறுத்து;

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
	போது ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    எனில் (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    இல்லையெனில்
		cc = gchar_cursor();
	    எனில் (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		போது (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    எனில் (wcc < 2)
			wcc++;
		}
		எனில் (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    நிறுத்து;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		எனில் (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    தொடர்;

		// Don't break until after the comment leader
		எனில் (curwin->w_cursor.col < leader_len)
		    நிறுத்து;
		எனில் (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    எனில் (curwin->w_cursor.col == 0)
			நிறுத்து;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    எனில் (curwin->w_cursor.col <= leader_len)
			நிறுத்து;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    எனில் (WHITECHAR(cc))
			தொடர்;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		எனில் (curwin->w_cursor.col <= (colnr_T)wantcol)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		முழு ncc;
		முழு allow_break;

		// Break after or before a multi-byte character.
		எனில் (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    எனில் (curwin->w_cursor.col < leader_len)
			நிறுத்து;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    எனில் (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			எனில் (curwin->w_cursor.col <= (colnr_T)wantcol)
			    நிறுத்து;
		    }
		    curwin->w_cursor.col = col;
		}

		எனில் (curwin->w_cursor.col == 0)
		    நிறுத்து;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		எனில் (WHITECHAR(cc))
		    தொடர்;		// break with space
		// Don't break until after the comment leader.
		எனில் (curwin->w_cursor.col < leader_len)
		    நிறுத்து;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		எனில் (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		எனில் (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    முழு ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    எனில் (allow_break)
			நிறுத்து;
		    எனில் (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			எனில் (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    நிறுத்து;
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

			எனில் (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    நிறுத்து;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    எனில் (curwin->w_cursor.col == 0)
		நிறுத்து;
	    dec_cursor();
	}

	எனில் (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    நிறுத்து;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	எனில் (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	இல்லையெனில்
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	போது ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	எனில் (startcol < 0)
	    startcol = 0;

	எனில் (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    எனில் (saved_text == வெற்று)
		நிறுத்து;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    எனில் (!fo_white_par)
		backspace_until_column(foundcol);
	}
	இல்லையெனில்
	{
	    // put cursor after pos. to break line
	    எனில் (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_வரி(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	எனில் (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	எனில் (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	எனில் (first_வரி)
	{
	    எனில் (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		எனில் (second_indent < 0 && has_format_option(FO_Q_எண்))
		    second_indent =
				 get_எண்_indent(curwin->w_cursor.lnum - 1);
		எனில் (second_indent >= 0)
		{
		    எனில் (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    இல்லையெனில்
			எனில் (leader_len > 0 && second_indent - leader_len > 0)
		    {
			முழு i;
			முழு padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			க்கு (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    இல்லையெனில்
		    {
			(வெறுமை)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_வரி = FALSE;
	}

	எனில் (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	இல்லையெனில்
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    எனில் (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	வரி_breakcheck();
    }

    எனில் (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    எனில் (!format_only && haveto_redraw)
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
    நிலையான முழு
fmt_check_par(
    linenr_T	lnum,
    முழு		*leader_len,
    char_u	**leader_flags,
    முழு		do_comments)
{
    char_u	*flags = வெற்று;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    எனில் (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    இல்லையெனில்
	*leader_len = 0;

    எனில் (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	போது (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    திரும்பு (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    நிலையான முழு
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    எனில் (*s == NUL)
	திரும்பு FALSE;
    l = ml_get_len(lnum) - 1;
    திரும்பு VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    நிலையான முழு
same_leader(
    linenr_T lnum,
    முழு	    leader1_len,
    char_u  *leader1_flags,
    முழு	    leader2_len,
    char_u  *leader2_flags)
{
    முழு	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    எனில் (leader1_len == 0)
	திரும்பு (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    எனில் (leader1_flags != வெற்று)
    {
	char_u	*p;

	க்கு (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    எனில் (*p == COM_FIRST)
		திரும்பு (leader2_len == 0);
	    எனில் (*p == COM_END)
		திரும்பு FALSE;
	    எனில் (*p == COM_START)
	    {
		முழு வரி_len = ml_get_len(lnum);
		எனில் (வரி_len <= leader1_len)
		    திரும்பு FALSE;
		எனில் (leader2_flags == வெற்று || leader2_len == 0)
		    திரும்பு FALSE;
		க்கு (p = leader2_flags; *p && *p != ':'; ++p)
		    எனில் (*p == COM_நடு)
			திரும்பு TRUE;
		திரும்பு FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    எனில் (line1 != வெற்று)
    {
	char_u  *line2;

	க்கு (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	க்கு (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    எனில் (!VIM_ISWHITE(line2[idx2]))
	    {
		எனில் (line1[idx1++] != line2[idx2])
		    நிறுத்து;
	    }
	    இல்லையெனில்
		போது (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    திரும்பு (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    நிலையான முழு
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    முழு		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = வெற்று;	// flags for leader of current line
    முழு		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    முழு		do_comments;		// format comments

    எனில் (lnum <= 1)
	திரும்பு TRUE;		// start of the file

    p = ml_get(lnum - 1);
    எனில் (*p == NUL)
	திரும்பு TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    எனில் (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	திரும்பு TRUE;		// after non-paragraph line

    எனில் (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	திரும்பு TRUE;		// "lnum" is not a paragraph line

    எனில் (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	திரும்பு TRUE;		// missing trailing space in previous line.

    எனில் (has_format_option(FO_Q_எண்) && (get_எண்_indent(lnum) > 0))
	திரும்பு TRUE;		// numbered item starts in "lnum".

    எனில் (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	திரும்பு TRUE;		// change of comment leader.

    திரும்பு FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    வெறுமை
auto_format(
    முழு		trailblank,	// when TRUE also format with trailing blank
    முழு		prev_வரி)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    முழு		wasatend;

    எனில் (!has_format_option(FO_AUTO))
	திரும்பு;

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
    எனில் (*old != NUL && !trailblank && wasatend)
    {
	முழு cc;

	dec_cursor();
	cc = gchar_cursor();
	எனில் (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	எனில் (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    திரும்பு;
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
    எனில் (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_செருகு))
    {
	char_u *வரி = ml_get_curline();
	எனில் (WHITECHAR(வரி[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    திரும்பு;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    எனில் (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, வெற்று, FALSE, TRUE) == 0)
	திரும்பு;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    எனில் (prev_வரி && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	எனில் (u_save_cursor() == FAIL)
	    திரும்பு;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_வரிகள்((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    எனில் (curwin->w_cursor.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	coladvance((colnr_T)MAXCOL);
    }
    இல்லையெனில்
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    எனில் (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	எனில் (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    எனில் (pnew == வெற்று)
		திரும்பு;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	இல்லையெனில்
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
    வெறுமை
check_auto_format(
    முழு		end_செருகு)	    // TRUE when ending Insert mode
{
    முழு		c = ' ';
    முழு		cc;

    எனில் (!did_add_space)
	திரும்பு;

    cc = gchar_cursor();
    எனில் (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    இல்லையெனில்
    {
	எனில் (!end_செருகு)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	எனில் (c != NUL)
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
    முழு
comp_textwidth(
    முழு		ff)	// force formatting (for "gq" command)
{
    முழு		textwidth;

    textwidth = curbuf->b_p_tw;
    எனில் (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	எனில் (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	எனில் (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	எனில் (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    எனில் (textwidth < 0)
	textwidth = 0;
    எனில் (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	எனில் (textwidth > 79)
	    textwidth = 79;
    }
    திரும்பு textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    வெறுமை
op_format(
    oparg_T	*oap,
    முழு		keep_cursor)		// keep cursor on same text char
{
    long	old_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    எனில் (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	திரும்பு;
    curwin->w_cursor = oap->start;

    எனில் (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    எனில் (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_வரிகள்(oap->வரி_மொத்தம், keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    எனில் (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_வரி_மொத்தம்)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம் - old_வரி_மொத்தம்;
    msgmore(old_வரி_மொத்தம்);

    எனில் ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    எனில் (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    எனில் (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		எனில் (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_வரி_மொத்தம்;
		இல்லையெனில்
		    wp->w_old_visual_lnum += old_வரி_மொத்தம்;
	    }
	}
    }
}

#எனில் defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    வெறுமை
op_formatexpr(oparg_T *oap)
{
    எனில் (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    எனில் (fex_format(oap->start.lnum, oap->வரி_மொத்தம், NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    முழு
fex_format(
    linenr_T	lnum,
    long	மொத்தம்,
    முழு		c)	// character to be inserted
{
    முழு		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    முழு		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_மொத்தம், மொத்தம்);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    எனில் (fex == வெற்று)
	திரும்பு 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    எனில் (use_sandbox)
	++sandbox;
    r = (முழு)eval_to_எண்(fex, TRUE);
    எனில் (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, வெற்று, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    திரும்பு r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    வெறுமை
format_வரிகள்(
    linenr_T	வரி_மொத்தம்,
    முழு		avoid_fex)		// don't use 'formatexpr'
{
    முழு		max_len;
    முழு		is_not_par;		// current line not part of parag.
    முழு		next_is_not_par;	// next line not part of paragraph
    முழு		is_end_par;		// at end of paragraph
    முழு		prev_is_end_par = FALSE;// prev. line not part of parag.
    முழு		next_is_start_par = FALSE;
    முழு		leader_len = 0;		// leader len of current line
    முழு		next_leader_len;	// leader len of next line
    char_u	*leader_flags = வெற்று;	// flags for leader of current line
    char_u	*next_leader_flags = வெற்று; // flags for leader of next line
    முழு		do_comments;		// format comments
    முழு		do_comments_list = 0;	// format comments with 'n' or '2'
    முழு		advance = TRUE;
    முழு		second_indent = -1;	// indent for second line (comment
					// aware)
    முழு		do_second_indent;
    முழு		do_எண்_indent;
    முழு		do_trail_white;
    முழு		first_par_வரி = TRUE;
    முழு		smd_save;
    long	மொத்தம்;
    முழு		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_வரி = curwin->w_cursor.lnum;
    முழு		force_format = FALSE;
    முழு		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_எண்_indent = has_format_option(FO_Q_எண்);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    எனில் (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    இல்லையெனில்
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    எனில் (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    க்கு (மொத்தம் = வரி_மொத்தம்; மொத்தம் != 0 && !got_int; --மொத்தம்)
    {
	// Advance to next paragraph.
	எனில் (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	எனில் (மொத்தம் == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = வெற்று;
	}
	இல்லையெனில்
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    எனில் (do_எண்_indent)
		next_is_start_par =
			   (get_எண்_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	எனில் (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	எனில் (is_not_par)
	{
	    எனில் (வரி_மொத்தம் < 0)
		நிறுத்து;
	}
	இல்லையெனில்
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    எனில் (first_par_வரி
		    && (do_second_indent || do_எண்_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_வரி_மொத்தம்)
	    {
		எனில் (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    எனில் (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    இல்லையெனில்
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		இல்லையெனில் எனில் (do_எண்_indent)
		{
		    எனில் (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_எண்_indent(curwin->w_cursor.lnum);
		    }
		    இல்லையெனில்
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_எண்_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    எனில் (curwin->w_cursor.lnum >= curbuf->b_ml.ml_வரி_மொத்தம்
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		எனில் (next_leader_flags == வெற்று
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    எனில் (is_end_par || force_format)
	    {
		எனில் (need_set_indent)
		{
		    முழு		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    எனில் (curwin->w_cursor.lnum == first_வரி)
			indent = get_indent();
		    இல்லையெனில் எனில் (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    இல்லையெனில்
		    {
			எனில் (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_கோவை_indent() :
#endif
				 get_c_indent();
			}
			இல்லையெனில்
			    indent = get_indent();
		    }
		    (வெறுமை)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		போது (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_செருகு;	// for open_line()
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
		எனில் (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    எனில் (வரி_மொத்தம் < 0)
			நிறுத்து;
		    first_par_வரி = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    எனில் (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		எனில் (வரி_மொத்தம் < 0 && u_save_cursor() == FAIL)
		    நிறுத்து;
		எனில் (next_leader_len > 0)
		{
		    (வெறுமை)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		இல்லையெனில் எனில் (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    முழு indent = getwhitecols_curline();

		    எனில் (indent > 0)
		    {
			(வெறுமை)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		எனில் (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    நிறுத்து;
		}
		first_par_வரி = FALSE;
		// If the line is getting long, format it next time
		எனில் (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		இல்லையெனில்
		    force_format = FALSE;
	    }
	}
	வரி_breakcheck();
    }
}
