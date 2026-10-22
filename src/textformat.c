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

#अन्तर्भाव "vim.h"

स्थिर पूर्णाङ्क	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    पूर्णाङ्क
has_format_option(पूर्णाङ्क x)
{
    यदि (p_paste)
	प्रत्यागम FALSE;
    प्रत्यागम (vim_strchr(curbuf->b_p_fo, x) != रिक्त);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    शून्य
internal_format(
    पूर्णाङ्क		textwidth,
    पूर्णाङ्क		second_indent,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		format_only,
    पूर्णाङ्क		c) // character to be inserted (can be NUL)
{
    पूर्णाङ्क		cc;
    पूर्णाङ्क		skip_pos;
    पूर्णाङ्क		save_char = NUL;
    पूर्णाङ्क		haveto_redraw = FALSE;
    पूर्णाङ्क		fo_ins_blank = has_format_option(FO_INS_BLANK);
    पूर्णाङ्क		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    पूर्णाङ्क		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    पूर्णाङ्क		fo_white_par = has_format_option(FO_WHITE_PAR);
    पूर्णाङ्क		first_पंक्ति = TRUE;
    colnr_T	leader_len;
    पूर्णाङ्क		no_leader = FALSE;
    पूर्णाङ्क		do_comments = (flags & INSCHAR_DO_COM);
    पूर्णाङ्क		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    पूर्णाङ्क		has_lbr = curwin->w_p_lbr;
    पूर्णाङ्क		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    यदि (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	यदि (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    यावत् (!got_int)
    {
	पूर्णाङ्क	startcol;		// Cursor column at entry
	पूर्णाङ्क	wantcol;		// column at textwidth border
	पूर्णाङ्क	foundcol;		// column for start of spaces
	पूर्णाङ्क	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	पूर्णाङ्क	orig_col = 0;
	char_u	*saved_text = रिक्त;
	colnr_T	col;
	colnr_T	end_col;
	पूर्णाङ्क	wcc;			// counter for whitespace chars
	पूर्णाङ्क	did_do_comment = FALSE;
	पूर्णाङ्क	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	यदि (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    यदि (virtcol <= (colnr_T)textwidth)
		भङ्ग;
	}

	यदि (no_leader)
	    do_comments = FALSE;
	अन्यथा यदि (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	यदि (do_comments)
	{
	    char_u *पंक्ति = ml_get_curline();

	    leader_len = get_leader_len(पंक्ति, रिक्त, FALSE, TRUE);
	    यदि (leader_len == 0 && curbuf->b_p_cin)
	    {
		पूर्णाङ्क		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(पंक्ति);
		यदि (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      पंक्ति + comment_start, रिक्त, FALSE, TRUE);
		    यदि (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	अन्यथा
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	यदि (leader_len == 0)
	    no_leader = TRUE;
	यदि (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    भङ्ग;
	यदि ((startcol = curwin->w_cursor.col) == 0)
	    भङ्ग;

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
	यावत् ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    यदि (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    अन्यथा
		cc = gchar_cursor();
	    यदि (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		यावत् (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    यदि (wcc < 2)
			wcc++;
		}
		यदि (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    भङ्ग;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		यदि (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    अनुवर्त;

		// Don't break until after the comment leader
		यदि (curwin->w_cursor.col < leader_len)
		    भङ्ग;
		यदि (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    यदि (curwin->w_cursor.col == 0)
			भङ्ग;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    यदि (curwin->w_cursor.col <= leader_len)
			भङ्ग;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    यदि (WHITECHAR(cc))
			अनुवर्त;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		यदि (curwin->w_cursor.col <= (colnr_T)wantcol)
		    भङ्ग;
	    }
	    अन्यथा यदि ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		पूर्णाङ्क ncc;
		पूर्णाङ्क allow_break;

		// Break after or before a multi-byte character.
		यदि (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    यदि (curwin->w_cursor.col < leader_len)
			भङ्ग;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    यदि (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			यदि (curwin->w_cursor.col <= (colnr_T)wantcol)
			    भङ्ग;
		    }
		    curwin->w_cursor.col = col;
		}

		यदि (curwin->w_cursor.col == 0)
		    भङ्ग;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		यदि (WHITECHAR(cc))
		    अनुवर्त;		// break with space
		// Don't break until after the comment leader.
		यदि (curwin->w_cursor.col < leader_len)
		    भङ्ग;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		यदि (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		यदि (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    पूर्णाङ्क ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    यदि (allow_break)
			भङ्ग;
		    यदि (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			यदि (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    भङ्ग;
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

			यदि (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    भङ्ग;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    यदि (curwin->w_cursor.col == 0)
		भङ्ग;
	    dec_cursor();
	}

	यदि (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    भङ्ग;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	यदि (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	अन्यथा
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	यावत् ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	यदि (startcol < 0)
	    startcol = 0;

	यदि (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    यदि (saved_text == रिक्त)
		भङ्ग;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    यदि (!fo_white_par)
		backspace_until_column(foundcol);
	}
	अन्यथा
	{
	    // put cursor after pos. to break line
	    यदि (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_पंक्ति(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	यदि (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	यदि (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	यदि (first_पंक्ति)
	{
	    यदि (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		यदि (second_indent < 0 && has_format_option(FO_Q_संख्या))
		    second_indent =
				 get_संख्या_indent(curwin->w_cursor.lnum - 1);
		यदि (second_indent >= 0)
		{
		    यदि (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    अन्यथा
			यदि (leader_len > 0 && second_indent - leader_len > 0)
		    {
			पूर्णाङ्क i;
			पूर्णाङ्क padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			प्रति (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    अन्यथा
		    {
			(शून्य)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_पंक्ति = FALSE;
	}

	यदि (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	अन्यथा
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    यदि (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	पंक्ति_breakcheck();
    }

    यदि (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    यदि (!format_only && haveto_redraw)
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
    स्थिर पूर्णाङ्क
fmt_check_par(
    linenr_T	lnum,
    पूर्णाङ्क		*leader_len,
    char_u	**leader_flags,
    पूर्णाङ्क		do_comments)
{
    char_u	*flags = रिक्त;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    यदि (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    अन्यथा
	*leader_len = 0;

    यदि (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	यावत् (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    प्रत्यागम (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    स्थिर पूर्णाङ्क
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    माप_प्रकार	l;

    यदि (*s == NUL)
	प्रत्यागम FALSE;
    l = ml_get_len(lnum) - 1;
    प्रत्यागम VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    स्थिर पूर्णाङ्क
same_leader(
    linenr_T lnum,
    पूर्णाङ्क	    leader1_len,
    char_u  *leader1_flags,
    पूर्णाङ्क	    leader2_len,
    char_u  *leader2_flags)
{
    पूर्णाङ्क	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    यदि (leader1_len == 0)
	प्रत्यागम (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    यदि (leader1_flags != रिक्त)
    {
	char_u	*p;

	प्रति (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    यदि (*p == COM_FIRST)
		प्रत्यागम (leader2_len == 0);
	    यदि (*p == COM_END)
		प्रत्यागम FALSE;
	    यदि (*p == COM_START)
	    {
		पूर्णाङ्क पंक्ति_len = ml_get_len(lnum);
		यदि (पंक्ति_len <= leader1_len)
		    प्रत्यागम FALSE;
		यदि (leader2_flags == रिक्त || leader2_len == 0)
		    प्रत्यागम FALSE;
		प्रति (p = leader2_flags; *p && *p != ':'; ++p)
		    यदि (*p == COM_मध्य)
			प्रत्यागम TRUE;
		प्रत्यागम FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    यदि (line1 != रिक्त)
    {
	char_u  *line2;

	प्रति (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	प्रति (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    यदि (!VIM_ISWHITE(line2[idx2]))
	    {
		यदि (line1[idx1++] != line2[idx2])
		    भङ्ग;
	    }
	    अन्यथा
		यावत् (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    प्रत्यागम (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    स्थिर पूर्णाङ्क
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    पूर्णाङ्क		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = रिक्त;	// flags for leader of current line
    पूर्णाङ्क		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    पूर्णाङ्क		do_comments;		// format comments

    यदि (lnum <= 1)
	प्रत्यागम TRUE;		// start of the file

    p = ml_get(lnum - 1);
    यदि (*p == NUL)
	प्रत्यागम TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    यदि (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	प्रत्यागम TRUE;		// after non-paragraph line

    यदि (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	प्रत्यागम TRUE;		// "lnum" is not a paragraph line

    यदि (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	प्रत्यागम TRUE;		// missing trailing space in previous line.

    यदि (has_format_option(FO_Q_संख्या) && (get_संख्या_indent(lnum) > 0))
	प्रत्यागम TRUE;		// numbered item starts in "lnum".

    यदि (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	प्रत्यागम TRUE;		// change of comment leader.

    प्रत्यागम FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    शून्य
auto_format(
    पूर्णाङ्क		trailblank,	// when TRUE also format with trailing blank
    पूर्णाङ्क		prev_पंक्ति)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    पूर्णाङ्क		wasatend;

    यदि (!has_format_option(FO_AUTO))
	प्रत्यागम;

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
    यदि (*old != NUL && !trailblank && wasatend)
    {
	पूर्णाङ्क cc;

	dec_cursor();
	cc = gchar_cursor();
	यदि (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	यदि (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    प्रत्यागम;
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
    यदि (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_जोड़ो))
    {
	char_u *पंक्ति = ml_get_curline();
	यदि (WHITECHAR(पंक्ति[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    प्रत्यागम;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    यदि (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, रिक्त, FALSE, TRUE) == 0)
	प्रत्यागम;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    यदि (prev_पंक्ति && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	यदि (u_save_cursor() == FAIL)
	    प्रत्यागम;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_पंक्तियाँ((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    यदि (curwin->w_cursor.lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_पंक्ति_गिनती;
	coladvance((colnr_T)MAXCOL);
    }
    अन्यथा
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    यदि (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	यदि (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    यदि (pnew == रिक्त)
		प्रत्यागम;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	अन्यथा
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
    शून्य
check_auto_format(
    पूर्णाङ्क		end_जोड़ो)	    // TRUE when ending Insert mode
{
    पूर्णाङ्क		c = ' ';
    पूर्णाङ्क		cc;

    यदि (!did_add_space)
	प्रत्यागम;

    cc = gchar_cursor();
    यदि (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    अन्यथा
    {
	यदि (!end_जोड़ो)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	यदि (c != NUL)
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
    पूर्णाङ्क
comp_textwidth(
    पूर्णाङ्क		ff)	// force formatting (for "gq" command)
{
    पूर्णाङ्क		textwidth;

    textwidth = curbuf->b_p_tw;
    यदि (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	यदि (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	यदि (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	यदि (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    यदि (textwidth < 0)
	textwidth = 0;
    यदि (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	यदि (textwidth > 79)
	    textwidth = 79;
    }
    प्रत्यागम textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    शून्य
op_format(
    oparg_T	*oap,
    पूर्णाङ्क		keep_cursor)		// keep cursor on same text char
{
    दीर्घ	old_पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    यदि (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	प्रत्यागम;
    curwin->w_cursor = oap->start;

    यदि (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    यदि (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_पंक्तियाँ(oap->पंक्ति_गिनती, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    यदि (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_पंक्ति_गिनती)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती - old_पंक्ति_गिनती;
    msgmore(old_पंक्ति_गिनती);

    यदि ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    यदि (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    यदि (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    यदि (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		यदि (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_पंक्ति_गिनती;
		अन्यथा
		    wp->w_old_visual_lnum += old_पंक्ति_गिनती;
	    }
	}
    }
}

#यदि defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    शून्य
op_formatexpr(oparg_T *oap)
{
    यदि (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    यदि (fex_format(oap->start.lnum, oap->पंक्ति_गिनती, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    पूर्णाङ्क
fex_format(
    linenr_T	lnum,
    दीर्घ	गिनती,
    पूर्णाङ्क		c)	// character to be inserted
{
    पूर्णाङ्क		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    पूर्णाङ्क		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_गिनती, गिनती);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    यदि (fex == रिक्त)
	प्रत्यागम 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    यदि (use_sandbox)
	++sandbox;
    r = (पूर्णाङ्क)eval_to_संख्या(fex, TRUE);
    यदि (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, रिक्त, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    प्रत्यागम r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    शून्य
format_पंक्तियाँ(
    linenr_T	पंक्ति_गिनती,
    पूर्णाङ्क		avoid_fex)		// don't use 'formatexpr'
{
    पूर्णाङ्क		max_len;
    पूर्णाङ्क		is_not_par;		// current line not part of parag.
    पूर्णाङ्क		next_is_not_par;	// next line not part of paragraph
    पूर्णाङ्क		is_end_par;		// at end of paragraph
    पूर्णाङ्क		prev_is_end_par = FALSE;// prev. line not part of parag.
    पूर्णाङ्क		next_is_start_par = FALSE;
    पूर्णाङ्क		leader_len = 0;		// leader len of current line
    पूर्णाङ्क		next_leader_len;	// leader len of next line
    char_u	*leader_flags = रिक्त;	// flags for leader of current line
    char_u	*next_leader_flags = रिक्त; // flags for leader of next line
    पूर्णाङ्क		do_comments;		// format comments
    पूर्णाङ्क		do_comments_list = 0;	// format comments with 'n' or '2'
    पूर्णाङ्क		advance = TRUE;
    पूर्णाङ्क		second_indent = -1;	// indent for second line (comment
					// aware)
    पूर्णाङ्क		do_second_indent;
    पूर्णाङ्क		do_संख्या_indent;
    पूर्णाङ्क		do_trail_white;
    पूर्णाङ्क		first_par_पंक्ति = TRUE;
    पूर्णाङ्क		smd_save;
    दीर्घ	गिनती;
    पूर्णाङ्क		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_पंक्ति = curwin->w_cursor.lnum;
    पूर्णाङ्क		force_format = FALSE;
    पूर्णाङ्क		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_संख्या_indent = has_format_option(FO_Q_संख्या);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    यदि (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    अन्यथा
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    यदि (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    प्रति (गिनती = पंक्ति_गिनती; गिनती != 0 && !got_int; --गिनती)
    {
	// Advance to next paragraph.
	यदि (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	यदि (गिनती == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = रिक्त;
	}
	अन्यथा
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    यदि (do_संख्या_indent)
		next_is_start_par =
			   (get_संख्या_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	यदि (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	यदि (is_not_par)
	{
	    यदि (पंक्ति_गिनती < 0)
		भङ्ग;
	}
	अन्यथा
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    यदि (first_par_पंक्ति
		    && (do_second_indent || do_संख्या_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_पंक्ति_गिनती)
	    {
		यदि (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    यदि (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    अन्यथा
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		अन्यथा यदि (do_संख्या_indent)
		{
		    यदि (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_संख्या_indent(curwin->w_cursor.lnum);
		    }
		    अन्यथा
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_संख्या_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    यदि (curwin->w_cursor.lnum >= curbuf->b_ml.ml_पंक्ति_गिनती
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		यदि (next_leader_flags == रिक्त
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    यदि (is_end_par || force_format)
	    {
		यदि (need_set_indent)
		{
		    पूर्णाङ्क		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    यदि (curwin->w_cursor.lnum == first_पंक्ति)
			indent = get_indent();
		    अन्यथा यदि (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    अन्यथा
		    {
			यदि (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_व्यंजक_indent() :
#endif
				 get_c_indent();
			}
			अन्यथा
			    indent = get_indent();
		    }
		    (शून्य)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		यावत् (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_जोड़ो;	// for open_line()
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
		यदि (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    यदि (पंक्ति_गिनती < 0)
			भङ्ग;
		    first_par_पंक्ति = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    यदि (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		यदि (पंक्ति_गिनती < 0 && u_save_cursor() == FAIL)
		    भङ्ग;
		यदि (next_leader_len > 0)
		{
		    (शून्य)del_bytes((दीर्घ)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (दीर्घ)-next_leader_len, 0);
		}
		अन्यथा यदि (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    पूर्णाङ्क indent = getwhitecols_curline();

		    यदि (indent > 0)
		    {
			(शून्य)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (दीर्घ)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		यदि (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    भङ्ग;
		}
		first_par_पंक्ति = FALSE;
		// If the line is getting long, format it next time
		यदि (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		अन्यथा
		    force_format = FALSE;
	    }
	}
	पंक्ति_breakcheck();
    }
}
