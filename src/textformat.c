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

#ضمن "vim.h"

ساكن صحيح	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    صحيح
has_format_option(صحيح x)
{
    إذا (p_paste)
	أرجع FALSE;
    أرجع (vim_strchr(curbuf->b_p_fo, x) != عدم);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    فراغ
internal_format(
    صحيح		textwidth,
    صحيح		second_indent,
    صحيح		flags,
    صحيح		format_only,
    صحيح		c) // character to be inserted (can be NUL)
{
    صحيح		cc;
    صحيح		skip_pos;
    صحيح		save_char = NUL;
    صحيح		haveto_redraw = FALSE;
    صحيح		fo_ins_blank = has_format_option(FO_INS_BLANK);
    صحيح		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    صحيح		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    صحيح		fo_white_par = has_format_option(FO_WHITE_PAR);
    صحيح		first_سطر = TRUE;
    colnr_T	leader_len;
    صحيح		no_leader = FALSE;
    صحيح		do_comments = (flags & INSCHAR_DO_COM);
    صحيح		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    صحيح		has_lbr = curwin->w_p_lbr;
    صحيح		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    إذا (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	إذا (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    طالما (!got_int)
    {
	صحيح	startcol;		// Cursor column at entry
	صحيح	wantcol;		// column at textwidth border
	صحيح	foundcol;		// column for start of spaces
	صحيح	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	صحيح	orig_col = 0;
	char_u	*saved_text = عدم;
	colnr_T	col;
	colnr_T	end_col;
	صحيح	wcc;			// counter for whitespace chars
	صحيح	did_do_comment = FALSE;
	صحيح	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	إذا (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    إذا (virtcol <= (colnr_T)textwidth)
		اكسر;
	}

	إذا (no_leader)
	    do_comments = FALSE;
	وإلا إذا (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	إذا (do_comments)
	{
	    char_u *سطر = ml_get_curline();

	    leader_len = get_leader_len(سطر, عدم, FALSE, TRUE);
	    إذا (leader_len == 0 && curbuf->b_p_cin)
	    {
		صحيح		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(سطر);
		إذا (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      سطر + comment_start, عدم, FALSE, TRUE);
		    إذا (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	وإلا
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	إذا (leader_len == 0)
	    no_leader = TRUE;
	إذا (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    اكسر;
	إذا ((startcol = curwin->w_cursor.col) == 0)
	    اكسر;

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
	طالما ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    إذا (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    وإلا
		cc = gchar_cursor();
	    إذا (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		طالما (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    إذا (wcc < 2)
			wcc++;
		}
		إذا (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    اكسر;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		إذا (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    استمر;

		// Don't break until after the comment leader
		إذا (curwin->w_cursor.col < leader_len)
		    اكسر;
		إذا (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    إذا (curwin->w_cursor.col == 0)
			اكسر;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    إذا (curwin->w_cursor.col <= leader_len)
			اكسر;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    إذا (WHITECHAR(cc))
			استمر;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		إذا (curwin->w_cursor.col <= (colnr_T)wantcol)
		    اكسر;
	    }
	    وإلا إذا ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		صحيح ncc;
		صحيح allow_break;

		// Break after or before a multi-byte character.
		إذا (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    إذا (curwin->w_cursor.col < leader_len)
			اكسر;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    إذا (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			إذا (curwin->w_cursor.col <= (colnr_T)wantcol)
			    اكسر;
		    }
		    curwin->w_cursor.col = col;
		}

		إذا (curwin->w_cursor.col == 0)
		    اكسر;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		إذا (WHITECHAR(cc))
		    استمر;		// break with space
		// Don't break until after the comment leader.
		إذا (curwin->w_cursor.col < leader_len)
		    اكسر;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		إذا (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		إذا (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    صحيح ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    إذا (allow_break)
			اكسر;
		    إذا (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			إذا (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    اكسر;
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

			إذا (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    اكسر;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    إذا (curwin->w_cursor.col == 0)
		اكسر;
	    dec_cursor();
	}

	إذا (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    اكسر;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	إذا (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	وإلا
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	طالما ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	إذا (startcol < 0)
	    startcol = 0;

	إذا (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    إذا (saved_text == عدم)
		اكسر;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    إذا (!fo_white_par)
		backspace_until_column(foundcol);
	}
	وإلا
	{
	    // put cursor after pos. to break line
	    إذا (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_سطر(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	إذا (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	إذا (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	إذا (first_سطر)
	{
	    إذا (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		إذا (second_indent < 0 && has_format_option(FO_Q_رقم))
		    second_indent =
				 get_رقم_indent(curwin->w_cursor.lnum - 1);
		إذا (second_indent >= 0)
		{
		    إذا (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    وإلا
			إذا (leader_len > 0 && second_indent - leader_len > 0)
		    {
			صحيح i;
			صحيح padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			لكل (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    وإلا
		    {
			(فراغ)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_سطر = FALSE;
	}

	إذا (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	وإلا
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    إذا (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	سطر_breakcheck();
    }

    إذا (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    إذا (!format_only && haveto_redraw)
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
    ساكن صحيح
fmt_check_par(
    linenr_T	lnum,
    صحيح		*leader_len,
    char_u	**leader_flags,
    صحيح		do_comments)
{
    char_u	*flags = عدم;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    إذا (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    وإلا
	*leader_len = 0;

    إذا (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	طالما (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    أرجع (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ساكن صحيح
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    إذا (*s == NUL)
	أرجع FALSE;
    l = ml_get_len(lnum) - 1;
    أرجع VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ساكن صحيح
same_leader(
    linenr_T lnum,
    صحيح	    leader1_len,
    char_u  *leader1_flags,
    صحيح	    leader2_len,
    char_u  *leader2_flags)
{
    صحيح	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    إذا (leader1_len == 0)
	أرجع (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    إذا (leader1_flags != عدم)
    {
	char_u	*p;

	لكل (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    إذا (*p == COM_FIRST)
		أرجع (leader2_len == 0);
	    إذا (*p == COM_END)
		أرجع FALSE;
	    إذا (*p == COM_START)
	    {
		صحيح سطر_len = ml_get_len(lnum);
		إذا (سطر_len <= leader1_len)
		    أرجع FALSE;
		إذا (leader2_flags == عدم || leader2_len == 0)
		    أرجع FALSE;
		لكل (p = leader2_flags; *p && *p != ':'; ++p)
		    إذا (*p == COM_وسط)
			أرجع TRUE;
		أرجع FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    إذا (line1 != عدم)
    {
	char_u  *line2;

	لكل (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	لكل (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    إذا (!VIM_ISWHITE(line2[idx2]))
	    {
		إذا (line1[idx1++] != line2[idx2])
		    اكسر;
	    }
	    وإلا
		طالما (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    أرجع (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ساكن صحيح
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    صحيح		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = عدم;	// flags for leader of current line
    صحيح		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    صحيح		do_comments;		// format comments

    إذا (lnum <= 1)
	أرجع TRUE;		// start of the file

    p = ml_get(lnum - 1);
    إذا (*p == NUL)
	أرجع TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    إذا (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	أرجع TRUE;		// after non-paragraph line

    إذا (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	أرجع TRUE;		// "lnum" is not a paragraph line

    إذا (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	أرجع TRUE;		// missing trailing space in previous line.

    إذا (has_format_option(FO_Q_رقم) && (get_رقم_indent(lnum) > 0))
	أرجع TRUE;		// numbered item starts in "lnum".

    إذا (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	أرجع TRUE;		// change of comment leader.

    أرجع FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    فراغ
auto_format(
    صحيح		trailblank,	// when TRUE also format with trailing blank
    صحيح		prev_سطر)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    صحيح		wasatend;

    إذا (!has_format_option(FO_AUTO))
	أرجع;

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
    إذا (*old != NUL && !trailblank && wasatend)
    {
	صحيح cc;

	dec_cursor();
	cc = gchar_cursor();
	إذا (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	إذا (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    أرجع;
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
    إذا (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_أدرج))
    {
	char_u *سطر = ml_get_curline();
	إذا (WHITECHAR(سطر[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    أرجع;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    إذا (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, عدم, FALSE, TRUE) == 0)
	أرجع;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    إذا (prev_سطر && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	إذا (u_save_cursor() == FAIL)
	    أرجع;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_أسطر((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_عدد)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_عدد;
	coladvance((colnr_T)MAXCOL);
    }
    وإلا
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    إذا (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	إذا (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    إذا (pnew == عدم)
		أرجع;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	وإلا
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
    فراغ
check_auto_format(
    صحيح		end_أدرج)	    // TRUE when ending Insert mode
{
    صحيح		c = ' ';
    صحيح		cc;

    إذا (!did_add_space)
	أرجع;

    cc = gchar_cursor();
    إذا (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    وإلا
    {
	إذا (!end_أدرج)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	إذا (c != NUL)
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
    صحيح
comp_textwidth(
    صحيح		ff)	// force formatting (for "gq" command)
{
    صحيح		textwidth;

    textwidth = curbuf->b_p_tw;
    إذا (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	إذا (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	إذا (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	إذا (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    إذا (textwidth < 0)
	textwidth = 0;
    إذا (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	إذا (textwidth > 79)
	    textwidth = 79;
    }
    أرجع textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    فراغ
op_format(
    oparg_T	*oap,
    صحيح		keep_cursor)		// keep cursor on same text char
{
    long	old_سطر_عدد = curbuf->b_ml.ml_سطر_عدد;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    إذا (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	أرجع;
    curwin->w_cursor = oap->start;

    إذا (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    إذا (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_أسطر(oap->سطر_عدد, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    إذا (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_عدد)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_سطر_عدد = curbuf->b_ml.ml_سطر_عدد - old_سطر_عدد;
    msgmore(old_سطر_عدد);

    إذا ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    إذا (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    إذا (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		إذا (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_سطر_عدد;
		وإلا
		    wp->w_old_visual_lnum += old_سطر_عدد;
	    }
	}
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    فراغ
op_formatexpr(oparg_T *oap)
{
    إذا (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    إذا (fex_format(oap->start.lnum, oap->سطر_عدد, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    صحيح
fex_format(
    linenr_T	lnum,
    long	عدد,
    صحيح		c)	// character to be inserted
{
    صحيح		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    صحيح		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_عدد, عدد);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    إذا (fex == عدم)
	أرجع 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    إذا (use_sandbox)
	++sandbox;
    r = (صحيح)eval_to_رقم(fex, TRUE);
    إذا (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, عدم, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    أرجع r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    فراغ
format_أسطر(
    linenr_T	سطر_عدد,
    صحيح		avoid_fex)		// don't use 'formatexpr'
{
    صحيح		max_len;
    صحيح		is_not_par;		// current line not part of parag.
    صحيح		next_is_not_par;	// next line not part of paragraph
    صحيح		is_end_par;		// at end of paragraph
    صحيح		prev_is_end_par = FALSE;// prev. line not part of parag.
    صحيح		next_is_start_par = FALSE;
    صحيح		leader_len = 0;		// leader len of current line
    صحيح		next_leader_len;	// leader len of next line
    char_u	*leader_flags = عدم;	// flags for leader of current line
    char_u	*next_leader_flags = عدم; // flags for leader of next line
    صحيح		do_comments;		// format comments
    صحيح		do_comments_list = 0;	// format comments with 'n' or '2'
    صحيح		advance = TRUE;
    صحيح		second_indent = -1;	// indent for second line (comment
					// aware)
    صحيح		do_second_indent;
    صحيح		do_رقم_indent;
    صحيح		do_trail_white;
    صحيح		first_par_سطر = TRUE;
    صحيح		smd_save;
    long	عدد;
    صحيح		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_سطر = curwin->w_cursor.lnum;
    صحيح		force_format = FALSE;
    صحيح		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_رقم_indent = has_format_option(FO_Q_رقم);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    إذا (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    وإلا
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    إذا (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    لكل (عدد = سطر_عدد; عدد != 0 && !got_int; --عدد)
    {
	// Advance to next paragraph.
	إذا (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	إذا (عدد == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_عدد)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = عدم;
	}
	وإلا
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    إذا (do_رقم_indent)
		next_is_start_par =
			   (get_رقم_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	إذا (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	إذا (is_not_par)
	{
	    إذا (سطر_عدد < 0)
		اكسر;
	}
	وإلا
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    إذا (first_par_سطر
		    && (do_second_indent || do_رقم_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_عدد)
	    {
		إذا (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    إذا (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    وإلا
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		وإلا إذا (do_رقم_indent)
		{
		    إذا (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_رقم_indent(curwin->w_cursor.lnum);
		    }
		    وإلا
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_رقم_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    إذا (curwin->w_cursor.lnum >= curbuf->b_ml.ml_سطر_عدد
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		إذا (next_leader_flags == عدم
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    إذا (is_end_par || force_format)
	    {
		إذا (need_set_indent)
		{
		    صحيح		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    إذا (curwin->w_cursor.lnum == first_سطر)
			indent = get_indent();
		    وإلا إذا (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    وإلا
		    {
			إذا (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_تعبير_indent() :
#endif
				 get_c_indent();
			}
			وإلا
			    indent = get_indent();
		    }
		    (فراغ)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		طالما (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_أدرج;	// for open_line()
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
		إذا (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    إذا (سطر_عدد < 0)
			اكسر;
		    first_par_سطر = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    إذا (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		إذا (سطر_عدد < 0 && u_save_cursor() == FAIL)
		    اكسر;
		إذا (next_leader_len > 0)
		{
		    (فراغ)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		وإلا إذا (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    صحيح indent = getwhitecols_curline();

		    إذا (indent > 0)
		    {
			(فراغ)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		إذا (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    اكسر;
		}
		first_par_سطر = FALSE;
		// If the line is getting long, format it next time
		إذا (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		وإلا
		    force_format = FALSE;
	    }
	}
	سطر_breakcheck();
    }
}
