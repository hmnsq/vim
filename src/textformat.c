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

#অন্তর্ভাব "vim.h"

স্থির পূর্ণসংখ্যা	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    পূর্ণসংখ্যা
has_format_option(পূর্ণসংখ্যা x)
{
    যদি (p_paste)
	ফেরত FALSE;
    ফেরত (vim_strchr(curbuf->b_p_fo, x) != ফাঁকা);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    শূন্য
internal_format(
    পূর্ণসংখ্যা		textwidth,
    পূর্ণসংখ্যা		second_indent,
    পূর্ণসংখ্যা		flags,
    পূর্ণসংখ্যা		format_only,
    পূর্ণসংখ্যা		c) // character to be inserted (can be NUL)
{
    পূর্ণসংখ্যা		cc;
    পূর্ণসংখ্যা		skip_pos;
    পূর্ণসংখ্যা		save_char = NUL;
    পূর্ণসংখ্যা		haveto_redraw = FALSE;
    পূর্ণসংখ্যা		fo_ins_blank = has_format_option(FO_INS_BLANK);
    পূর্ণসংখ্যা		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    পূর্ণসংখ্যা		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    পূর্ণসংখ্যা		fo_white_par = has_format_option(FO_WHITE_PAR);
    পূর্ণসংখ্যা		first_সারি = TRUE;
    colnr_T	leader_len;
    পূর্ণসংখ্যা		no_leader = FALSE;
    পূর্ণসংখ্যা		do_comments = (flags & INSCHAR_DO_COM);
    পূর্ণসংখ্যা		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    পূর্ণসংখ্যা		has_lbr = curwin->w_p_lbr;
    পূর্ণসংখ্যা		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    যদি (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	যদি (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    যখন (!got_int)
    {
	পূর্ণসংখ্যা	startcol;		// Cursor column at entry
	পূর্ণসংখ্যা	wantcol;		// column at textwidth border
	পূর্ণসংখ্যা	foundcol;		// column for start of spaces
	পূর্ণসংখ্যা	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	পূর্ণসংখ্যা	orig_col = 0;
	char_u	*saved_text = ফাঁকা;
	colnr_T	col;
	colnr_T	end_col;
	পূর্ণসংখ্যা	wcc;			// counter for whitespace chars
	পূর্ণসংখ্যা	did_do_comment = FALSE;
	পূর্ণসংখ্যা	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	যদি (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    যদি (virtcol <= (colnr_T)textwidth)
		বিরতি;
	}

	যদি (no_leader)
	    do_comments = FALSE;
	অন্যথা যদি (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	যদি (do_comments)
	{
	    char_u *সারি = ml_get_curline();

	    leader_len = get_leader_len(সারি, ফাঁকা, FALSE, TRUE);
	    যদি (leader_len == 0 && curbuf->b_p_cin)
	    {
		পূর্ণসংখ্যা		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(সারি);
		যদি (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      সারি + comment_start, ফাঁকা, FALSE, TRUE);
		    যদি (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	অন্যথা
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	যদি (leader_len == 0)
	    no_leader = TRUE;
	যদি (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    বিরতি;
	যদি ((startcol = curwin->w_cursor.col) == 0)
	    বিরতি;

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
	যখন ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    যদি (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    অন্যথা
		cc = gchar_cursor();
	    যদি (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		যখন (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    যদি (wcc < 2)
			wcc++;
		}
		যদি (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    বিরতি;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		যদি (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    চালু;

		// Don't break until after the comment leader
		যদি (curwin->w_cursor.col < leader_len)
		    বিরতি;
		যদি (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    যদি (curwin->w_cursor.col == 0)
			বিরতি;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    যদি (curwin->w_cursor.col <= leader_len)
			বিরতি;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    যদি (WHITECHAR(cc))
			চালু;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		যদি (curwin->w_cursor.col <= (colnr_T)wantcol)
		    বিরতি;
	    }
	    অন্যথা যদি ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		পূর্ণসংখ্যা ncc;
		পূর্ণসংখ্যা allow_break;

		// Break after or before a multi-byte character.
		যদি (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    যদি (curwin->w_cursor.col < leader_len)
			বিরতি;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    যদি (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			যদি (curwin->w_cursor.col <= (colnr_T)wantcol)
			    বিরতি;
		    }
		    curwin->w_cursor.col = col;
		}

		যদি (curwin->w_cursor.col == 0)
		    বিরতি;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		যদি (WHITECHAR(cc))
		    চালু;		// break with space
		// Don't break until after the comment leader.
		যদি (curwin->w_cursor.col < leader_len)
		    বিরতি;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		যদি (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		যদি (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    পূর্ণসংখ্যা ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    যদি (allow_break)
			বিরতি;
		    যদি (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			যদি (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    বিরতি;
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

			যদি (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    বিরতি;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    যদি (curwin->w_cursor.col == 0)
		বিরতি;
	    dec_cursor();
	}

	যদি (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    বিরতি;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	যদি (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	অন্যথা
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	যখন ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	যদি (startcol < 0)
	    startcol = 0;

	যদি (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    যদি (saved_text == ফাঁকা)
		বিরতি;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    যদি (!fo_white_par)
		backspace_until_column(foundcol);
	}
	অন্যথা
	{
	    // put cursor after pos. to break line
	    যদি (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_সারি(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	যদি (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	যদি (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	যদি (first_সারি)
	{
	    যদি (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		যদি (second_indent < 0 && has_format_option(FO_Q_সংখ্যা))
		    second_indent =
				 get_সংখ্যা_indent(curwin->w_cursor.lnum - 1);
		যদি (second_indent >= 0)
		{
		    যদি (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    অন্যথা
			যদি (leader_len > 0 && second_indent - leader_len > 0)
		    {
			পূর্ণসংখ্যা i;
			পূর্ণসংখ্যা padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			জন্য (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    অন্যথা
		    {
			(শূন্য)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_সারি = FALSE;
	}

	যদি (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	অন্যথা
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    যদি (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	সারি_breakcheck();
    }

    যদি (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    যদি (!format_only && haveto_redraw)
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
    স্থির পূর্ণসংখ্যা
fmt_check_par(
    linenr_T	lnum,
    পূর্ণসংখ্যা		*leader_len,
    char_u	**leader_flags,
    পূর্ণসংখ্যা		do_comments)
{
    char_u	*flags = ফাঁকা;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    যদি (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    অন্যথা
	*leader_len = 0;

    যদি (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	যখন (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    ফেরত (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    স্থির পূর্ণসংখ্যা
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    যদি (*s == NUL)
	ফেরত FALSE;
    l = ml_get_len(lnum) - 1;
    ফেরত VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    স্থির পূর্ণসংখ্যা
same_leader(
    linenr_T lnum,
    পূর্ণসংখ্যা	    leader1_len,
    char_u  *leader1_flags,
    পূর্ণসংখ্যা	    leader2_len,
    char_u  *leader2_flags)
{
    পূর্ণসংখ্যা	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    যদি (leader1_len == 0)
	ফেরত (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    যদি (leader1_flags != ফাঁকা)
    {
	char_u	*p;

	জন্য (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    যদি (*p == COM_FIRST)
		ফেরত (leader2_len == 0);
	    যদি (*p == COM_END)
		ফেরত FALSE;
	    যদি (*p == COM_START)
	    {
		পূর্ণসংখ্যা সারি_len = ml_get_len(lnum);
		যদি (সারি_len <= leader1_len)
		    ফেরত FALSE;
		যদি (leader2_flags == ফাঁকা || leader2_len == 0)
		    ফেরত FALSE;
		জন্য (p = leader2_flags; *p && *p != ':'; ++p)
		    যদি (*p == COM_মধ্য)
			ফেরত TRUE;
		ফেরত FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    যদি (line1 != ফাঁকা)
    {
	char_u  *line2;

	জন্য (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	জন্য (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    যদি (!VIM_ISWHITE(line2[idx2]))
	    {
		যদি (line1[idx1++] != line2[idx2])
		    বিরতি;
	    }
	    অন্যথা
		যখন (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    ফেরত (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    স্থির পূর্ণসংখ্যা
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    পূর্ণসংখ্যা		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ফাঁকা;	// flags for leader of current line
    পূর্ণসংখ্যা		পরবর্তী_leader_len;	// leader len of next line
    char_u	*পরবর্তী_leader_flags;	// flags for leader of next line
    পূর্ণসংখ্যা		do_comments;		// format comments

    যদি (lnum <= 1)
	ফেরত TRUE;		// start of the file

    p = ml_get(lnum - 1);
    যদি (*p == NUL)
	ফেরত TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    যদি (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	ফেরত TRUE;		// after non-paragraph line

    যদি (fmt_check_par(lnum, &পরবর্তী_leader_len, &পরবর্তী_leader_flags, do_comments))
	ফেরত TRUE;		// "lnum" is not a paragraph line

    যদি (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	ফেরত TRUE;		// missing trailing space in previous line.

    যদি (has_format_option(FO_Q_সংখ্যা) && (get_সংখ্যা_indent(lnum) > 0))
	ফেরত TRUE;		// numbered item starts in "lnum".

    যদি (!same_leader(lnum - 1, leader_len, leader_flags,
					  পরবর্তী_leader_len, পরবর্তী_leader_flags))
	ফেরত TRUE;		// change of comment leader.

    ফেরত FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    শূন্য
auto_format(
    পূর্ণসংখ্যা		trailblank,	// when TRUE also format with trailing blank
    পূর্ণসংখ্যা		পূর্ব_সারি)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    পূর্ণসংখ্যা		wasatend;

    যদি (!has_format_option(FO_AUTO))
	ফেরত;

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
    যদি (*old != NUL && !trailblank && wasatend)
    {
	পূর্ণসংখ্যা cc;

	dec_cursor();
	cc = gchar_cursor();
	যদি (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	যদি (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    ফেরত;
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
    যদি (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_ঢোকাও))
    {
	char_u *সারি = ml_get_curline();
	যদি (WHITECHAR(সারি[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    ফেরত;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    যদি (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ফাঁকা, FALSE, TRUE) == 0)
	ফেরত;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    যদি (পূর্ব_সারি && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	যদি (u_save_cursor() == FAIL)
	    ফেরত;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_সারিগুলি((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    যদি (curwin->w_cursor.lnum > curbuf->b_ml.ml_সারি_গুনতি)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	coladvance((colnr_T)MAXCOL);
    }
    অন্যথা
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    যদি (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	যদি (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    যদি (pnew == ফাঁকা)
		ফেরত;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	অন্যথা
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
    শূন্য
check_auto_format(
    পূর্ণসংখ্যা		end_ঢোকাও)	    // TRUE when ending Insert mode
{
    পূর্ণসংখ্যা		c = ' ';
    পূর্ণসংখ্যা		cc;

    যদি (!did_add_space)
	ফেরত;

    cc = gchar_cursor();
    যদি (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    অন্যথা
    {
	যদি (!end_ঢোকাও)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	যদি (c != NUL)
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
    পূর্ণসংখ্যা
comp_textwidth(
    পূর্ণসংখ্যা		ff)	// force formatting (for "gq" command)
{
    পূর্ণসংখ্যা		textwidth;

    textwidth = curbuf->b_p_tw;
    যদি (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	যদি (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	যদি (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	যদি (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    যদি (textwidth < 0)
	textwidth = 0;
    যদি (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	যদি (textwidth > 79)
	    textwidth = 79;
    }
    ফেরত textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    শূন্য
op_format(
    oparg_T	*oap,
    পূর্ণসংখ্যা		keep_cursor)		// keep cursor on same text char
{
    long	old_সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    যদি (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ফেরত;
    curwin->w_cursor = oap->start;

    যদি (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    যদি (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_সারিগুলি(oap->সারি_গুনতি, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    যদি (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি - old_সারি_গুনতি;
    msgmore(old_সারি_গুনতি);

    যদি ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    যদি (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    যদি (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		যদি (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_সারি_গুনতি;
		অন্যথা
		    wp->w_old_visual_lnum += old_সারি_গুনতি;
	    }
	}
    }
}

#যদি defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    শূন্য
op_formatexpr(oparg_T *oap)
{
    যদি (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    যদি (fex_format(oap->start.lnum, oap->সারি_গুনতি, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    পূর্ণসংখ্যা
fex_format(
    linenr_T	lnum,
    long	গুনতি,
    পূর্ণসংখ্যা		c)	// character to be inserted
{
    পূর্ণসংখ্যা		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    পূর্ণসংখ্যা		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_গুনতি, গুনতি);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    যদি (fex == ফাঁকা)
	ফেরত 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    যদি (use_sandbox)
	++sandbox;
    r = (পূর্ণসংখ্যা)eval_to_সংখ্যা(fex, TRUE);
    যদি (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, ফাঁকা, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    ফেরত r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    শূন্য
format_সারিগুলি(
    linenr_T	সারি_গুনতি,
    পূর্ণসংখ্যা		avoid_fex)		// don't use 'formatexpr'
{
    পূর্ণসংখ্যা		max_len;
    পূর্ণসংখ্যা		is_not_par;		// current line not part of parag.
    পূর্ণসংখ্যা		পরবর্তী_is_not_par;	// next line not part of paragraph
    পূর্ণসংখ্যা		is_end_par;		// at end of paragraph
    পূর্ণসংখ্যা		পূর্ব_is_end_par = FALSE;// prev. line not part of parag.
    পূর্ণসংখ্যা		পরবর্তী_is_start_par = FALSE;
    পূর্ণসংখ্যা		leader_len = 0;		// leader len of current line
    পূর্ণসংখ্যা		পরবর্তী_leader_len;	// leader len of next line
    char_u	*leader_flags = ফাঁকা;	// flags for leader of current line
    char_u	*পরবর্তী_leader_flags = ফাঁকা; // flags for leader of next line
    পূর্ণসংখ্যা		do_comments;		// format comments
    পূর্ণসংখ্যা		do_comments_list = 0;	// format comments with 'n' or '2'
    পূর্ণসংখ্যা		advance = TRUE;
    পূর্ণসংখ্যা		second_indent = -1;	// indent for second line (comment
					// aware)
    পূর্ণসংখ্যা		do_second_indent;
    পূর্ণসংখ্যা		do_সংখ্যা_indent;
    পূর্ণসংখ্যা		do_trail_white;
    পূর্ণসংখ্যা		first_par_সারি = TRUE;
    পূর্ণসংখ্যা		smd_save;
    long	গুনতি;
    পূর্ণসংখ্যা		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_সারি = curwin->w_cursor.lnum;
    পূর্ণসংখ্যা		force_format = FALSE;
    পূর্ণসংখ্যা		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_সংখ্যা_indent = has_format_option(FO_Q_সংখ্যা);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    যদি (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    অন্যথা
	is_not_par = TRUE;
    পরবর্তী_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &পরবর্তী_leader_len, &পরবর্তী_leader_flags, do_comments);
    is_end_par = (is_not_par || পরবর্তী_is_not_par);
    যদি (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    জন্য (গুনতি = সারি_গুনতি; গুনতি != 0 && !got_int; --গুনতি)
    {
	// Advance to next paragraph.
	যদি (advance)
	{
	    curwin->w_cursor.lnum++;
	    পূর্ব_is_end_par = is_end_par;
	    is_not_par = পরবর্তী_is_not_par;
	    leader_len = পরবর্তী_leader_len;
	    leader_flags = পরবর্তী_leader_flags;
	}

	// The last line to be formatted.
	যদি (গুনতি == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_সারি_গুনতি)
	{
	    পরবর্তী_is_not_par = TRUE;
	    পরবর্তী_leader_len = 0;
	    পরবর্তী_leader_flags = ফাঁকা;
	}
	অন্যথা
	{
	    পরবর্তী_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &পরবর্তী_leader_len, &পরবর্তী_leader_flags, do_comments);
	    যদি (do_সংখ্যা_indent)
		পরবর্তী_is_start_par =
			   (get_সংখ্যা_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || পরবর্তী_is_not_par || পরবর্তী_is_start_par);
	যদি (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	যদি (is_not_par)
	{
	    যদি (সারি_গুনতি < 0)
		বিরতি;
	}
	অন্যথা
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    যদি (first_par_সারি
		    && (do_second_indent || do_সংখ্যা_indent)
		    && পূর্ব_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি)
	    {
		যদি (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    যদি (leader_len == 0 && পরবর্তী_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    অন্যথা
		    {
			second_indent = পরবর্তী_leader_len;
			do_comments_list = 1;
		    }
		}
		অন্যথা যদি (do_সংখ্যা_indent)
		{
		    যদি (leader_len == 0 && পরবর্তী_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_সংখ্যা_indent(curwin->w_cursor.lnum);
		    }
		    অন্যথা
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_সংখ্যা_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    যদি (curwin->w_cursor.lnum >= curbuf->b_ml.ml_সারি_গুনতি
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   পরবর্তী_leader_len, পরবর্তী_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		যদি (পরবর্তী_leader_flags == ফাঁকা
			|| STRNCMP(পরবর্তী_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    যদি (is_end_par || force_format)
	    {
		যদি (need_set_indent)
		{
		    পূর্ণসংখ্যা		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    যদি (curwin->w_cursor.lnum == first_সারি)
			indent = get_indent();
		    অন্যথা যদি (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    অন্যথা
		    {
			যদি (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_রাশিমালা_indent() :
#endif
				 get_c_indent();
			}
			অন্যথা
			    indent = get_indent();
		    }
		    (শূন্য)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		যখন (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_ঢোকাও;	// for open_line()
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
		যদি (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    যদি (সারি_গুনতি < 0)
			বিরতি;
		    first_par_সারি = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    যদি (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		যদি (সারি_গুনতি < 0 && u_save_cursor() == FAIL)
		    বিরতি;
		যদি (পরবর্তী_leader_len > 0)
		{
		    (শূন্য)del_bytes((long)পরবর্তী_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-পরবর্তী_leader_len, 0);
		}
		অন্যথা যদি (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    পূর্ণসংখ্যা indent = getwhitecols_curline();

		    যদি (indent > 0)
		    {
			(শূন্য)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		যদি (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    বিরতি;
		}
		first_par_সারি = FALSE;
		// If the line is getting long, format it next time
		যদি (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		অন্যথা
		    force_format = FALSE;
	    }
	}
	সারি_breakcheck();
    }
}
