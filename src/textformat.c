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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_ಕೂಡಿಸು_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#ವ್ಯಾಖ್ಯಾನಿಸು WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    ಪೂರ್ಣಾಂಕ
has_format_option(ಪೂರ್ಣಾಂಕ x)
{
    ಆದರೆ (p_paste)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ (vim_strchr(curbuf->b_p_fo, x) != ಖಾಲಿ);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    ಶೂನ್ಯ
ಆಂತರಿಕ_format(
    ಪೂರ್ಣಾಂಕ		textwidth,
    ಪೂರ್ಣಾಂಕ		ಎರಡನೇ_indent,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		format_only,
    ಪೂರ್ಣಾಂಕ		c) // character to be inserted (can be NUL)
{
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		skip_pos;
    ಪೂರ್ಣಾಂಕ		save_char = NUL;
    ಪೂರ್ಣಾಂಕ		haveto_redraw = FALSE;
    ಪೂರ್ಣಾಂಕ		fo_ins_blank = has_format_option(FO_INS_BLANK);
    ಪೂರ್ಣಾಂಕ		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    ಪೂರ್ಣಾಂಕ		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    ಪೂರ್ಣಾಂಕ		fo_white_par = has_format_option(FO_WHITE_PAR);
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_ಸಾಲು = TRUE;
    colnr_T	leader_len;
    ಪೂರ್ಣಾಂಕ		no_leader = FALSE;
    ಪೂರ್ಣಾಂಕ		do_comments = (flags & INSCHAR_DO_COM);
    ಪೂರ್ಣಾಂಕ		ಸುರಕ್ಷಿತ_tw = trim_ಗಮ್ಯ_int(8 * (vimlong_T)textwidth);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    ಪೂರ್ಣಾಂಕ		has_lbr = curwin->w_p_lbr;
    ಪೂರ್ಣಾಂಕ		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ಆದರೆ (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ಆದರೆ (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ಆಗ (!got_int)
    {
	ಪೂರ್ಣಾಂಕ	startcol;		// Cursor column at entry
	ಪೂರ್ಣಾಂಕ	wantcol;		// column at textwidth border
	ಪೂರ್ಣಾಂಕ	foundcol;		// column for start of spaces
	ಪೂರ್ಣಾಂಕ	ಕೊನೆ_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	ಪೂರ್ಣಾಂಕ	orig_ಲಂಬಸಾಲು = 0;
	char_u	*saved_ಪಠ್ಯ = ಖಾಲಿ;
	colnr_T	ಲಂಬಸಾಲು;
	colnr_T	ಕೊನೆ_ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ	wcc;			// counter for whitespace chars
	ಪೂರ್ಣಾಂಕ	did_do_comment = FALSE;
	ಪೂರ್ಣಾಂಕ	ಮೊದಲ_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < ಸುರಕ್ಷಿತ_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ಆದರೆ (virtcol <= (colnr_T)textwidth)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (no_leader)
	    do_comments = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ಆದರೆ (do_comments)
	{
	    char_u *ಸಾಲು = ml_get_curline();

	    leader_len = get_leader_len(ಸಾಲು, ಖಾಲಿ, FALSE, TRUE);
	    ಆದರೆ (leader_len == 0 && curbuf->b_p_cin)
	    {
		ಪೂರ್ಣಾಂಕ		comment_ಆರಂಭ;

		// Check for a line comment after code.
		comment_ಆರಂಭ = check_linecomment(ಸಾಲು);
		ಆದರೆ (comment_ಆರಂಭ != MAXCOL)
		{
		    leader_len = get_leader_len(
				      ಸಾಲು + comment_ಆರಂಭ, ಖಾಲಿ, FALSE, TRUE);
		    ಆದರೆ (leader_len != 0)
			leader_len += comment_ಆರಂಭ;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ಆದರೆ (leader_len == 0)
	    no_leader = TRUE;
	ಆದರೆ (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    ನಿಲ್ಲಿಸು;
	ಆದರೆ ((startcol = curwin->w_cursor.ಲಂಬಸಾಲು) == 0)
	    ನಿಲ್ಲಿಸು;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.ಲಂಬಸಾಲು;

	// If startcol is large (a long line), formatting takes too much
	// time. The algorithm is O(n^2), it walks from the end of the
	// line to textwidth border every time for each line break.
	//
	// Ceil to 8 * textwidth to optimize.
	curwin->w_cursor.ಲಂಬಸಾಲು = startcol < ಸುರಕ್ಷಿತ_tw ? startcol : ಸುರಕ್ಷಿತ_tw;

	foundcol = 0;
	skip_pos = 0;
	ಮೊದಲ_pass = TRUE;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	ಆಗ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.ಲಂಬಸಾಲು >= Insstart.ಲಂಬಸಾಲು)
	{
	    ಆದರೆ (ಮೊದಲ_pass && c != NUL)
	    {
		cc = c;
		ಮೊದಲ_pass = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		cc = gchar_cursor();
	    ಆದರೆ (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		ಕೊನೆ_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

		// find start of sequence of blanks
		wcc = 0;
		ಆಗ (curwin->w_cursor.ಲಂಬಸಾಲು > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ಆದರೆ (wcc < 2)
			wcc++;
		}
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0 && WHITECHAR(cc))
		    ನಿಲ್ಲಿಸು;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ಆದರೆ (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    ಮುಂದುವರಿ;

		// Don't break until after the comment leader
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < leader_len)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
			ನಿಲ್ಲಿಸು;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= leader_len)
			ನಿಲ್ಲಿಸು;
		    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
		    dec_cursor();
		    cc = gchar_cursor();

		    ಆದರೆ (WHITECHAR(cc))
			ಮುಂದುವರಿ;	// one-letter, continue
		    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		}

		inc_cursor();

		ಕೊನೆ_foundcol = ಕೊನೆ_ಲಂಬಸಾಲು + 1;
		foundcol = curwin->w_cursor.ಲಂಬಸಾಲು;
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= (colnr_T)wantcol)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		ಪೂರ್ಣಾಂಕ ncc;
		ಪೂರ್ಣಾಂಕ allow_break;

		// Break after or before a multi-byte character.
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು != startcol)
		{
		    // Don't break until after the comment leader
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < leader_len)
			ನಿಲ್ಲಿಸು;
		    ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.ಲಂಬಸಾಲು;
			ಕೊನೆ_foundcol = foundcol;
			ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= (colnr_T)wantcol)
			    ನಿಲ್ಲಿಸು;
		    }
		    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		}

		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
		    ನಿಲ್ಲಿಸು;

		ncc = cc;
		ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

		dec_cursor();
		cc = gchar_cursor();

		ಆದರೆ (WHITECHAR(cc))
		    ಮುಂದುವರಿ;		// break with space
		// Don't break until after the comment leader.
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < leader_len)
		    ನಿಲ್ಲಿಸು;

		curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		skip_pos = curwin->w_cursor.ಲಂಬಸಾಲು;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ಆದರೆ (allow_break)
		{
		    foundcol = curwin->w_cursor.ಲಂಬಸಾಲು;
		    ಕೊನೆ_foundcol = foundcol;
		}
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= (colnr_T)wantcol)
		{
		    ಪೂರ್ಣಾಂಕ ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ಆದರೆ (allow_break)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    ಕೊನೆ_foundcol = foundcol = 0;
			    ನಿಲ್ಲಿಸು;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			ಆದರೆ (allow_break)
			{
			    // Break only when we are not at end of line.
			    ಕೊನೆ_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.ಲಂಬಸಾಲು;
			    ನಿಲ್ಲಿಸು;
			}
			curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		    }
		}
	    }
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
		ನಿಲ್ಲಿಸು;
	    dec_cursor();
	}

	ಆದರೆ (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = startcol;
	    ನಿಲ್ಲಿಸು;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ಆದರೆ (State & VREPLACE_FLAG)
	    orig_ಲಂಬಸಾಲು = startcol;	// Will start backspacing from here
	ಇಲ್ಲದಿದ್ದರೆ
	    replace_ಸ್ಥಾನಾಂತರ = startcol - ಕೊನೆ_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.ಲಂಬಸಾಲು = foundcol;
	ಆಗ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.ಲಂಬಸಾಲು < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.ಲಂಬಸಾಲು;
	ಆದರೆ (startcol < 0)
	    startcol = 0;

	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_ಪಠ್ಯ = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.ಲಂಬಸಾಲು = orig_ಲಂಬಸಾಲು;
	    ಆದರೆ (saved_ಪಠ್ಯ == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;	// Can't do it, out of memory
	    saved_ಪಠ್ಯ[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ಆದರೆ (!fo_white_par)
		backspace_until_ಸ್ತಂಭ(foundcol);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // put cursor after pos. to break line
	    ಆದರೆ (!fo_white_par)
		curwin->w_cursor.ಲಂಬಸಾಲು = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_ಸಾಲು(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? ಎರಡನೇ_indent : old_indent),
		&did_do_comment);
	ಆದರೆ (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ಆದರೆ (did_do_comment)
	    no_leader = FALSE;

	replace_ಸ್ಥಾನಾಂತರ = 0;
	ಆದರೆ (ಮೊದಲ_ಸಾಲು)
	{
	    ಆದರೆ (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ಆದರೆ (ಎರಡನೇ_indent < 0 && has_format_option(FO_Q_ಸಂಖ್ಯೆ))
		    ಎರಡನೇ_indent =
				 get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum - 1);
		ಆದರೆ (ಎರಡನೇ_indent >= 0)
		{
		    ಆದರೆ (State & VREPLACE_FLAG)
			change_indent(INDENT_ಗಣ, ಎರಡನೇ_indent,
							    FALSE, NUL, TRUE);
		    ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (leader_len > 0 && ಎರಡನೇ_indent - leader_len > 0)
		    {
			ಪೂರ್ಣಾಂಕ i;
			ಪೂರ್ಣಾಂಕ padding = ಎರಡನೇ_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			ಗಾಗಿ (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			(ಶೂನ್ಯ)ಗಣ_indent(ಎರಡನೇ_indent, SIN_ಬದಲಾಯಿತು);
		    }
		}
	    }
	    ಮೊದಲ_ಸಾಲು = FALSE;
	}

	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_ಬೈಟುಗಳು(saved_ಪಠ್ಯ);
	    vim_free(saved_ಪಠ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.ಲಂಬಸಾಲು += startcol;
	    len = ml_get_curline_len();
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > len)
		curwin->w_cursor.ಲಂಬಸಾಲು = len;
	}

	haveto_redraw = TRUE;
	ಗಣ_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_ಹಿಂಭಾಗ = FALSE;
	ಸಾಲು_breakcheck();
    }

    ಆದರೆ (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!format_only && haveto_redraw)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fmt_check_par(
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		*leader_len,
    char_u	**leader_flags,
    ಪೂರ್ಣಾಂಕ		do_comments)
{
    char_u	*flags = ಖಾಲಿ;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ಆದರೆ (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	*leader_len = 0;

    ಆದರೆ (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ಆಗ (*flags && *flags != ':' && *flags != COM_ಕೊನೆ)
	    ++flags;
    }

    ಹಿಂತಿರುಗಿ (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_ಕೊನೆ)
	    || ಆರಂಭ_PS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತುದಿಗಳು_ಒಳ_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ಆದರೆ (*s == NUL)
	ಹಿಂತಿರುಗಿ FALSE;
    l = ml_get_len(lnum) - 1;
    ಹಿಂತಿರುಗಿ VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
same_leader(
    linenr_T lnum,
    ಪೂರ್ಣಾಂಕ	    leader1_len,
    char_u  *leader1_flags,
    ಪೂರ್ಣಾಂಕ	    leader2_len,
    char_u  *leader2_flags)
{
    ಪೂರ್ಣಾಂಕ	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ಆದರೆ (leader1_len == 0)
	ಹಿಂತಿರುಗಿ (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ಆದರೆ (leader1_flags != ಖಾಲಿ)
    {
	char_u	*p;

	ಗಾಗಿ (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ಆದರೆ (*p == COM_ಮೊದಲ)
		ಹಿಂತಿರುಗಿ (leader2_len == 0);
	    ಆದರೆ (*p == COM_ಕೊನೆ)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (*p == COM_ಆರಂಭ)
	    {
		ಪೂರ್ಣಾಂಕ ಸಾಲು_len = ml_get_len(lnum);
		ಆದರೆ (ಸಾಲು_len <= leader1_len)
		    ಹಿಂತಿರುಗಿ FALSE;
		ಆದರೆ (leader2_flags == ಖಾಲಿ || leader2_len == 0)
		    ಹಿಂತಿರುಗಿ FALSE;
		ಗಾಗಿ (p = leader2_flags; *p && *p != ':'; ++p)
		    ಆದರೆ (*p == COM_ಮಧ್ಯ)
			ಹಿಂತಿರುಗಿ TRUE;
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ಆದರೆ (line1 != ಖಾಲಿ)
    {
	char_u  *line2;

	ಗಾಗಿ (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	ಗಾಗಿ (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ಆದರೆ (!VIM_ISWHITE(line2[idx2]))
	    {
		ಆದರೆ (line1[idx1++] != line2[idx2])
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆಗ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    ಹಿಂತಿರುಗಿ (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
paragraph_ಆರಂಭ(linenr_T lnum)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ಖಾಲಿ;	// flags for leader of current line
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_leader_len;	// leader len of next line
    char_u	*ಮುಂದಿನ_leader_flags;	// flags for leader of next line
    ಪೂರ್ಣಾಂಕ		do_comments;		// format comments

    ಆದರೆ (lnum <= 1)
	ಹಿಂತಿರುಗಿ TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ಆದರೆ (*p == NUL)
	ಹಿಂತಿರುಗಿ TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ಆದರೆ (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	ಹಿಂತಿರುಗಿ TRUE;		// after non-paragraph line

    ಆದರೆ (fmt_check_par(lnum, &ಮುಂದಿನ_leader_len, &ಮುಂದಿನ_leader_flags, do_comments))
	ಹಿಂತಿರುಗಿ TRUE;		// "lnum" is not a paragraph line

    ಆದರೆ (has_format_option(FO_WHITE_PAR) && !ತುದಿಗಳು_ಒಳ_white(lnum - 1))
	ಹಿಂತಿರುಗಿ TRUE;		// missing trailing space in previous line.

    ಆದರೆ (has_format_option(FO_Q_ಸಂಖ್ಯೆ) && (get_ಸಂಖ್ಯೆ_indent(lnum) > 0))
	ಹಿಂತಿರುಗಿ TRUE;		// numbered item starts in "lnum".

    ಆದರೆ (!same_leader(lnum - 1, leader_len, leader_flags,
					  ಮುಂದಿನ_leader_len, ಮುಂದಿನ_leader_flags))
	ಹಿಂತಿರುಗಿ TRUE;		// change of comment leader.

    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ		trailblank,	// when TRUE also format with trailing blank
    ಪೂರ್ಣಾಂಕ		prev_ಸಾಲು)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    ಪೂರ್ಣಾಂಕ		wasatend;

    ಆದರೆ (!has_format_option(FO_AUTO))
	ಹಿಂತಿರುಗಿ;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.ಲಂಬಸಾಲು == ml_get_curline_len());
    ಆದರೆ (*old != NUL && !trailblank && wasatend)
    {
	ಪೂರ್ಣಾಂಕ cc;

	dec_cursor();
	cc = gchar_cursor();
	ಆದರೆ (!WHITECHAR(cc) && curwin->w_cursor.ಲಂಬಸಾಲು > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ಆದರೆ (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    ಹಿಂತಿರುಗಿ;
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
    ಆದರೆ (*old != NUL && !trailblank && !wasatend && pos.ಲಂಬಸಾಲು > 0
	    && (State & ಬಹುಲಕ_ಸೇರಿಸು))
    {
	char_u *ಸಾಲು = ml_get_curline();
	ಆದರೆ (WHITECHAR(ಸಾಲು[pos.ಲಂಬಸಾಲು - 1]))
	{
	    curwin->w_cursor = pos;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ಆದರೆ (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ಖಾಲಿ, FALSE, TRUE) == 0)
	ಹಿಂತಿರುಗಿ;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ಆದರೆ (prev_ಸಾಲು && !paragraph_ಆರಂಭ(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ಆದರೆ (u_save_cursor() == FAIL)
	    ಹಿಂತಿರುಗಿ;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_ಸಾಲುಗಳು((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ಆದರೆ (curwin->w_cursor.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	coladvance((colnr_T)MAXCOL);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	check_cursor_ಲಂಬಸಾಲು();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ಆದರೆ (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ಆದರೆ (pnew == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_ಕೂಡಿಸು_space = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಸೇರಿಸು)	    // TRUE when ending Insert mode
{
    ಪೂರ್ಣಾಂಕ		c = ' ';
    ಪೂರ್ಣಾಂಕ		cc;

    ಆದರೆ (!did_ಕೂಡಿಸು_space)
	ಹಿಂತಿರುಗಿ;

    cc = gchar_cursor();
    ಆದರೆ (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_ಕೂಡಿಸು_space = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!ಕೊನೆ_ಸೇರಿಸು)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ಆದರೆ (c != NUL)
	{
	    // The space is no longer at the end of the line, delete it.
	    del_char(FALSE);
	    did_ಕೂಡಿಸು_space = FALSE;
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
    ಪೂರ್ಣಾಂಕ
comp_textwidth(
    ಪೂರ್ಣಾಂಕ		ff)	// force formatting (for "gq" command)
{
    ಪೂರ್ಣಾಂಕ		textwidth;

    textwidth = curbuf->b_p_tw;
    ಆದರೆ (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_ಅಗಲ - curbuf->b_p_wm;
	ಆದರೆ (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
	ಆದರೆ (signcolumn_on(curwin))
	    textwidth -= 1;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ಆದರೆ (textwidth < 0)
	textwidth = 0;
    ಆದರೆ (ff && textwidth == 0)
    {
	textwidth = curwin->w_ಅಗಲ - 1;
	ಆದರೆ (textwidth > 79)
	    textwidth = 79;
    }
    ಹಿಂತಿರುಗಿ textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    ಶೂನ್ಯ
op_format(
    oparg_T	*oap,
    ಪೂರ್ಣಾಂಕ		keep_cursor)		// keep cursor on same text char
{
    ದೀರ್ಘ	old_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_ಆರಂಭ;

    ಆದರೆ (u_save((linenr_T)(oap->ಆರಂಭ.lnum - 1),
				       (linenr_T)(oap->ಕೊನೆ.lnum + 1)) == FAIL)
	ಹಿಂತಿರುಗಿ;
    curwin->w_cursor = oap->ಆರಂಭ;

    ಆದರೆ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_ಆರಂಭ = oap->ಆರಂಭ;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ಆದರೆ (keep_cursor)
	saved_cursor = oap->cursor_ಆರಂಭ;

    format_ಸಾಲುಗಳು(oap->ಸಾಲು_ಎಣಿಕೆ, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ಆದರೆ (oap->ಕೊನೆ_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - old_ಸಾಲು_ಎಣಿಕೆ;
    msgmore(old_ಸಾಲು_ಎಣಿಕೆ);

    ಆದರೆ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_ಕೊನೆ = curwin->w_cursor;

    ಆದರೆ (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ಆದರೆ (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ಆದರೆ (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_ಸಾಲು_ಎಣಿಕೆ;
		ಇಲ್ಲದಿದ್ದರೆ
		    wp->w_old_visual_lnum += old_ಸಾಲು_ಎಣಿಕೆ;
	    }
	}
    }
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    ಶೂನ್ಯ
op_formatexpr(oparg_T *oap)
{
    ಆದರೆ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ಆದರೆ (fex_format(oap->ಆರಂಭ.lnum, oap->ಸಾಲು_ಎಣಿಕೆ, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    ಪೂರ್ಣಾಂಕ
fex_format(
    linenr_T	lnum,
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		c)	// character to be inserted
{
    ಪೂರ್ಣಾಂಕ		use_sandbox = was_ಗಣ_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    ಪೂರ್ಣಾಂಕ		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    ಗಣ_vim_var_nr(VV_LNUM, lnum);
    ಗಣ_vim_var_nr(VV_ಎಣಿಕೆ, ಎಣಿಕೆ_2);
    ಗಣ_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ಆದರೆ (fex == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ಆದರೆ (use_sandbox)
	++sandbox;
    r = (ಪೂರ್ಣಾಂಕ)eval_ಗಮ್ಯ_ಸಂಖ್ಯೆ(fex, TRUE);
    ಆದರೆ (use_sandbox)
	--sandbox;

    ಗಣ_vim_var_string(VV_CHAR, ಖಾಲಿ, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    ಹಿಂತಿರುಗಿ r;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    ಶೂನ್ಯ
format_ಸಾಲುಗಳು(
    linenr_T	ಸಾಲು_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		avoid_fex)		// don't use 'formatexpr'
{
    ಪೂರ್ಣಾಂಕ		max_len;
    ಪೂರ್ಣಾಂಕ		is_not_par;		// current line not part of parag.
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_is_not_par;	// next line not part of paragraph
    ಪೂರ್ಣಾಂಕ		is_ಕೊನೆ_par;		// at end of paragraph
    ಪೂರ್ಣಾಂಕ		prev_is_ಕೊನೆ_par = FALSE;// prev. line not part of parag.
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_is_ಆರಂಭ_par = FALSE;
    ಪೂರ್ಣಾಂಕ		leader_len = 0;		// leader len of current line
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_leader_len;	// leader len of next line
    char_u	*leader_flags = ಖಾಲಿ;	// flags for leader of current line
    char_u	*ಮುಂದಿನ_leader_flags = ಖಾಲಿ; // flags for leader of next line
    ಪೂರ್ಣಾಂಕ		do_comments;		// format comments
    ಪೂರ್ಣಾಂಕ		do_comments_list = 0;	// format comments with 'n' or '2'
    ಪೂರ್ಣಾಂಕ		advance = TRUE;
    ಪೂರ್ಣಾಂಕ		ಎರಡನೇ_indent = -1;	// indent for second line (comment
					// aware)
    ಪೂರ್ಣಾಂಕ		do_ಎರಡನೇ_indent;
    ಪೂರ್ಣಾಂಕ		do_ಸಂಖ್ಯೆ_indent;
    ಪೂರ್ಣಾಂಕ		do_trail_white;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ_par_ಸಾಲು = TRUE;
    ಪೂರ್ಣಾಂಕ		smd_save;
    ದೀರ್ಘ	ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		need_ಗಣ_indent = TRUE;	// set indent of next paragraph
    linenr_T	ಮೊದಲ_ಸಾಲು = curwin->w_cursor.lnum;
    ಪೂರ್ಣಾಂಕ		force_format = FALSE;
    ಪೂರ್ಣಾಂಕ		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_ಎರಡನೇ_indent = has_format_option(FO_Q_ಎರಡನೇ);
    do_ಸಂಖ್ಯೆ_indent = has_format_option(FO_Q_ಸಂಖ್ಯೆ);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ಆದರೆ (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    ಇಲ್ಲದಿದ್ದರೆ
	is_not_par = TRUE;
    ಮುಂದಿನ_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &ಮುಂದಿನ_leader_len, &ಮುಂದಿನ_leader_flags, do_comments);
    is_ಕೊನೆ_par = (is_not_par || ಮುಂದಿನ_is_not_par);
    ಆದರೆ (!is_ಕೊನೆ_par && do_trail_white)
	is_ಕೊನೆ_par = !ತುದಿಗಳು_ಒಳ_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    ಗಾಗಿ (ಎಣಿಕೆ_2 = ಸಾಲು_ಎಣಿಕೆ; ಎಣಿಕೆ_2 != 0 && !got_int; --ಎಣಿಕೆ_2)
    {
	// Advance to next paragraph.
	ಆದರೆ (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_ಕೊನೆ_par = is_ಕೊನೆ_par;
	    is_not_par = ಮುಂದಿನ_is_not_par;
	    leader_len = ಮುಂದಿನ_leader_len;
	    leader_flags = ಮುಂದಿನ_leader_flags;
	}

	// The last line to be formatted.
	ಆದರೆ (ಎಣಿಕೆ_2 == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಮುಂದಿನ_is_not_par = TRUE;
	    ಮುಂದಿನ_leader_len = 0;
	    ಮುಂದಿನ_leader_flags = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಮುಂದಿನ_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &ಮುಂದಿನ_leader_len, &ಮುಂದಿನ_leader_flags, do_comments);
	    ಆದರೆ (do_ಸಂಖ್ಯೆ_indent)
		ಮುಂದಿನ_is_ಆರಂಭ_par =
			   (get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_ಕೊನೆ_par = (is_not_par || ಮುಂದಿನ_is_not_par || ಮುಂದಿನ_is_ಆರಂಭ_par);
	ಆದರೆ (!is_ಕೊನೆ_par && do_trail_white)
	    is_ಕೊನೆ_par = !ತುದಿಗಳು_ಒಳ_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ಆದರೆ (is_not_par)
	{
	    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ < 0)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ಆದರೆ (ಮೊದಲ_par_ಸಾಲು
		    && (do_ಎರಡನೇ_indent || do_ಸಂಖ್ಯೆ_indent)
		    && prev_is_ಕೊನೆ_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ಆದರೆ (do_ಎರಡನೇ_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ಆದರೆ (leader_len == 0 && ಮುಂದಿನ_leader_len == 0)
		    {
			// no comment found
			ಎರಡನೇ_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಎರಡನೇ_indent = ಮುಂದಿನ_leader_len;
			do_comments_list = 1;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ಸಂಖ್ಯೆ_indent)
		{
		    ಆದರೆ (leader_len == 0 && ಮುಂದಿನ_leader_len == 0)
		    {
			// no comment found
			ಎರಡನೇ_indent =
				     get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// get_number_indent() is now "comment aware"...
			ಎರಡನೇ_indent =
				     get_ಸಂಖ್ಯೆ_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ಆದರೆ (curwin->w_cursor.lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   ಮುಂದಿನ_leader_len, ಮುಂದಿನ_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ಆದರೆ (ಮುಂದಿನ_leader_flags == ಖಾಲಿ
			|| STRNCMP(ಮುಂದಿನ_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_ಕೊನೆ_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ಆದರೆ (is_ಕೊನೆ_par || force_format)
	    {
		ಆದರೆ (need_ಗಣ_indent)
		{
		    ಪೂರ್ಣಾಂಕ		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ಆದರೆ (curwin->w_cursor.lnum == ಮೊದಲ_ಸಾಲು)
			indent = get_indent();
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (cindent_on())
			{
			    indent =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_ಅಭಿವ್ಯಕ್ತಿ_indent() :
#ಆದರೆ_ಅಂತ್ಯ
				 get_c_indent();
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    indent = get_indent();
		    }
		    (ಶೂನ್ಯ)ಗಣ_indent(indent, SIN_ಬದಲಾಯಿತು);
		}

		// put cursor on last non-space
		State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ಆಗ (curwin->w_cursor.ಲಂಬಸಾಲು && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = ಬಹುಲಕ_ಸೇರಿಸು;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;

		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), ಎರಡನೇ_indent);

		State = old_State;
		p_smd = smd_save;
		// Cursor and mouse shape shapes may have been updated (e.g. by
		// :normal) in insertchar(), so they need to be updated here.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
		ui_cursor_shape();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
		update_mouseshape(-1);
#ಆದರೆ_ಅಂತ್ಯ

		ಎರಡನೇ_indent = -1;
		// at end of par.: need to set indent of next par.
		need_ಗಣ_indent = is_ಕೊನೆ_par;
		ಆದರೆ (is_ಕೊನೆ_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ < 0)
			ನಿಲ್ಲಿಸು;
		    ಮೊದಲ_par_ಸಾಲು = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ಆದರೆ (!is_ಕೊನೆ_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ < 0 && u_save_cursor() == FAIL)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (ಮುಂದಿನ_leader_len > 0)
		{
		    (ಶೂನ್ಯ)del_ಬೈಟುಗಳು((ದೀರ್ಘ)ಮುಂದಿನ_leader_len, FALSE, FALSE);
		    mark_ಲಂಬಸಾಲು_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (ದೀರ್ಘ)-ಮುಂದಿನ_leader_len, 0);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎರಡನೇ_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    ಪೂರ್ಣಾಂಕ indent = getwhitecols_curline();

		    ಆದರೆ (indent > 0)
		    {
			(ಶೂನ್ಯ)del_ಬೈಟುಗಳು(indent, FALSE, FALSE);
			mark_ಲಂಬಸಾಲು_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (ದೀರ್ಘ)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ಆದರೆ (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    ನಿಲ್ಲಿಸು;
		}
		ಮೊದಲ_par_ಸಾಲು = FALSE;
		// If the line is getting long, format it next time
		ಆದರೆ (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		    force_format = FALSE;
	    }
	}
	ಸಾಲು_breakcheck();
    }
}
