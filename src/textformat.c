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

#ଅନ୍ତର୍ଭାଵ "vim.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_format_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
    ଯଦି (p_paste)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ (vim_strchr(curbuf->b_p_fo, x) != ଖାଲି);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    ଶୂନ୍ୟ
internal_format(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		textwidth,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		second_indent,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		format_only,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c) // character to be inserted (can be NUL)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_pos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_char = NUL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		haveto_redraw = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fo_ins_blank = has_format_option(FO_INS_BLANK);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fo_white_par = has_format_option(FO_WHITE_PAR);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_ଧାଡ଼ି = TRUE;
    colnr_T	leader_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_leader = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_comments = (flags & INSCHAR_DO_COM);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_lbr = curwin->w_p_lbr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ଯଦି (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ଯଦି (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ଯେତେବେଳେ (!got_int)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	startcol;		// Cursor column at entry
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wantcol;		// column at textwidth border
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	foundcol;		// column for start of spaces
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_col = 0;
	char_u	*saved_text = ଖାଲି;
	colnr_T	col;
	colnr_T	end_col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	wcc;			// counter for whitespace chars
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_do_comment = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ଯଦି (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ଯଦି (virtcol <= (colnr_T)textwidth)
		ଭାଙ୍ଗ;
	}

	ଯଦି (no_leader)
	    do_comments = FALSE;
	ଅନ୍ୟଥା ଯଦି (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ଯଦି (do_comments)
	{
	    char_u *ଧାଡ଼ି = ml_get_curline();

	    leader_len = get_leader_len(ଧାଡ଼ି, ଖାଲି, FALSE, TRUE);
	    ଯଦି (leader_len == 0 && curbuf->b_p_cin)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(ଧାଡ଼ି);
		ଯଦି (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      ଧାଡ଼ି + comment_start, ଖାଲି, FALSE, TRUE);
		    ଯଦି (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	ଅନ୍ୟଥା
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ଯଦି (leader_len == 0)
	    no_leader = TRUE;
	ଯଦି (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    ଭାଙ୍ଗ;
	ଯଦି ((startcol = curwin->w_cursor.col) == 0)
	    ଭାଙ୍ଗ;

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
	ଯେତେବେଳେ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    ଯଦି (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    ଅନ୍ୟଥା
		cc = gchar_cursor();
	    ଯଦି (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		ଯେତେବେଳେ (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ଯଦି (wcc < 2)
			wcc++;
		}
		ଯଦି (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    ଭାଙ୍ଗ;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ଯଦି (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    ଜାରି;

		// Don't break until after the comment leader
		ଯଦି (curwin->w_cursor.col < leader_len)
		    ଭାଙ୍ଗ;
		ଯଦି (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ଯଦି (curwin->w_cursor.col == 0)
			ଭାଙ୍ଗ;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ଯଦି (curwin->w_cursor.col <= leader_len)
			ଭାଙ୍ଗ;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    ଯଦି (WHITECHAR(cc))
			ଜାରି;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		ଯଦି (curwin->w_cursor.col <= (colnr_T)wantcol)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncc;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_break;

		// Break after or before a multi-byte character.
		ଯଦି (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    ଯଦି (curwin->w_cursor.col < leader_len)
			ଭାଙ୍ଗ;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ଯଦି (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			ଯଦି (curwin->w_cursor.col <= (colnr_T)wantcol)
			    ଭାଙ୍ଗ;
		    }
		    curwin->w_cursor.col = col;
		}

		ଯଦି (curwin->w_cursor.col == 0)
		    ଭାଙ୍ଗ;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		ଯଦି (WHITECHAR(cc))
		    ଜାରି;		// break with space
		// Don't break until after the comment leader.
		ଯଦି (curwin->w_cursor.col < leader_len)
		    ଭାଙ୍ଗ;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ଯଦି (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		ଯଦି (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ଯଦି (allow_break)
			ଭାଙ୍ଗ;
		    ଯଦି (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ଯଦି (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    ଭାଙ୍ଗ;
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

			ଯଦି (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    ଭାଙ୍ଗ;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    ଯଦି (curwin->w_cursor.col == 0)
		ଭାଙ୍ଗ;
	    dec_cursor();
	}

	ଯଦି (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    ଭାଙ୍ଗ;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ଯଦି (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	ଅନ୍ୟଥା
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	ଯେତେବେଳେ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	ଯଦି (startcol < 0)
	    startcol = 0;

	ଯଦି (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    ଯଦି (saved_text == ଖାଲି)
		ଭାଙ୍ଗ;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ଯଦି (!fo_white_par)
		backspace_until_column(foundcol);
	}
	ଅନ୍ୟଥା
	{
	    // put cursor after pos. to break line
	    ଯଦି (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_ଧାଡ଼ି(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	ଯଦି (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ଯଦି (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	ଯଦି (first_ଧାଡ଼ି)
	{
	    ଯଦି (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ଯଦି (second_indent < 0 && has_format_option(FO_Q_ସଂଖ୍ୟା))
		    second_indent =
				 get_ସଂଖ୍ୟା_indent(curwin->w_cursor.lnum - 1);
		ଯଦି (second_indent >= 0)
		{
		    ଯଦି (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    ଅନ୍ୟଥା
			ଯଦି (leader_len > 0 && second_indent - leader_len > 0)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			ପାଇଁ (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    ଅନ୍ୟଥା
		    {
			(ଶୂନ୍ୟ)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_ଧାଡ଼ି = FALSE;
	}

	ଯଦି (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	ଅନ୍ୟଥା
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    ଯଦି (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	ଧାଡ଼ି_breakcheck();
    }

    ଯଦି (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    ଯଦି (!format_only && haveto_redraw)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fmt_check_par(
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*leader_len,
    char_u	**leader_flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_comments)
{
    char_u	*flags = ଖାଲି;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ଯଦି (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    ଅନ୍ୟଥା
	*leader_len = 0;

    ଯଦି (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ଯେତେବେଳେ (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    ଫେରନ୍ତୁ (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ଯଦି (*s == NUL)
	ଫେରନ୍ତୁ FALSE;
    l = ml_get_len(lnum) - 1;
    ଫେରନ୍ତୁ VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
same_leader(
    linenr_T lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    leader1_len,
    char_u  *leader1_flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    leader2_len,
    char_u  *leader2_flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ଯଦି (leader1_len == 0)
	ଫେରନ୍ତୁ (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ଯଦି (leader1_flags != ଖାଲି)
    {
	char_u	*p;

	ପାଇଁ (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ଯଦି (*p == COM_FIRST)
		ଫେରନ୍ତୁ (leader2_len == 0);
	    ଯଦି (*p == COM_END)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (*p == COM_START)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_len = ml_get_len(lnum);
		ଯଦି (ଧାଡ଼ି_len <= leader1_len)
		    ଫେରନ୍ତୁ FALSE;
		ଯଦି (leader2_flags == ଖାଲି || leader2_len == 0)
		    ଫେରନ୍ତୁ FALSE;
		ପାଇଁ (p = leader2_flags; *p && *p != ':'; ++p)
		    ଯଦି (*p == COM_ମଧ୍ୟ)
			ଫେରନ୍ତୁ TRUE;
		ଫେରନ୍ତୁ FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ଯଦି (line1 != ଖାଲି)
    {
	char_u  *line2;

	ପାଇଁ (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	ପାଇଁ (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ଯଦି (!VIM_ISWHITE(line2[idx2]))
	    {
		ଯଦି (line1[idx1++] != line2[idx2])
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
		ଯେତେବେଳେ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    ଫେରନ୍ତୁ (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ଖାଲି;	// flags for leader of current line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_comments;		// format comments

    ଯଦି (lnum <= 1)
	ଫେରନ୍ତୁ TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ଯଦି (*p == NUL)
	ଫେରନ୍ତୁ TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ଯଦି (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	ଫେରନ୍ତୁ TRUE;		// after non-paragraph line

    ଯଦି (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	ଫେରନ୍ତୁ TRUE;		// "lnum" is not a paragraph line

    ଯଦି (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	ଫେରନ୍ତୁ TRUE;		// missing trailing space in previous line.

    ଯଦି (has_format_option(FO_Q_ସଂଖ୍ୟା) && (get_ସଂଖ୍ୟା_indent(lnum) > 0))
	ଫେରନ୍ତୁ TRUE;		// numbered item starts in "lnum".

    ଯଦି (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	ଫେରନ୍ତୁ TRUE;		// change of comment leader.

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    ଶୂନ୍ୟ
auto_format(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trailblank,	// when TRUE also format with trailing blank
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_ଧାଡ଼ି)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wasatend;

    ଯଦି (!has_format_option(FO_AUTO))
	ଫେରନ୍ତୁ;

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
    ଯଦି (*old != NUL && !trailblank && wasatend)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc;

	dec_cursor();
	cc = gchar_cursor();
	ଯଦି (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ଯଦି (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    ଫେରନ୍ତୁ;
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
    ଯଦି (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_ଭର୍ତ୍ତି))
    {
	char_u *ଧାଡ଼ି = ml_get_curline();
	ଯଦି (WHITECHAR(ଧାଡ଼ି[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    ଫେରନ୍ତୁ;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ଯଦି (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ଖାଲି, FALSE, TRUE) == 0)
	ଫେରନ୍ତୁ;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ଯଦି (prev_ଧାଡ଼ି && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ଯଦି (u_save_cursor() == FAIL)
	    ଫେରନ୍ତୁ;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_ଧାଡ଼ିଗୁଡ଼ିକ((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ଯଦି (curwin->w_cursor.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	coladvance((colnr_T)MAXCOL);
    }
    ଅନ୍ୟଥା
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ଯଦି (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ଯଦି (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ଯଦି (pnew == ଖାଲି)
		ଫେରନ୍ତୁ;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
check_auto_format(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_ଭର୍ତ୍ତି)	    // TRUE when ending Insert mode
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = ' ';
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;

    ଯଦି (!did_add_space)
	ଫେରନ୍ତୁ;

    cc = gchar_cursor();
    ଯଦି (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    ଅନ୍ୟଥା
    {
	ଯଦି (!end_ଭର୍ତ୍ତି)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ଯଦି (c != NUL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
comp_textwidth(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ff)	// force formatting (for "gq" command)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		textwidth;

    textwidth = curbuf->b_p_tw;
    ଯଦି (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	ଯଦି (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	ଯଦି (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	ଯଦି (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ଯଦି (textwidth < 0)
	textwidth = 0;
    ଯଦି (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	ଯଦି (textwidth > 79)
	    textwidth = 79;
    }
    ଫେରନ୍ତୁ textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    ଶୂନ୍ୟ
op_format(
    oparg_T	*oap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_cursor)		// keep cursor on same text char
{
    long	old_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    ଯଦି (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	ଫେରନ୍ତୁ;
    curwin->w_cursor = oap->start;

    ଯଦି (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ଯଦି (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_ଧାଡ଼ିଗୁଡ଼ିକ(oap->ଧାଡ଼ି_ଗଣନା, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ଯଦି (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - old_ଧାଡ଼ି_ଗଣନା;
    msgmore(old_ଧାଡ଼ି_ଗଣନା);

    ଯଦି ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    ଯଦି (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ଯଦି (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ଯଦି (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_ଧାଡ଼ି_ଗଣନା;
		ଅନ୍ୟଥା
		    wp->w_old_visual_lnum += old_ଧାଡ଼ି_ଗଣନା;
	    }
	}
    }
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    ଶୂନ୍ୟ
op_formatexpr(oparg_T *oap)
{
    ଯଦି (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ଯଦି (fex_format(oap->start.lnum, oap->ଧାଡ଼ି_ଗଣନା, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fex_format(
    linenr_T	lnum,
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c)	// character to be inserted
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_ଗଣନା, ଗଣନା_2);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ଯଦି (fex == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ଯଦି (use_sandbox)
	++sandbox;
    r = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eval_to_ସଂଖ୍ୟା(fex, TRUE);
    ଯଦି (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, ଖାଲି, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    ଫେରନ୍ତୁ r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    ଶୂନ୍ୟ
format_ଧାଡ଼ିଗୁଡ଼ିକ(
    linenr_T	ଧାଡ଼ି_ଗଣନା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		avoid_fex)		// don't use 'formatexpr'
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_not_par;		// current line not part of parag.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_is_not_par;	// next line not part of paragraph
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_end_par;		// at end of paragraph
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_is_end_par = FALSE;// prev. line not part of parag.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_is_start_par = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		leader_len = 0;		// leader len of current line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_leader_len;	// leader len of next line
    char_u	*leader_flags = ଖାଲି;	// flags for leader of current line
    char_u	*next_leader_flags = ଖାଲି; // flags for leader of next line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_comments;		// format comments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_comments_list = 0;	// format comments with 'n' or '2'
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		advance = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		second_indent = -1;	// indent for second line (comment
					// aware)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_second_indent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_ସଂଖ୍ୟା_indent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_trail_white;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first_par_ଧାଡ଼ି = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		smd_save;
    long	ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_ଧାଡ଼ି = curwin->w_cursor.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force_format = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_ସଂଖ୍ୟା_indent = has_format_option(FO_Q_ସଂଖ୍ୟା);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ଯଦି (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    ଅନ୍ୟଥା
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    ଯଦି (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    ପାଇଁ (ଗଣନା_2 = ଧାଡ଼ି_ଗଣନା; ଗଣନା_2 != 0 && !got_int; --ଗଣନା_2)
    {
	// Advance to next paragraph.
	ଯଦି (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	ଯଦି (ଗଣନା_2 == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    ଯଦି (do_ସଂଖ୍ୟା_indent)
		next_is_start_par =
			   (get_ସଂଖ୍ୟା_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	ଯଦି (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ଯଦି (is_not_par)
	{
	    ଯଦି (ଧାଡ଼ି_ଗଣନା < 0)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ଯଦି (first_par_ଧାଡ଼ି
		    && (do_second_indent || do_ସଂଖ୍ୟା_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    {
		ଯଦି (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ଯଦି (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    ଅନ୍ୟଥା
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (do_ସଂଖ୍ୟା_indent)
		{
		    ଯଦି (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_ସଂଖ୍ୟା_indent(curwin->w_cursor.lnum);
		    }
		    ଅନ୍ୟଥା
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_ସଂଖ୍ୟା_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ଯଦି (curwin->w_cursor.lnum >= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ଯଦି (next_leader_flags == ଖାଲି
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ଯଦି (is_end_par || force_format)
	    {
		ଯଦି (need_set_indent)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ଯଦି (curwin->w_cursor.lnum == first_ଧାଡ଼ି)
			indent = get_indent();
		    ଅନ୍ୟଥା ଯଦି (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    ଅନ୍ୟଥା
		    {
			ଯଦି (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_ପଦସମଷ୍ଟି_indent() :
#endif
				 get_c_indent();
			}
			ଅନ୍ୟଥା
			    indent = get_indent();
		    }
		    (ଶୂନ୍ୟ)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ଯେତେବେଳେ (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_ଭର୍ତ୍ତି;	// for open_line()
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
		ଯଦି (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ଯଦି (ଧାଡ଼ି_ଗଣନା < 0)
			ଭାଙ୍ଗ;
		    first_par_ଧାଡ଼ି = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ଯଦି (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		ଯଦି (ଧାଡ଼ି_ଗଣନା < 0 && u_save_cursor() == FAIL)
		    ଭାଙ୍ଗ;
		ଯଦି (next_leader_len > 0)
		{
		    (ଶୂନ୍ୟ)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		ଅନ୍ୟଥା ଯଦି (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent = getwhitecols_curline();

		    ଯଦି (indent > 0)
		    {
			(ଶୂନ୍ୟ)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ଯଦି (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    ଭାଙ୍ଗ;
		}
		first_par_ଧାଡ଼ି = FALSE;
		// If the line is getting long, format it next time
		ଯଦି (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		ଅନ୍ୟଥା
		    force_format = FALSE;
	    }
	}
	ଧାଡ଼ି_breakcheck();
    }
}
