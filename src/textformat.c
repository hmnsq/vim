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

#అన్తర్భావ "vim.h"

స్థిర పూర్ణాంకం	did_కూడు_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#నిర్వచించు WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    పూర్ణాంకం
has_format_option(పూర్ణాంకం x)
{
    ఒకవేళ (p_paste)
	తిరిగి FALSE;
    తిరిగి (vim_strchr(curbuf->b_p_fo, x) != ఖాళీ);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    శూన్య
అంతర్గత_format(
    పూర్ణాంకం		textwidth,
    పూర్ణాంకం		రెండవ_indent,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		format_only,
    పూర్ణాంకం		c) // character to be inserted (can be NUL)
{
    పూర్ణాంకం		cc;
    పూర్ణాంకం		skip_pos;
    పూర్ణాంకం		save_char = NUL;
    పూర్ణాంకం		haveto_redraw = FALSE;
    పూర్ణాంకం		fo_ins_blank = has_format_option(FO_INS_BLANK);
    పూర్ణాంకం		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    పూర్ణాంకం		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    పూర్ణాంకం		fo_white_par = has_format_option(FO_WHITE_PAR);
    పూర్ణాంకం		మొదటి_పంక్తి = TRUE;
    colnr_T	leader_len;
    పూర్ణాంకం		no_leader = FALSE;
    పూర్ణాంకం		do_comments = (flags & INSCHAR_DO_COM);
    పూర్ణాంకం		సురక్షిత_tw = trim_గమ్యం_int(8 * (vimlong_T)textwidth);
#నిర్వచితమైతే FEAT_LINEBREAK
    పూర్ణాంకం		has_lbr = curwin->w_p_lbr;
    పూర్ణాంకం		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#అయితే_ముగింపు

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ఒకవేళ (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ఒకవేళ (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    అయితే (!got_int)
    {
	పూర్ణాంకం	startcol;		// Cursor column at entry
	పూర్ణాంకం	wantcol;		// column at textwidth border
	పూర్ణాంకం	foundcol;		// column for start of spaces
	పూర్ణాంకం	చివర_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	పూర్ణాంకం	orig_నిలువు = 0;
	char_u	*saved_పాఠ్యం = ఖాళీ;
	colnr_T	నిలువు;
	colnr_T	చివర_నిలువు;
	పూర్ణాంకం	wcc;			// counter for whitespace chars
	పూర్ణాంకం	did_do_comment = FALSE;
	పూర్ణాంకం	మొదటి_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ఒకవేళ (curwin->w_cursor.నిలువు < సురక్షిత_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ఒకవేళ (virtcol <= (colnr_T)textwidth)
		ఆపు;
	}

	ఒకవేళ (no_leader)
	    do_comments = FALSE;
	లేకపోతే ఒకవేళ (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ఒకవేళ (do_comments)
	{
	    char_u *పంక్తి = ml_get_curline();

	    leader_len = get_leader_len(పంక్తి, ఖాళీ, FALSE, TRUE);
	    ఒకవేళ (leader_len == 0 && curbuf->b_p_cin)
	    {
		పూర్ణాంకం		comment_ప్రారంభం;

		// Check for a line comment after code.
		comment_ప్రారంభం = check_linecomment(పంక్తి);
		ఒకవేళ (comment_ప్రారంభం != MAXCOL)
		{
		    leader_len = get_leader_len(
				      పంక్తి + comment_ప్రారంభం, ఖాళీ, FALSE, TRUE);
		    ఒకవేళ (leader_len != 0)
			leader_len += comment_ప్రారంభం;
		}
	    }
	}
	లేకపోతే
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ఒకవేళ (leader_len == 0)
	    no_leader = TRUE;
	ఒకవేళ (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    ఆపు;
	ఒకవేళ ((startcol = curwin->w_cursor.నిలువు) == 0)
	    ఆపు;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.నిలువు;

	// If startcol is large (a long line), formatting takes too much
	// time. The algorithm is O(n^2), it walks from the end of the
	// line to textwidth border every time for each line break.
	//
	// Ceil to 8 * textwidth to optimize.
	curwin->w_cursor.నిలువు = startcol < సురక్షిత_tw ? startcol : సురక్షిత_tw;

	foundcol = 0;
	skip_pos = 0;
	మొదటి_pass = TRUE;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	అయితే ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.నిలువు >= Insstart.నిలువు)
	{
	    ఒకవేళ (మొదటి_pass && c != NUL)
	    {
		cc = c;
		మొదటి_pass = FALSE;
	    }
	    లేకపోతే
		cc = gchar_cursor();
	    ఒకవేళ (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		చివర_నిలువు = curwin->w_cursor.నిలువు;

		// find start of sequence of blanks
		wcc = 0;
		అయితే (curwin->w_cursor.నిలువు > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ఒకవేళ (wcc < 2)
			wcc++;
		}
		ఒకవేళ (curwin->w_cursor.నిలువు == 0 && WHITECHAR(cc))
		    ఆపు;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ఒకవేళ (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    కొనసాగు;

		// Don't break until after the comment leader
		ఒకవేళ (curwin->w_cursor.నిలువు < leader_len)
		    ఆపు;
		ఒకవేళ (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
			ఆపు;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ఒకవేళ (curwin->w_cursor.నిలువు <= leader_len)
			ఆపు;
		    నిలువు = curwin->w_cursor.నిలువు;
		    dec_cursor();
		    cc = gchar_cursor();

		    ఒకవేళ (WHITECHAR(cc))
			కొనసాగు;	// one-letter, continue
		    curwin->w_cursor.నిలువు = నిలువు;
		}

		inc_cursor();

		చివర_foundcol = చివర_నిలువు + 1;
		foundcol = curwin->w_cursor.నిలువు;
		ఒకవేళ (curwin->w_cursor.నిలువు <= (colnr_T)wantcol)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		పూర్ణాంకం ncc;
		పూర్ణాంకం allow_break;

		// Break after or before a multi-byte character.
		ఒకవేళ (curwin->w_cursor.నిలువు != startcol)
		{
		    // Don't break until after the comment leader
		    ఒకవేళ (curwin->w_cursor.నిలువు < leader_len)
			ఆపు;
		    నిలువు = curwin->w_cursor.నిలువు;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ఒకవేళ (curwin->w_cursor.నిలువు != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.నిలువు;
			చివర_foundcol = foundcol;
			ఒకవేళ (curwin->w_cursor.నిలువు <= (colnr_T)wantcol)
			    ఆపు;
		    }
		    curwin->w_cursor.నిలువు = నిలువు;
		}

		ఒకవేళ (curwin->w_cursor.నిలువు == 0)
		    ఆపు;

		ncc = cc;
		నిలువు = curwin->w_cursor.నిలువు;

		dec_cursor();
		cc = gchar_cursor();

		ఒకవేళ (WHITECHAR(cc))
		    కొనసాగు;		// break with space
		// Don't break until after the comment leader.
		ఒకవేళ (curwin->w_cursor.నిలువు < leader_len)
		    ఆపు;

		curwin->w_cursor.నిలువు = నిలువు;
		skip_pos = curwin->w_cursor.నిలువు;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ఒకవేళ (allow_break)
		{
		    foundcol = curwin->w_cursor.నిలువు;
		    చివర_foundcol = foundcol;
		}
		ఒకవేళ (curwin->w_cursor.నిలువు <= (colnr_T)wantcol)
		{
		    పూర్ణాంకం ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ఒకవేళ (allow_break)
			ఆపు;
		    ఒకవేళ (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ఒకవేళ (curwin->w_cursor.నిలువు == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    చివర_foundcol = foundcol = 0;
			    ఆపు;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			నిలువు = curwin->w_cursor.నిలువు;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			ఒకవేళ (allow_break)
			{
			    // Break only when we are not at end of line.
			    చివర_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.నిలువు;
			    ఆపు;
			}
			curwin->w_cursor.నిలువు = నిలువు;
		    }
		}
	    }
	    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
		ఆపు;
	    dec_cursor();
	}

	ఒకవేళ (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.నిలువు = startcol;
	    ఆపు;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ఒకవేళ (State & VREPLACE_FLAG)
	    orig_నిలువు = startcol;	// Will start backspacing from here
	లేకపోతే
	    replace_స్థానభ్రంశం = startcol - చివర_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.నిలువు = foundcol;
	అయితే ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.నిలువు < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.నిలువు;
	ఒకవేళ (startcol < 0)
	    startcol = 0;

	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_పాఠ్యం = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.నిలువు = orig_నిలువు;
	    ఒకవేళ (saved_పాఠ్యం == ఖాళీ)
		ఆపు;	// Can't do it, out of memory
	    saved_పాఠ్యం[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ఒకవేళ (!fo_white_par)
		backspace_until_స్తంభం(foundcol);
	}
	లేకపోతే
	{
	    // put cursor after pos. to break line
	    ఒకవేళ (!fo_white_par)
		curwin->w_cursor.నిలువు = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_పంక్తి(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? రెండవ_indent : old_indent),
		&did_do_comment);
	ఒకవేళ (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ఒకవేళ (did_do_comment)
	    no_leader = FALSE;

	replace_స్థానభ్రంశం = 0;
	ఒకవేళ (మొదటి_పంక్తి)
	{
	    ఒకవేళ (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ఒకవేళ (రెండవ_indent < 0 && has_format_option(FO_Q_సంఖ్య))
		    రెండవ_indent =
				 get_సంఖ్య_indent(curwin->w_cursor.lnum - 1);
		ఒకవేళ (రెండవ_indent >= 0)
		{
		    ఒకవేళ (State & VREPLACE_FLAG)
			change_indent(INDENT_సమితి, రెండవ_indent,
							    FALSE, NUL, TRUE);
		    లేకపోతే
			ఒకవేళ (leader_len > 0 && రెండవ_indent - leader_len > 0)
		    {
			పూర్ణాంకం i;
			పూర్ణాంకం padding = రెండవ_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			కొరకు (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    లేకపోతే
		    {
			(శూన్య)సమితి_indent(రెండవ_indent, SIN_మారింది);
		    }
		}
	    }
	    మొదటి_పంక్తి = FALSE;
	}

	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_బైట్లు(saved_పాఠ్యం);
	    vim_free(saved_పాఠ్యం);
	}
	లేకపోతే
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.నిలువు += startcol;
	    len = ml_get_curline_len();
	    ఒకవేళ (curwin->w_cursor.నిలువు > len)
		curwin->w_cursor.నిలువు = len;
	}

	haveto_redraw = TRUE;
	సమితి_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_వెనుక = FALSE;
	పంక్తి_breakcheck();
    }

    ఒకవేళ (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#నిర్వచితమైతే FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#అయితే_ముగింపు
    ఒకవేళ (!format_only && haveto_redraw)
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
    స్థిర పూర్ణాంకం
fmt_check_par(
    linenr_T	lnum,
    పూర్ణాంకం		*leader_len,
    char_u	**leader_flags,
    పూర్ణాంకం		do_comments)
{
    char_u	*flags = ఖాళీ;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ఒకవేళ (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    లేకపోతే
	*leader_len = 0;

    ఒకవేళ (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	అయితే (*flags && *flags != ':' && *flags != COM_చివర)
	    ++flags;
    }

    తిరిగి (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_చివర)
	    || ప్రారంభం_PS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    స్థిర పూర్ణాంకం
చివర్లు_లోపల_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ఒకవేళ (*s == NUL)
	తిరిగి FALSE;
    l = ml_get_len(lnum) - 1;
    తిరిగి VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    స్థిర పూర్ణాంకం
same_leader(
    linenr_T lnum,
    పూర్ణాంకం	    leader1_len,
    char_u  *leader1_flags,
    పూర్ణాంకం	    leader2_len,
    char_u  *leader2_flags)
{
    పూర్ణాంకం	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ఒకవేళ (leader1_len == 0)
	తిరిగి (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ఒకవేళ (leader1_flags != ఖాళీ)
    {
	char_u	*p;

	కొరకు (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ఒకవేళ (*p == COM_మొదటి)
		తిరిగి (leader2_len == 0);
	    ఒకవేళ (*p == COM_చివర)
		తిరిగి FALSE;
	    ఒకవేళ (*p == COM_ప్రారంభం)
	    {
		పూర్ణాంకం పంక్తి_len = ml_get_len(lnum);
		ఒకవేళ (పంక్తి_len <= leader1_len)
		    తిరిగి FALSE;
		ఒకవేళ (leader2_flags == ఖాళీ || leader2_len == 0)
		    తిరిగి FALSE;
		కొరకు (p = leader2_flags; *p && *p != ':'; ++p)
		    ఒకవేళ (*p == COM_మధ్య)
			తిరిగి TRUE;
		తిరిగి FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ఒకవేళ (line1 != ఖాళీ)
    {
	char_u  *line2;

	కొరకు (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	కొరకు (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ఒకవేళ (!VIM_ISWHITE(line2[idx2]))
	    {
		ఒకవేళ (line1[idx1++] != line2[idx2])
		    ఆపు;
	    }
	    లేకపోతే
		అయితే (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    తిరిగి (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    స్థిర పూర్ణాంకం
paragraph_ప్రారంభం(linenr_T lnum)
{
    char_u	*p;
    పూర్ణాంకం		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = ఖాళీ;	// flags for leader of current line
    పూర్ణాంకం		తదుపరి_leader_len;	// leader len of next line
    char_u	*తదుపరి_leader_flags;	// flags for leader of next line
    పూర్ణాంకం		do_comments;		// format comments

    ఒకవేళ (lnum <= 1)
	తిరిగి TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ఒకవేళ (*p == NUL)
	తిరిగి TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ఒకవేళ (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	తిరిగి TRUE;		// after non-paragraph line

    ఒకవేళ (fmt_check_par(lnum, &తదుపరి_leader_len, &తదుపరి_leader_flags, do_comments))
	తిరిగి TRUE;		// "lnum" is not a paragraph line

    ఒకవేళ (has_format_option(FO_WHITE_PAR) && !చివర్లు_లోపల_white(lnum - 1))
	తిరిగి TRUE;		// missing trailing space in previous line.

    ఒకవేళ (has_format_option(FO_Q_సంఖ్య) && (get_సంఖ్య_indent(lnum) > 0))
	తిరిగి TRUE;		// numbered item starts in "lnum".

    ఒకవేళ (!same_leader(lnum - 1, leader_len, leader_flags,
					  తదుపరి_leader_len, తదుపరి_leader_flags))
	తిరిగి TRUE;		// change of comment leader.

    తిరిగి FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    శూన్య
auto_format(
    పూర్ణాంకం		trailblank,	// when TRUE also format with trailing blank
    పూర్ణాంకం		prev_పంక్తి)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    పూర్ణాంకం		wasatend;

    ఒకవేళ (!has_format_option(FO_AUTO))
	తిరిగి;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.నిలువు == ml_get_curline_len());
    ఒకవేళ (*old != NUL && !trailblank && wasatend)
    {
	పూర్ణాంకం cc;

	dec_cursor();
	cc = gchar_cursor();
	ఒకవేళ (!WHITECHAR(cc) && curwin->w_cursor.నిలువు > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ఒకవేళ (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    తిరిగి;
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
    ఒకవేళ (*old != NUL && !trailblank && !wasatend && pos.నిలువు > 0
	    && (State & బహుళకం_చొప్పించు))
    {
	char_u *పంక్తి = ml_get_curline();
	ఒకవేళ (WHITECHAR(పంక్తి[pos.నిలువు - 1]))
	{
	    curwin->w_cursor = pos;
	    తిరిగి;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ఒకవేళ (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, ఖాళీ, FALSE, TRUE) == 0)
	తిరిగి;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ఒకవేళ (prev_పంక్తి && !paragraph_ప్రారంభం(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ఒకవేళ (u_save_cursor() == FAIL)
	    తిరిగి;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_పంక్తులు((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ఒకవేళ (curwin->w_cursor.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	coladvance((colnr_T)MAXCOL);
    }
    లేకపోతే
	check_cursor_నిలువు();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ఒకవేళ (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ఒకవేళ (curwin->w_cursor.నిలువు == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ఒకవేళ (pnew == ఖాళీ)
		తిరిగి;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_కూడు_space = TRUE;
	}
	లేకపోతే
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
    శూన్య
check_auto_format(
    పూర్ణాంకం		చివర_చొప్పించు)	    // TRUE when ending Insert mode
{
    పూర్ణాంకం		c = ' ';
    పూర్ణాంకం		cc;

    ఒకవేళ (!did_కూడు_space)
	తిరిగి;

    cc = gchar_cursor();
    ఒకవేళ (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_కూడు_space = FALSE;
    లేకపోతే
    {
	ఒకవేళ (!చివర_చొప్పించు)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ఒకవేళ (c != NUL)
	{
	    // The space is no longer at the end of the line, delete it.
	    del_char(FALSE);
	    did_కూడు_space = FALSE;
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
    పూర్ణాంకం
comp_textwidth(
    పూర్ణాంకం		ff)	// force formatting (for "gq" command)
{
    పూర్ణాంకం		textwidth;

    textwidth = curbuf->b_p_tw;
    ఒకవేళ (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_వెడల్పు - curbuf->b_p_wm;
	ఒకవేళ (curbuf == cmdwin_buf)
	    textwidth -= 1;
#నిర్వచితమైతే FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
	ఒకవేళ (signcolumn_on(curwin))
	    textwidth -= 1;
#అయితే_ముగింపు
	ఒకవేళ (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ఒకవేళ (textwidth < 0)
	textwidth = 0;
    ఒకవేళ (ff && textwidth == 0)
    {
	textwidth = curwin->w_వెడల్పు - 1;
	ఒకవేళ (textwidth > 79)
	    textwidth = 79;
    }
    తిరిగి textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    శూన్య
op_format(
    oparg_T	*oap,
    పూర్ణాంకం		keep_cursor)		// keep cursor on same text char
{
    దీర్ఘ	old_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_ప్రారంభం;

    ఒకవేళ (u_save((linenr_T)(oap->ప్రారంభం.lnum - 1),
				       (linenr_T)(oap->చివర.lnum + 1)) == FAIL)
	తిరిగి;
    curwin->w_cursor = oap->ప్రారంభం;

    ఒకవేళ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_ప్రారంభం = oap->ప్రారంభం;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ఒకవేళ (keep_cursor)
	saved_cursor = oap->cursor_ప్రారంభం;

    format_పంక్తులు(oap->పంక్తి_లెక్క, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ఒకవేళ (oap->చివర_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క - old_పంక్తి_లెక్క;
    msgmore(old_పంక్తి_లెక్క);

    ఒకవేళ ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_చివర = curwin->w_cursor;

    ఒకవేళ (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ఒకవేళ (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ఒకవేళ (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_పంక్తి_లెక్క;
		లేకపోతే
		    wp->w_old_visual_lnum += old_పంక్తి_లెక్క;
	    }
	}
    }
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    శూన్య
op_formatexpr(oparg_T *oap)
{
    ఒకవేళ (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ఒకవేళ (fex_format(oap->ప్రారంభం.lnum, oap->పంక్తి_లెక్క, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    పూర్ణాంకం
fex_format(
    linenr_T	lnum,
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		c)	// character to be inserted
{
    పూర్ణాంకం		use_sandbox = was_సమితి_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    పూర్ణాంకం		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    సమితి_vim_var_nr(VV_LNUM, lnum);
    సమితి_vim_var_nr(VV_లెక్క, లెక్క);
    సమితి_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ఒకవేళ (fex == ఖాళీ)
	తిరిగి 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ఒకవేళ (use_sandbox)
	++sandbox;
    r = (పూర్ణాంకం)eval_గమ్యం_సంఖ్య(fex, TRUE);
    ఒకవేళ (use_sandbox)
	--sandbox;

    సమితి_vim_var_string(VV_CHAR, ఖాళీ, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    తిరిగి r;
}
#అయితే_ముగింపు

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    శూన్య
format_పంక్తులు(
    linenr_T	పంక్తి_లెక్క,
    పూర్ణాంకం		avoid_fex)		// don't use 'formatexpr'
{
    పూర్ణాంకం		max_len;
    పూర్ణాంకం		is_not_par;		// current line not part of parag.
    పూర్ణాంకం		తదుపరి_is_not_par;	// next line not part of paragraph
    పూర్ణాంకం		is_చివర_par;		// at end of paragraph
    పూర్ణాంకం		prev_is_చివర_par = FALSE;// prev. line not part of parag.
    పూర్ణాంకం		తదుపరి_is_ప్రారంభం_par = FALSE;
    పూర్ణాంకం		leader_len = 0;		// leader len of current line
    పూర్ణాంకం		తదుపరి_leader_len;	// leader len of next line
    char_u	*leader_flags = ఖాళీ;	// flags for leader of current line
    char_u	*తదుపరి_leader_flags = ఖాళీ; // flags for leader of next line
    పూర్ణాంకం		do_comments;		// format comments
    పూర్ణాంకం		do_comments_list = 0;	// format comments with 'n' or '2'
    పూర్ణాంకం		advance = TRUE;
    పూర్ణాంకం		రెండవ_indent = -1;	// indent for second line (comment
					// aware)
    పూర్ణాంకం		do_రెండవ_indent;
    పూర్ణాంకం		do_సంఖ్య_indent;
    పూర్ణాంకం		do_trail_white;
    పూర్ణాంకం		మొదటి_par_పంక్తి = TRUE;
    పూర్ణాంకం		smd_save;
    దీర్ఘ	లెక్క;
    పూర్ణాంకం		need_సమితి_indent = TRUE;	// set indent of next paragraph
    linenr_T	మొదటి_పంక్తి = curwin->w_cursor.lnum;
    పూర్ణాంకం		force_format = FALSE;
    పూర్ణాంకం		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_రెండవ_indent = has_format_option(FO_Q_రెండవ);
    do_సంఖ్య_indent = has_format_option(FO_Q_సంఖ్య);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ఒకవేళ (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    లేకపోతే
	is_not_par = TRUE;
    తదుపరి_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &తదుపరి_leader_len, &తదుపరి_leader_flags, do_comments);
    is_చివర_par = (is_not_par || తదుపరి_is_not_par);
    ఒకవేళ (!is_చివర_par && do_trail_white)
	is_చివర_par = !చివర్లు_లోపల_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    కొరకు (లెక్క = పంక్తి_లెక్క; లెక్క != 0 && !got_int; --లెక్క)
    {
	// Advance to next paragraph.
	ఒకవేళ (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_చివర_par = is_చివర_par;
	    is_not_par = తదుపరి_is_not_par;
	    leader_len = తదుపరి_leader_len;
	    leader_flags = తదుపరి_leader_flags;
	}

	// The last line to be formatted.
	ఒకవేళ (లెక్క == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    తదుపరి_is_not_par = TRUE;
	    తదుపరి_leader_len = 0;
	    తదుపరి_leader_flags = ఖాళీ;
	}
	లేకపోతే
	{
	    తదుపరి_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &తదుపరి_leader_len, &తదుపరి_leader_flags, do_comments);
	    ఒకవేళ (do_సంఖ్య_indent)
		తదుపరి_is_ప్రారంభం_par =
			   (get_సంఖ్య_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_చివర_par = (is_not_par || తదుపరి_is_not_par || తదుపరి_is_ప్రారంభం_par);
	ఒకవేళ (!is_చివర_par && do_trail_white)
	    is_చివర_par = !చివర్లు_లోపల_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ఒకవేళ (is_not_par)
	{
	    ఒకవేళ (పంక్తి_లెక్క < 0)
		ఆపు;
	}
	లేకపోతే
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ఒకవేళ (మొదటి_par_పంక్తి
		    && (do_రెండవ_indent || do_సంఖ్య_indent)
		    && prev_is_చివర_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
	    {
		ఒకవేళ (do_రెండవ_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ఒకవేళ (leader_len == 0 && తదుపరి_leader_len == 0)
		    {
			// no comment found
			రెండవ_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    లేకపోతే
		    {
			రెండవ_indent = తదుపరి_leader_len;
			do_comments_list = 1;
		    }
		}
		లేకపోతే ఒకవేళ (do_సంఖ్య_indent)
		{
		    ఒకవేళ (leader_len == 0 && తదుపరి_leader_len == 0)
		    {
			// no comment found
			రెండవ_indent =
				     get_సంఖ్య_indent(curwin->w_cursor.lnum);
		    }
		    లేకపోతే
		    {
			// get_number_indent() is now "comment aware"...
			రెండవ_indent =
				     get_సంఖ్య_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ఒకవేళ (curwin->w_cursor.lnum >= curbuf->b_ml.ml_పంక్తి_లెక్క
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   తదుపరి_leader_len, తదుపరి_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ఒకవేళ (తదుపరి_leader_flags == ఖాళీ
			|| STRNCMP(తదుపరి_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_చివర_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ఒకవేళ (is_చివర_par || force_format)
	    {
		ఒకవేళ (need_సమితి_indent)
		{
		    పూర్ణాంకం		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ఒకవేళ (curwin->w_cursor.lnum == మొదటి_పంక్తి)
			indent = get_indent();
		    లేకపోతే ఒకవేళ (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    లేకపోతే
		    {
			ఒకవేళ (cindent_on())
			{
			    indent =
#నిర్వచితమైతే FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_సమాసం_indent() :
#అయితే_ముగింపు
				 get_c_indent();
			}
			లేకపోతే
			    indent = get_indent();
		    }
		    (శూన్య)సమితి_indent(indent, SIN_మారింది);
		}

		// put cursor on last non-space
		State = బహుళకం_సాధారణ;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		అయితే (curwin->w_cursor.నిలువు && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = బహుళకం_చొప్పించు;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;

		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), రెండవ_indent);

		State = old_State;
		p_smd = smd_save;
		// Cursor and mouse shape shapes may have been updated (e.g. by
		// :normal) in insertchar(), so they need to be updated here.
#నిర్వచితమైతే CURSOR_SHAPE
		ui_cursor_shape();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSESHAPE
		update_mouseshape(-1);
#అయితే_ముగింపు

		రెండవ_indent = -1;
		// at end of par.: need to set indent of next par.
		need_సమితి_indent = is_చివర_par;
		ఒకవేళ (is_చివర_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ఒకవేళ (పంక్తి_లెక్క < 0)
			ఆపు;
		    మొదటి_par_పంక్తి = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ఒకవేళ (!is_చివర_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.నిలువు = 0;
		ఒకవేళ (పంక్తి_లెక్క < 0 && u_save_cursor() == FAIL)
		    ఆపు;
		ఒకవేళ (తదుపరి_leader_len > 0)
		{
		    (శూన్య)del_బైట్లు((దీర్ఘ)తదుపరి_leader_len, FALSE, FALSE);
		    mark_నిలువు_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (దీర్ఘ)-తదుపరి_leader_len, 0);
		}
		లేకపోతే ఒకవేళ (రెండవ_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    పూర్ణాంకం indent = getwhitecols_curline();

		    ఒకవేళ (indent > 0)
		    {
			(శూన్య)del_బైట్లు(indent, FALSE, FALSE);
			mark_నిలువు_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (దీర్ఘ)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ఒకవేళ (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    ఆపు;
		}
		మొదటి_par_పంక్తి = FALSE;
		// If the line is getting long, format it next time
		ఒకవేళ (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		లేకపోతే
		    force_format = FALSE;
	    }
	}
	పంక్తి_breakcheck();
    }
}
