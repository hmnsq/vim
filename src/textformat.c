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

#ಅನ್ತರ್ಭಾವ "vim.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    ಪೂರ್ಣಾಙ್ಕ
has_format_option(ಪೂರ್ಣಾಙ್ಕ x)
{
    ಯದಿ (p_paste)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ (vim_strchr(curbuf->b_p_fo, x) != ರಿಕ್ತ);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    ಶೂನ್ಯ
internal_format(
    ಪೂರ್ಣಾಙ್ಕ		textwidth,
    ಪೂರ್ಣಾಙ್ಕ		second_indent,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		format_only,
    ಪೂರ್ಣಾಙ್ಕ		c) // character to be inserted (can be NUL)
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		skip_pos;
    ಪೂರ್ಣಾಙ್ಕ		save_char = NUL;
    ಪೂರ್ಣಾಙ್ಕ		haveto_redraw = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		fo_ins_blank = has_format_option(FO_INS_BLANK);
    ಪೂರ್ಣಾಙ್ಕ		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    ಪೂರ್ಣಾಙ್ಕ		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    ಪೂರ್ಣಾಙ್ಕ		fo_white_par = has_format_option(FO_WHITE_PAR);
    ಪೂರ್ಣಾಙ್ಕ		first_ಸಾಲು = TRUE;
    colnr_T	leader_len;
    ಪೂರ್ಣಾಙ್ಕ		no_leader = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		do_comments = (flags & INSCHAR_DO_COM);
    ಪೂರ್ಣಾಙ್ಕ		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    ಪೂರ್ಣಾಙ್ಕ		has_lbr = curwin->w_p_lbr;
    ಪೂರ್ಣಾಙ್ಕ		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ಯದಿ (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ಯದಿ (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ಯಾವತ್ (!got_int)
    {
	ಪೂರ್ಣಾಙ್ಕ	startcol;		// Cursor column at entry
	ಪೂರ್ಣಾಙ್ಕ	wantcol;		// column at textwidth border
	ಪೂರ್ಣಾಙ್ಕ	foundcol;		// column for start of spaces
	ಪೂರ್ಣಾಙ್ಕ	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	ಪೂರ್ಣಾಙ್ಕ	orig_col = 0;
	char_u	*saved_text = ರಿಕ್ತ;
	colnr_T	col;
	colnr_T	end_col;
	ಪೂರ್ಣಾಙ್ಕ	wcc;			// counter for whitespace chars
	ಪೂರ್ಣಾಙ್ಕ	did_do_comment = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ಯದಿ (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ಯದಿ (virtcol <= (colnr_T)textwidth)
		ಭಙ್ಗ;
	}

	ಯದಿ (no_leader)
	    do_comments = FALSE;
	ಅನ್ಯಥಾ ಯದಿ (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ಯದಿ (do_comments)
	{
	    char_u *ಸಾಲು = ml_get_curline();

	    leader_len = get_leader_len(ಸಾಲು, ರಿಕ್ತ, FALSE, TRUE);
	    ಯದಿ (leader_len == 0 && curbuf->b_p_cin)
	    {
		ಪೂರ್ಣಾಙ್ಕ		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(ಸಾಲು);
		ಯದಿ (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      ಸಾಲು + comment_start, ರಿಕ್ತ, FALSE, TRUE);
		    ಯದಿ (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ಯದಿ (leader_len == 0)
	    no_leader = TRUE;
	ಯದಿ (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    ಭಙ್ಗ;
	ಯದಿ ((startcol = curwin->w_cursor.col) == 0)
	    ಭಙ್ಗ;

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
	ಯಾವತ್ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    ಯದಿ (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    ಅನ್ಯಥಾ
		cc = gchar_cursor();
	    ಯದಿ (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		ಯಾವತ್ (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ಯದಿ (wcc < 2)
			wcc++;
		}
		ಯದಿ (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    ಭಙ್ಗ;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ಯದಿ (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    ಅನುವರ್ತ;

		// Don't break until after the comment leader
		ಯದಿ (curwin->w_cursor.col < leader_len)
		    ಭಙ್ಗ;
		ಯದಿ (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ಯದಿ (curwin->w_cursor.col == 0)
			ಭಙ್ಗ;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ಯದಿ (curwin->w_cursor.col <= leader_len)
			ಭಙ್ಗ;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    ಯದಿ (WHITECHAR(cc))
			ಅನುವರ್ತ;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		ಯದಿ (curwin->w_cursor.col <= (colnr_T)wantcol)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		ಪೂರ್ಣಾಙ್ಕ ncc;
		ಪೂರ್ಣಾಙ್ಕ allow_break;

		// Break after or before a multi-byte character.
		ಯದಿ (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    ಯದಿ (curwin->w_cursor.col < leader_len)
			ಭಙ್ಗ;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ಯದಿ (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			ಯದಿ (curwin->w_cursor.col <= (colnr_T)wantcol)
			    ಭಙ್ಗ;
		    }
		    curwin->w_cursor.col = col;
		}

		ಯದಿ (curwin->w_cursor.col == 0)
		    ಭಙ್ಗ;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		ಯದಿ (WHITECHAR(cc))
		    ಅನುವರ್ತ;		// break with space
		// Don't break until after the comment leader.
		ಯದಿ (curwin->w_cursor.col < leader_len)
		    ಭಙ್ಗ;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ಯದಿ (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		ಯದಿ (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    ಪೂರ್ಣಾಙ್ಕ ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ಯದಿ (allow_break)
			ಭಙ್ಗ;
		    ಯದಿ (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ಯದಿ (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    ಭಙ್ಗ;
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

			ಯದಿ (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    ಭಙ್ಗ;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    ಯದಿ (curwin->w_cursor.col == 0)
		ಭಙ್ಗ;
	    dec_cursor();
	}

	ಯದಿ (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    ಭಙ್ಗ;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ಯದಿ (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	ಅನ್ಯಥಾ
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	ಯಾವತ್ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	ಯದಿ (startcol < 0)
	    startcol = 0;

	ಯದಿ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    ಯದಿ (saved_text == ರಿಕ್ತ)
		ಭಙ್ಗ;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ಯದಿ (!fo_white_par)
		backspace_until_column(foundcol);
	}
	ಅನ್ಯಥಾ
	{
	    // put cursor after pos. to break line
	    ಯದಿ (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_ಸಾಲು(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	ಯದಿ (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ಯದಿ (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	ಯದಿ (first_ಸಾಲು)
	{
	    ಯದಿ (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ಯದಿ (second_indent < 0 && has_format_option(FO_Q_ಸಂಖ್ಯೆ))
		    second_indent =
				 get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum - 1);
		ಯದಿ (second_indent >= 0)
		{
		    ಯದಿ (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    ಅನ್ಯಥಾ
			ಯದಿ (leader_len > 0 && second_indent - leader_len > 0)
		    {
			ಪೂರ್ಣಾಙ್ಕ i;
			ಪೂರ್ಣಾಙ್ಕ padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			ಪ್ರತಿ (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    ಅನ್ಯಥಾ
		    {
			(ಶೂನ್ಯ)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_ಸಾಲು = FALSE;
	}

	ಯದಿ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	ಅನ್ಯಥಾ
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    ಯದಿ (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	ಸಾಲು_breakcheck();
    }

    ಯದಿ (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    ಯದಿ (!format_only && haveto_redraw)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fmt_check_par(
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		*leader_len,
    char_u	**leader_flags,
    ಪೂರ್ಣಾಙ್ಕ		do_comments)
{
    char_u	*flags = ರಿಕ್ತ;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ಯದಿ (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    ಅನ್ಯಥಾ
	*leader_len = 0;

    ಯದಿ (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ಯಾವತ್ (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    ಪ್ರತ್ಯಾಗಮ (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ಯದಿ (*s == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    l = ml_get_len(lnum) - 1;
    ಪ್ರತ್ಯಾಗಮ VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
same_leader(
    linenr_T lnum,
    ಪೂರ್ಣಾಙ್ಕ	    leader1_len,
    char_u  *leader1_flags,
    ಪೂರ್ಣಾಙ್ಕ	    leader2_len,
    char_u  *leader2_flags)
{
    ಪೂರ್ಣಾಙ್ಕ	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ಯದಿ (leader1_len == 0)
	ಪ್ರತ್ಯಾಗಮ (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ಯದಿ (leader1_flags != ರಿಕ್ತ)
    {
	char_u	*p;

	ಪ್ರತಿ (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ಯದಿ (*p == COM_FIRST)
		ಪ್ರತ್ಯಾಗಮ (leader2_len == 0);
	    ಯದಿ (*p == COM_END)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (*p == COM_START)
	    {
		ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_len = ml_get_len(lnum);
		ಯದಿ (ಸಾಲು_len <= leader1_len)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		ಯದಿ (leader2_flags == ರಿಕ್ತ || leader2_len == 0)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		ಪ್ರತಿ (p = leader2_flags; *p && *p != ':'; ++p)
		    ಯದಿ (*p == COM_ಮಧ್ಯ)
			ಪ್ರತ್ಯಾಗಮ TRUE;
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ಯದಿ (line1 != ರಿಕ್ತ)
    {
	char_u  *line2;

	ಪ್ರತಿ (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	ಪ್ರತಿ (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ಯದಿ (!VIM_ISWHITE(line2[idx2]))
	    {
		ಯದಿ (line1[idx1++] != line2[idx2])
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
		ಯಾವತ್ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    ಪ್ರತ್ಯಾಗಮ (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ರಿಕ್ತ;	// flags for leader of current line
    ಪೂರ್ಣಾಙ್ಕ		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    ಪೂರ್ಣಾಙ್ಕ		do_comments;		// format comments

    ಯದಿ (lnum <= 1)
	ಪ್ರತ್ಯಾಗಮ TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ಯದಿ (*p == NUL)
	ಪ್ರತ್ಯಾಗಮ TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ಯದಿ (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	ಪ್ರತ್ಯಾಗಮ TRUE;		// after non-paragraph line

    ಯದಿ (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	ಪ್ರತ್ಯಾಗಮ TRUE;		// "lnum" is not a paragraph line

    ಯದಿ (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	ಪ್ರತ್ಯಾಗಮ TRUE;		// missing trailing space in previous line.

    ಯದಿ (has_format_option(FO_Q_ಸಂಖ್ಯೆ) && (get_ಸಂಖ್ಯೆ_indent(lnum) > 0))
	ಪ್ರತ್ಯಾಗಮ TRUE;		// numbered item starts in "lnum".

    ಯದಿ (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	ಪ್ರತ್ಯಾಗಮ TRUE;		// change of comment leader.

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    ಶೂನ್ಯ
auto_format(
    ಪೂರ್ಣಾಙ್ಕ		trailblank,	// when TRUE also format with trailing blank
    ಪೂರ್ಣಾಙ್ಕ		prev_ಸಾಲು)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    ಪೂರ್ಣಾಙ್ಕ		wasatend;

    ಯದಿ (!has_format_option(FO_AUTO))
	ಪ್ರತ್ಯಾಗಮ;

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
    ಯದಿ (*old != NUL && !trailblank && wasatend)
    {
	ಪೂರ್ಣಾಙ್ಕ cc;

	dec_cursor();
	cc = gchar_cursor();
	ಯದಿ (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ಯದಿ (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    ಪ್ರತ್ಯಾಗಮ;
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
    ಯದಿ (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_ಸೇರಿಸು))
    {
	char_u *ಸಾಲು = ml_get_curline();
	ಯದಿ (WHITECHAR(ಸಾಲು[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ಯದಿ (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ರಿಕ್ತ, FALSE, TRUE) == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ಯದಿ (prev_ಸಾಲು && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ಯದಿ (u_save_cursor() == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_ಸಾಲುಗಳು((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ಯದಿ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	coladvance((colnr_T)MAXCOL);
    }
    ಅನ್ಯಥಾ
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ಯದಿ (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ಯದಿ (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ಯದಿ (pnew == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
check_auto_format(
    ಪೂರ್ಣಾಙ್ಕ		end_ಸೇರಿಸು)	    // TRUE when ending Insert mode
{
    ಪೂರ್ಣಾಙ್ಕ		c = ' ';
    ಪೂರ್ಣಾಙ್ಕ		cc;

    ಯದಿ (!did_add_space)
	ಪ್ರತ್ಯಾಗಮ;

    cc = gchar_cursor();
    ಯದಿ (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!end_ಸೇರಿಸು)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ಯದಿ (c != NUL)
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
    ಪೂರ್ಣಾಙ್ಕ
comp_textwidth(
    ಪೂರ್ಣಾಙ್ಕ		ff)	// force formatting (for "gq" command)
{
    ಪೂರ್ಣಾಙ್ಕ		textwidth;

    textwidth = curbuf->b_p_tw;
    ಯದಿ (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	ಯದಿ (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	ಯದಿ (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	ಯದಿ (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ಯದಿ (textwidth < 0)
	textwidth = 0;
    ಯದಿ (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	ಯದಿ (textwidth > 79)
	    textwidth = 79;
    }
    ಪ್ರತ್ಯಾಗಮ textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    ಶೂನ್ಯ
op_format(
    oparg_T	*oap,
    ಪೂರ್ಣಾಙ್ಕ		keep_cursor)		// keep cursor on same text char
{
    long	old_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    ಯದಿ (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    curwin->w_cursor = oap->start;

    ಯದಿ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ಯದಿ (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_ಸಾಲುಗಳು(oap->ಸಾಲು_ಎಣಿಕೆ, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ಯದಿ (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_ಸಾಲು_ಎಣಿಕೆ;
    msgmore(old_ಸಾಲು_ಎಣಿಕೆ);

    ಯದಿ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    ಯದಿ (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ಯದಿ (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ಯದಿ (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_ಸಾಲು_ಎಣಿಕೆ;
		ಅನ್ಯಥಾ
		    wp->w_old_visual_lnum += old_ಸಾಲು_ಎಣಿಕೆ;
	    }
	}
    }
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    ಶೂನ್ಯ
op_formatexpr(oparg_T *oap)
{
    ಯದಿ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ಯದಿ (fex_format(oap->start.lnum, oap->ಸಾಲು_ಎಣಿಕೆ, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
fex_format(
    linenr_T	lnum,
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		c)	// character to be inserted
{
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    ಪೂರ್ಣಾಙ್ಕ		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_ಎಣಿಕೆ, ಎಣಿಕೆ);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ಯದಿ (fex == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ಯದಿ (use_sandbox)
	++sandbox;
    r = (ಪೂರ್ಣಾಙ್ಕ)eval_to_ಸಂಖ್ಯೆ(fex, TRUE);
    ಯದಿ (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, ರಿಕ್ತ, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    ಪ್ರತ್ಯಾಗಮ r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    ಶೂನ್ಯ
format_ಸಾಲುಗಳು(
    linenr_T	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		avoid_fex)		// don't use 'formatexpr'
{
    ಪೂರ್ಣಾಙ್ಕ		max_len;
    ಪೂರ್ಣಾಙ್ಕ		is_not_par;		// current line not part of parag.
    ಪೂರ್ಣಾಙ್ಕ		next_is_not_par;	// next line not part of paragraph
    ಪೂರ್ಣಾಙ್ಕ		is_end_par;		// at end of paragraph
    ಪೂರ್ಣಾಙ್ಕ		prev_is_end_par = FALSE;// prev. line not part of parag.
    ಪೂರ್ಣಾಙ್ಕ		next_is_start_par = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		leader_len = 0;		// leader len of current line
    ಪೂರ್ಣಾಙ್ಕ		next_leader_len;	// leader len of next line
    char_u	*leader_flags = ರಿಕ್ತ;	// flags for leader of current line
    char_u	*next_leader_flags = ರಿಕ್ತ; // flags for leader of next line
    ಪೂರ್ಣಾಙ್ಕ		do_comments;		// format comments
    ಪೂರ್ಣಾಙ್ಕ		do_comments_list = 0;	// format comments with 'n' or '2'
    ಪೂರ್ಣಾಙ್ಕ		advance = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		second_indent = -1;	// indent for second line (comment
					// aware)
    ಪೂರ್ಣಾಙ್ಕ		do_second_indent;
    ಪೂರ್ಣಾಙ್ಕ		do_ಸಂಖ್ಯೆ_indent;
    ಪೂರ್ಣಾಙ್ಕ		do_trail_white;
    ಪೂರ್ಣಾಙ್ಕ		first_par_ಸಾಲು = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		smd_save;
    long	ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_ಸಾಲು = curwin->w_cursor.lnum;
    ಪೂರ್ಣಾಙ್ಕ		force_format = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_ಸಂಖ್ಯೆ_indent = has_format_option(FO_Q_ಸಂಖ್ಯೆ);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ಯದಿ (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    ಅನ್ಯಥಾ
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    ಯದಿ (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    ಪ್ರತಿ (ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ; ಎಣಿಕೆ != 0 && !got_int; --ಎಣಿಕೆ)
    {
	// Advance to next paragraph.
	ಯದಿ (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	ಯದಿ (ಎಣಿಕೆ == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    ಯದಿ (do_ಸಂಖ್ಯೆ_indent)
		next_is_start_par =
			   (get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	ಯದಿ (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ಯದಿ (is_not_par)
	{
	    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ < 0)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ಯದಿ (first_par_ಸಾಲು
		    && (do_second_indent || do_ಸಂಖ್ಯೆ_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಯದಿ (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ಯದಿ (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    ಅನ್ಯಥಾ
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (do_ಸಂಖ್ಯೆ_indent)
		{
		    ಯದಿ (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum);
		    }
		    ಅನ್ಯಥಾ
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ಯದಿ (curwin->w_cursor.lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ಯದಿ (next_leader_flags == ರಿಕ್ತ
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ಯದಿ (is_end_par || force_format)
	    {
		ಯದಿ (need_set_indent)
		{
		    ಪೂರ್ಣಾಙ್ಕ		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ಯದಿ (curwin->w_cursor.lnum == first_ಸಾಲು)
			indent = get_indent();
		    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_ಅಭಿವ್ಯಕ್ತಿ_indent() :
#endif
				 get_c_indent();
			}
			ಅನ್ಯಥಾ
			    indent = get_indent();
		    }
		    (ಶೂನ್ಯ)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ಯಾವತ್ (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_ಸೇರಿಸು;	// for open_line()
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
		ಯದಿ (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ < 0)
			ಭಙ್ಗ;
		    first_par_ಸಾಲು = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ಯದಿ (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		ಯದಿ (ಸಾಲು_ಎಣಿಕೆ < 0 && u_save_cursor() == FAIL)
		    ಭಙ್ಗ;
		ಯದಿ (next_leader_len > 0)
		{
		    (ಶೂನ್ಯ)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		ಅನ್ಯಥಾ ಯದಿ (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    ಪೂರ್ಣಾಙ್ಕ indent = getwhitecols_curline();

		    ಯದಿ (indent > 0)
		    {
			(ಶೂನ್ಯ)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ಯದಿ (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    ಭಙ್ಗ;
		}
		first_par_ಸಾಲು = FALSE;
		// If the line is getting long, format it next time
		ಯದಿ (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		ಅನ್ಯಥಾ
		    force_format = FALSE;
	    }
	}
	ಸಾಲು_breakcheck();
    }
}
