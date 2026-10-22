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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

ਸਥਿਰ ਪੂਰਨ	did_ਜੋੜੋ_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    ਪੂਰਨ
has_format_option(ਪੂਰਨ x)
{
    ਜੇ (p_paste)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ (vim_strchr(curbuf->b_p_fo, x) != ਨਿਰਾ);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    ਖਾਲੀ
ਅੰਦਰੂਨੀ_format(
    ਪੂਰਨ		textwidth,
    ਪੂਰਨ		ਦੂਜਾ_indent,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		format_only,
    ਪੂਰਨ		c) // character to be inserted (can be NUL)
{
    ਪੂਰਨ		cc;
    ਪੂਰਨ		skip_pos;
    ਪੂਰਨ		save_char = NUL;
    ਪੂਰਨ		haveto_redraw = FALSE;
    ਪੂਰਨ		fo_ins_blank = has_format_option(FO_INS_BLANK);
    ਪੂਰਨ		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    ਪੂਰਨ		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    ਪੂਰਨ		fo_white_par = has_format_option(FO_WHITE_PAR);
    ਪੂਰਨ		ਪਹਿਲਾ_ਸਤਰ = TRUE;
    colnr_T	leader_len;
    ਪੂਰਨ		no_leader = FALSE;
    ਪੂਰਨ		do_comments = (flags & INSCHAR_DO_COM);
    ਪੂਰਨ		ਸੁਰੱਖਿਅਤ_tw = trim_ਮੰਜ਼ਿਲ_int(8 * (vimlong_T)textwidth);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    ਪੂਰਨ		has_lbr = curwin->w_p_lbr;
    ਪੂਰਨ		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#ਜੇ_ਅੰਤ

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ਜੇ (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ਜੇ (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ਜਦੋਂ (!got_int)
    {
	ਪੂਰਨ	startcol;		// Cursor column at entry
	ਪੂਰਨ	wantcol;		// column at textwidth border
	ਪੂਰਨ	foundcol;		// column for start of spaces
	ਪੂਰਨ	ਸਿਰਾ_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	ਪੂਰਨ	orig_ਖਾਨਾ = 0;
	char_u	*saved_ਪਾਠ = ਨਿਰਾ;
	colnr_T	ਖਾਨਾ;
	colnr_T	ਸਿਰਾ_ਖਾਨਾ;
	ਪੂਰਨ	wcc;			// counter for whitespace chars
	ਪੂਰਨ	did_do_comment = FALSE;
	ਪੂਰਨ	ਪਹਿਲਾ_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ਜੇ (curwin->w_cursor.ਖਾਨਾ < ਸੁਰੱਖਿਅਤ_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ਜੇ (virtcol <= (colnr_T)textwidth)
		ਰੋਕੋ;
	}

	ਜੇ (no_leader)
	    do_comments = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ਜੇ (do_comments)
	{
	    char_u *ਸਤਰ = ml_get_curline();

	    leader_len = get_leader_len(ਸਤਰ, ਨਿਰਾ, FALSE, TRUE);
	    ਜੇ (leader_len == 0 && curbuf->b_p_cin)
	    {
		ਪੂਰਨ		comment_ਸ਼ੁਰੂ;

		// Check for a line comment after code.
		comment_ਸ਼ੁਰੂ = check_linecomment(ਸਤਰ);
		ਜੇ (comment_ਸ਼ੁਰੂ != MAXCOL)
		{
		    leader_len = get_leader_len(
				      ਸਤਰ + comment_ਸ਼ੁਰੂ, ਨਿਰਾ, FALSE, TRUE);
		    ਜੇ (leader_len != 0)
			leader_len += comment_ਸ਼ੁਰੂ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ਜੇ (leader_len == 0)
	    no_leader = TRUE;
	ਜੇ (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    ਰੋਕੋ;
	ਜੇ ((startcol = curwin->w_cursor.ਖਾਨਾ) == 0)
	    ਰੋਕੋ;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.ਖਾਨਾ;

	// If startcol is large (a long line), formatting takes too much
	// time. The algorithm is O(n^2), it walks from the end of the
	// line to textwidth border every time for each line break.
	//
	// Ceil to 8 * textwidth to optimize.
	curwin->w_cursor.ਖਾਨਾ = startcol < ਸੁਰੱਖਿਅਤ_tw ? startcol : ਸੁਰੱਖਿਅਤ_tw;

	foundcol = 0;
	skip_pos = 0;
	ਪਹਿਲਾ_pass = TRUE;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	ਜਦੋਂ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.ਖਾਨਾ >= Insstart.ਖਾਨਾ)
	{
	    ਜੇ (ਪਹਿਲਾ_pass && c != NUL)
	    {
		cc = c;
		ਪਹਿਲਾ_pass = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
		cc = gchar_cursor();
	    ਜੇ (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		ਸਿਰਾ_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

		// find start of sequence of blanks
		wcc = 0;
		ਜਦੋਂ (curwin->w_cursor.ਖਾਨਾ > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ਜੇ (wcc < 2)
			wcc++;
		}
		ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0 && WHITECHAR(cc))
		    ਰੋਕੋ;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ਜੇ (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    ਜਾਰੀ;

		// Don't break until after the comment leader
		ਜੇ (curwin->w_cursor.ਖਾਨਾ < leader_len)
		    ਰੋਕੋ;
		ਜੇ (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
			ਰੋਕੋ;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ <= leader_len)
			ਰੋਕੋ;
		    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
		    dec_cursor();
		    cc = gchar_cursor();

		    ਜੇ (WHITECHAR(cc))
			ਜਾਰੀ;	// one-letter, continue
		    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		}

		inc_cursor();

		ਸਿਰਾ_foundcol = ਸਿਰਾ_ਖਾਨਾ + 1;
		foundcol = curwin->w_cursor.ਖਾਨਾ;
		ਜੇ (curwin->w_cursor.ਖਾਨਾ <= (colnr_T)wantcol)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		ਪੂਰਨ ncc;
		ਪੂਰਨ allow_break;

		// Break after or before a multi-byte character.
		ਜੇ (curwin->w_cursor.ਖਾਨਾ != startcol)
		{
		    // Don't break until after the comment leader
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ < leader_len)
			ਰੋਕੋ;
		    ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.ਖਾਨਾ;
			ਸਿਰਾ_foundcol = foundcol;
			ਜੇ (curwin->w_cursor.ਖਾਨਾ <= (colnr_T)wantcol)
			    ਰੋਕੋ;
		    }
		    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		}

		ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
		    ਰੋਕੋ;

		ncc = cc;
		ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

		dec_cursor();
		cc = gchar_cursor();

		ਜੇ (WHITECHAR(cc))
		    ਜਾਰੀ;		// break with space
		// Don't break until after the comment leader.
		ਜੇ (curwin->w_cursor.ਖਾਨਾ < leader_len)
		    ਰੋਕੋ;

		curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		skip_pos = curwin->w_cursor.ਖਾਨਾ;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ਜੇ (allow_break)
		{
		    foundcol = curwin->w_cursor.ਖਾਨਾ;
		    ਸਿਰਾ_foundcol = foundcol;
		}
		ਜੇ (curwin->w_cursor.ਖਾਨਾ <= (colnr_T)wantcol)
		{
		    ਪੂਰਨ ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ਜੇ (allow_break)
			ਰੋਕੋ;
		    ਜੇ (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ਜੇ (curwin->w_cursor.ਖਾਨਾ == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    ਸਿਰਾ_foundcol = foundcol = 0;
			    ਰੋਕੋ;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			ਜੇ (allow_break)
			{
			    // Break only when we are not at end of line.
			    ਸਿਰਾ_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.ਖਾਨਾ;
			    ਰੋਕੋ;
			}
			curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
		    }
		}
	    }
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
		ਰੋਕੋ;
	    dec_cursor();
	}

	ਜੇ (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.ਖਾਨਾ = startcol;
	    ਰੋਕੋ;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ਜੇ (State & VREPLACE_FLAG)
	    orig_ਖਾਨਾ = startcol;	// Will start backspacing from here
	ਨਹੀਂਤਾਂ
	    replace_ਵਿੱਥ = startcol - ਸਿਰਾ_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.ਖਾਨਾ = foundcol;
	ਜਦੋਂ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.ਖਾਨਾ < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.ਖਾਨਾ;
	ਜੇ (startcol < 0)
	    startcol = 0;

	ਜੇ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_ਪਾਠ = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.ਖਾਨਾ = orig_ਖਾਨਾ;
	    ਜੇ (saved_ਪਾਠ == ਨਿਰਾ)
		ਰੋਕੋ;	// Can't do it, out of memory
	    saved_ਪਾਠ[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ਜੇ (!fo_white_par)
		backspace_until_ਕਾਲਮ(foundcol);
	}
	ਨਹੀਂਤਾਂ
	{
	    // put cursor after pos. to break line
	    ਜੇ (!fo_white_par)
		curwin->w_cursor.ਖਾਨਾ = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_ਸਤਰ(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? ਦੂਜਾ_indent : old_indent),
		&did_do_comment);
	ਜੇ (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ਜੇ (did_do_comment)
	    no_leader = FALSE;

	replace_ਵਿੱਥ = 0;
	ਜੇ (ਪਹਿਲਾ_ਸਤਰ)
	{
	    ਜੇ (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ਜੇ (ਦੂਜਾ_indent < 0 && has_format_option(FO_Q_ਸੰਖਿਆ))
		    ਦੂਜਾ_indent =
				 get_ਸੰਖਿਆ_indent(curwin->w_cursor.lnum - 1);
		ਜੇ (ਦੂਜਾ_indent >= 0)
		{
		    ਜੇ (State & VREPLACE_FLAG)
			change_indent(INDENT_ਸੈੱਟ, ਦੂਜਾ_indent,
							    FALSE, NUL, TRUE);
		    ਨਹੀਂਤਾਂ
			ਜੇ (leader_len > 0 && ਦੂਜਾ_indent - leader_len > 0)
		    {
			ਪੂਰਨ i;
			ਪੂਰਨ padding = ਦੂਜਾ_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			ਲਈ (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			(ਖਾਲੀ)ਸੈੱਟ_indent(ਦੂਜਾ_indent, SIN_ਬਦਲਿਆ);
		    }
		}
	    }
	    ਪਹਿਲਾ_ਸਤਰ = FALSE;
	}

	ਜੇ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_ਬਾਈਟਾਂ(saved_ਪਾਠ);
	    vim_free(saved_ਪਾਠ);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.ਖਾਨਾ += startcol;
	    len = ml_get_curline_len();
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ > len)
		curwin->w_cursor.ਖਾਨਾ = len;
	}

	haveto_redraw = TRUE;
	ਸੈੱਟ_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_ਪਿੱਠ = FALSE;
	ਸਤਰ_breakcheck();
    }

    ਜੇ (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#ਜੇ_ਅੰਤ
    ਜੇ (!format_only && haveto_redraw)
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
    ਸਥਿਰ ਪੂਰਨ
fmt_check_par(
    linenr_T	lnum,
    ਪੂਰਨ		*leader_len,
    char_u	**leader_flags,
    ਪੂਰਨ		do_comments)
{
    char_u	*flags = ਨਿਰਾ;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ਜੇ (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    ਨਹੀਂਤਾਂ
	*leader_len = 0;

    ਜੇ (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ਜਦੋਂ (*flags && *flags != ':' && *flags != COM_ਸਿਰਾ)
	    ++flags;
    }

    ਵਾਪਸ (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_ਸਿਰਾ)
	    || ਸ਼ੁਰੂ_PS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸਿਰੇ_ਵਿੱਚ_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ਜੇ (*s == NUL)
	ਵਾਪਸ FALSE;
    l = ml_get_len(lnum) - 1;
    ਵਾਪਸ VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ਸਥਿਰ ਪੂਰਨ
same_leader(
    linenr_T lnum,
    ਪੂਰਨ	    leader1_len,
    char_u  *leader1_flags,
    ਪੂਰਨ	    leader2_len,
    char_u  *leader2_flags)
{
    ਪੂਰਨ	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ਜੇ (leader1_len == 0)
	ਵਾਪਸ (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ਜੇ (leader1_flags != ਨਿਰਾ)
    {
	char_u	*p;

	ਲਈ (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ਜੇ (*p == COM_ਪਹਿਲਾ)
		ਵਾਪਸ (leader2_len == 0);
	    ਜੇ (*p == COM_ਸਿਰਾ)
		ਵਾਪਸ FALSE;
	    ਜੇ (*p == COM_ਸ਼ੁਰੂ)
	    {
		ਪੂਰਨ ਸਤਰ_len = ml_get_len(lnum);
		ਜੇ (ਸਤਰ_len <= leader1_len)
		    ਵਾਪਸ FALSE;
		ਜੇ (leader2_flags == ਨਿਰਾ || leader2_len == 0)
		    ਵਾਪਸ FALSE;
		ਲਈ (p = leader2_flags; *p && *p != ':'; ++p)
		    ਜੇ (*p == COM_ਵਿਚਕਾਰ)
			ਵਾਪਸ TRUE;
		ਵਾਪਸ FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ਜੇ (line1 != ਨਿਰਾ)
    {
	char_u  *line2;

	ਲਈ (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	ਲਈ (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ਜੇ (!VIM_ISWHITE(line2[idx2]))
	    {
		ਜੇ (line1[idx1++] != line2[idx2])
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
		ਜਦੋਂ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    ਵਾਪਸ (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ਸਥਿਰ ਪੂਰਨ
paragraph_ਸ਼ੁਰੂ(linenr_T lnum)
{
    char_u	*p;
    ਪੂਰਨ		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ਨਿਰਾ;	// flags for leader of current line
    ਪੂਰਨ		ਅਗਲਾ_leader_len;	// leader len of next line
    char_u	*ਅਗਲਾ_leader_flags;	// flags for leader of next line
    ਪੂਰਨ		do_comments;		// format comments

    ਜੇ (lnum <= 1)
	ਵਾਪਸ TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ਜੇ (*p == NUL)
	ਵਾਪਸ TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ਜੇ (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	ਵਾਪਸ TRUE;		// after non-paragraph line

    ਜੇ (fmt_check_par(lnum, &ਅਗਲਾ_leader_len, &ਅਗਲਾ_leader_flags, do_comments))
	ਵਾਪਸ TRUE;		// "lnum" is not a paragraph line

    ਜੇ (has_format_option(FO_WHITE_PAR) && !ਸਿਰੇ_ਵਿੱਚ_white(lnum - 1))
	ਵਾਪਸ TRUE;		// missing trailing space in previous line.

    ਜੇ (has_format_option(FO_Q_ਸੰਖਿਆ) && (get_ਸੰਖਿਆ_indent(lnum) > 0))
	ਵਾਪਸ TRUE;		// numbered item starts in "lnum".

    ਜੇ (!same_leader(lnum - 1, leader_len, leader_flags,
					  ਅਗਲਾ_leader_len, ਅਗਲਾ_leader_flags))
	ਵਾਪਸ TRUE;		// change of comment leader.

    ਵਾਪਸ FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    ਖਾਲੀ
auto_format(
    ਪੂਰਨ		trailblank,	// when TRUE also format with trailing blank
    ਪੂਰਨ		prev_ਸਤਰ)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    ਪੂਰਨ		wasatend;

    ਜੇ (!has_format_option(FO_AUTO))
	ਵਾਪਸ;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.ਖਾਨਾ == ml_get_curline_len());
    ਜੇ (*old != NUL && !trailblank && wasatend)
    {
	ਪੂਰਨ cc;

	dec_cursor();
	cc = gchar_cursor();
	ਜੇ (!WHITECHAR(cc) && curwin->w_cursor.ਖਾਨਾ > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ਜੇ (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    ਵਾਪਸ;
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
    ਜੇ (*old != NUL && !trailblank && !wasatend && pos.ਖਾਨਾ > 0
	    && (State & ਬਹੁਲਕ_ਪਾਓ))
    {
	char_u *ਸਤਰ = ml_get_curline();
	ਜੇ (WHITECHAR(ਸਤਰ[pos.ਖਾਨਾ - 1]))
	{
	    curwin->w_cursor = pos;
	    ਵਾਪਸ;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ਜੇ (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ਨਿਰਾ, FALSE, TRUE) == 0)
	ਵਾਪਸ;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ਜੇ (prev_ਸਤਰ && !paragraph_ਸ਼ੁਰੂ(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ਜੇ (u_save_cursor() == FAIL)
	    ਵਾਪਸ;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_ਸਤਰਾਂ((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ਜੇ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	coladvance((colnr_T)MAXCOL);
    }
    ਨਹੀਂਤਾਂ
	check_cursor_ਖਾਨਾ();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ਜੇ (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ਜੇ (curwin->w_cursor.ਖਾਨਾ == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ਜੇ (pnew == ਨਿਰਾ)
		ਵਾਪਸ;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_ਜੋੜੋ_space = TRUE;
	}
	ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
check_auto_format(
    ਪੂਰਨ		ਸਿਰਾ_ਪਾਓ)	    // TRUE when ending Insert mode
{
    ਪੂਰਨ		c = ' ';
    ਪੂਰਨ		cc;

    ਜੇ (!did_ਜੋੜੋ_space)
	ਵਾਪਸ;

    cc = gchar_cursor();
    ਜੇ (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_ਜੋੜੋ_space = FALSE;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!ਸਿਰਾ_ਪਾਓ)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ਜੇ (c != NUL)
	{
	    // The space is no longer at the end of the line, delete it.
	    del_char(FALSE);
	    did_ਜੋੜੋ_space = FALSE;
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
    ਪੂਰਨ
comp_textwidth(
    ਪੂਰਨ		ff)	// force formatting (for "gq" command)
{
    ਪੂਰਨ		textwidth;

    textwidth = curbuf->b_p_tw;
    ਜੇ (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_ਚੌੜਾਈ - curbuf->b_p_wm;
	ਜੇ (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
	ਜੇ (signcolumn_on(curwin))
	    textwidth -= 1;
#ਜੇ_ਅੰਤ
	ਜੇ (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ਜੇ (textwidth < 0)
	textwidth = 0;
    ਜੇ (ff && textwidth == 0)
    {
	textwidth = curwin->w_ਚੌੜਾਈ - 1;
	ਜੇ (textwidth > 79)
	    textwidth = 79;
    }
    ਵਾਪਸ textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    ਖਾਲੀ
op_format(
    oparg_T	*oap,
    ਪੂਰਨ		keep_cursor)		// keep cursor on same text char
{
    ਲੰਮਾ	old_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_ਸ਼ੁਰੂ;

    ਜੇ (u_save((linenr_T)(oap->ਸ਼ੁਰੂ.lnum - 1),
				       (linenr_T)(oap->ਸਿਰਾ.lnum + 1)) == FAIL)
	ਵਾਪਸ;
    curwin->w_cursor = oap->ਸ਼ੁਰੂ;

    ਜੇ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_ਸ਼ੁਰੂ = oap->ਸ਼ੁਰੂ;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ਜੇ (keep_cursor)
	saved_cursor = oap->cursor_ਸ਼ੁਰੂ;

    format_ਸਤਰਾਂ(oap->ਸਤਰ_ਗਿਣਤੀ, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ਜੇ (oap->ਸਿਰਾ_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - old_ਸਤਰ_ਗਿਣਤੀ;
    msgmore(old_ਸਤਰ_ਗਿਣਤੀ);

    ਜੇ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_ਸਿਰਾ = curwin->w_cursor;

    ਜੇ (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ਜੇ (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ਜੇ (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_ਸਤਰ_ਗਿਣਤੀ;
		ਨਹੀਂਤਾਂ
		    wp->w_old_visual_lnum += old_ਸਤਰ_ਗਿਣਤੀ;
	    }
	}
    }
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    ਖਾਲੀ
op_formatexpr(oparg_T *oap)
{
    ਜੇ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ਜੇ (fex_format(oap->ਸ਼ੁਰੂ.lnum, oap->ਸਤਰ_ਗਿਣਤੀ, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    ਪੂਰਨ
fex_format(
    linenr_T	lnum,
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		c)	// character to be inserted
{
    ਪੂਰਨ		use_sandbox = was_ਸੈੱਟ_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    ਪੂਰਨ		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    ਸੈੱਟ_vim_var_nr(VV_LNUM, lnum);
    ਸੈੱਟ_vim_var_nr(VV_ਗਿਣਤੀ, ਗਿਣਤੀ_2);
    ਸੈੱਟ_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ਜੇ (fex == ਨਿਰਾ)
	ਵਾਪਸ 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ਜੇ (use_sandbox)
	++sandbox;
    r = (ਪੂਰਨ)eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(fex, TRUE);
    ਜੇ (use_sandbox)
	--sandbox;

    ਸੈੱਟ_vim_var_string(VV_CHAR, ਨਿਰਾ, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    ਵਾਪਸ r;
}
#ਜੇ_ਅੰਤ

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    ਖਾਲੀ
format_ਸਤਰਾਂ(
    linenr_T	ਸਤਰ_ਗਿਣਤੀ,
    ਪੂਰਨ		avoid_fex)		// don't use 'formatexpr'
{
    ਪੂਰਨ		max_len;
    ਪੂਰਨ		is_not_par;		// current line not part of parag.
    ਪੂਰਨ		ਅਗਲਾ_is_not_par;	// next line not part of paragraph
    ਪੂਰਨ		is_ਸਿਰਾ_par;		// at end of paragraph
    ਪੂਰਨ		prev_is_ਸਿਰਾ_par = FALSE;// prev. line not part of parag.
    ਪੂਰਨ		ਅਗਲਾ_is_ਸ਼ੁਰੂ_par = FALSE;
    ਪੂਰਨ		leader_len = 0;		// leader len of current line
    ਪੂਰਨ		ਅਗਲਾ_leader_len;	// leader len of next line
    char_u	*leader_flags = ਨਿਰਾ;	// flags for leader of current line
    char_u	*ਅਗਲਾ_leader_flags = ਨਿਰਾ; // flags for leader of next line
    ਪੂਰਨ		do_comments;		// format comments
    ਪੂਰਨ		do_comments_list = 0;	// format comments with 'n' or '2'
    ਪੂਰਨ		advance = TRUE;
    ਪੂਰਨ		ਦੂਜਾ_indent = -1;	// indent for second line (comment
					// aware)
    ਪੂਰਨ		do_ਦੂਜਾ_indent;
    ਪੂਰਨ		do_ਸੰਖਿਆ_indent;
    ਪੂਰਨ		do_trail_white;
    ਪੂਰਨ		ਪਹਿਲਾ_par_ਸਤਰ = TRUE;
    ਪੂਰਨ		smd_save;
    ਲੰਮਾ	ਗਿਣਤੀ_2;
    ਪੂਰਨ		need_ਸੈੱਟ_indent = TRUE;	// set indent of next paragraph
    linenr_T	ਪਹਿਲਾ_ਸਤਰ = curwin->w_cursor.lnum;
    ਪੂਰਨ		force_format = FALSE;
    ਪੂਰਨ		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_ਦੂਜਾ_indent = has_format_option(FO_Q_ਦੂਜਾ);
    do_ਸੰਖਿਆ_indent = has_format_option(FO_Q_ਸੰਖਿਆ);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ਜੇ (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    ਨਹੀਂਤਾਂ
	is_not_par = TRUE;
    ਅਗਲਾ_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &ਅਗਲਾ_leader_len, &ਅਗਲਾ_leader_flags, do_comments);
    is_ਸਿਰਾ_par = (is_not_par || ਅਗਲਾ_is_not_par);
    ਜੇ (!is_ਸਿਰਾ_par && do_trail_white)
	is_ਸਿਰਾ_par = !ਸਿਰੇ_ਵਿੱਚ_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    ਲਈ (ਗਿਣਤੀ_2 = ਸਤਰ_ਗਿਣਤੀ; ਗਿਣਤੀ_2 != 0 && !got_int; --ਗਿਣਤੀ_2)
    {
	// Advance to next paragraph.
	ਜੇ (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_ਸਿਰਾ_par = is_ਸਿਰਾ_par;
	    is_not_par = ਅਗਲਾ_is_not_par;
	    leader_len = ਅਗਲਾ_leader_len;
	    leader_flags = ਅਗਲਾ_leader_flags;
	}

	// The last line to be formatted.
	ਜੇ (ਗਿਣਤੀ_2 == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਅਗਲਾ_is_not_par = TRUE;
	    ਅਗਲਾ_leader_len = 0;
	    ਅਗਲਾ_leader_flags = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਅਗਲਾ_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &ਅਗਲਾ_leader_len, &ਅਗਲਾ_leader_flags, do_comments);
	    ਜੇ (do_ਸੰਖਿਆ_indent)
		ਅਗਲਾ_is_ਸ਼ੁਰੂ_par =
			   (get_ਸੰਖਿਆ_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_ਸਿਰਾ_par = (is_not_par || ਅਗਲਾ_is_not_par || ਅਗਲਾ_is_ਸ਼ੁਰੂ_par);
	ਜੇ (!is_ਸਿਰਾ_par && do_trail_white)
	    is_ਸਿਰਾ_par = !ਸਿਰੇ_ਵਿੱਚ_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ਜੇ (is_not_par)
	{
	    ਜੇ (ਸਤਰ_ਗਿਣਤੀ < 0)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ਜੇ (ਪਹਿਲਾ_par_ਸਤਰ
		    && (do_ਦੂਜਾ_indent || do_ਸੰਖਿਆ_indent)
		    && prev_is_ਸਿਰਾ_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਜੇ (do_ਦੂਜਾ_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ਜੇ (leader_len == 0 && ਅਗਲਾ_leader_len == 0)
		    {
			// no comment found
			ਦੂਜਾ_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			ਦੂਜਾ_indent = ਅਗਲਾ_leader_len;
			do_comments_list = 1;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (do_ਸੰਖਿਆ_indent)
		{
		    ਜੇ (leader_len == 0 && ਅਗਲਾ_leader_len == 0)
		    {
			// no comment found
			ਦੂਜਾ_indent =
				     get_ਸੰਖਿਆ_indent(curwin->w_cursor.lnum);
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// get_number_indent() is now "comment aware"...
			ਦੂਜਾ_indent =
				     get_ਸੰਖਿਆ_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ਜੇ (curwin->w_cursor.lnum >= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   ਅਗਲਾ_leader_len, ਅਗਲਾ_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ਜੇ (ਅਗਲਾ_leader_flags == ਨਿਰਾ
			|| STRNCMP(ਅਗਲਾ_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_ਸਿਰਾ_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ਜੇ (is_ਸਿਰਾ_par || force_format)
	    {
		ਜੇ (need_ਸੈੱਟ_indent)
		{
		    ਪੂਰਨ		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ਜੇ (curwin->w_cursor.lnum == ਪਹਿਲਾ_ਸਤਰ)
			indent = get_indent();
		    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (cindent_on())
			{
			    indent =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_ਵਿਅੰਜਕ_indent() :
#ਜੇ_ਅੰਤ
				 get_c_indent();
			}
			ਨਹੀਂਤਾਂ
			    indent = get_indent();
		    }
		    (ਖਾਲੀ)ਸੈੱਟ_indent(indent, SIN_ਬਦਲਿਆ);
		}

		// put cursor on last non-space
		State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ਜਦੋਂ (curwin->w_cursor.ਖਾਨਾ && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = ਬਹੁਲਕ_ਪਾਓ;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;

		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), ਦੂਜਾ_indent);

		State = old_State;
		p_smd = smd_save;
		// Cursor and mouse shape shapes may have been updated (e.g. by
		// :normal) in insertchar(), so they need to be updated here.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
		ui_cursor_shape();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
		update_mouseshape(-1);
#ਜੇ_ਅੰਤ

		ਦੂਜਾ_indent = -1;
		// at end of par.: need to set indent of next par.
		need_ਸੈੱਟ_indent = is_ਸਿਰਾ_par;
		ਜੇ (is_ਸਿਰਾ_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ਜੇ (ਸਤਰ_ਗਿਣਤੀ < 0)
			ਰੋਕੋ;
		    ਪਹਿਲਾ_par_ਸਤਰ = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ਜੇ (!is_ਸਿਰਾ_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ਖਾਨਾ = 0;
		ਜੇ (ਸਤਰ_ਗਿਣਤੀ < 0 && u_save_cursor() == FAIL)
		    ਰੋਕੋ;
		ਜੇ (ਅਗਲਾ_leader_len > 0)
		{
		    (ਖਾਲੀ)del_ਬਾਈਟਾਂ((ਲੰਮਾ)ਅਗਲਾ_leader_len, FALSE, FALSE);
		    mark_ਖਾਨਾ_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (ਲੰਮਾ)-ਅਗਲਾ_leader_len, 0);
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਦੂਜਾ_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    ਪੂਰਨ indent = getwhitecols_curline();

		    ਜੇ (indent > 0)
		    {
			(ਖਾਲੀ)del_ਬਾਈਟਾਂ(indent, FALSE, FALSE);
			mark_ਖਾਨਾ_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (ਲੰਮਾ)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ਜੇ (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    ਰੋਕੋ;
		}
		ਪਹਿਲਾ_par_ਸਤਰ = FALSE;
		// If the line is getting long, format it next time
		ਜੇ (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		ਨਹੀਂਤਾਂ
		    force_format = FALSE;
	    }
	}
	ਸਤਰ_breakcheck();
    }
}
