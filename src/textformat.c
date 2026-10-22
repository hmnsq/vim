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

#شامل "vim.h"

ساکن صحیح	did_جوڑو_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#تعریف_کرو WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    صحیح
has_format_option(صحیح x)
{
    اگر (p_paste)
	واپس FALSE;
    واپس (vim_strchr(curbuf->b_p_fo, x) != تہی);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    خالی
اندرونی_format(
    صحیح		textwidth,
    صحیح		دوسرا_indent,
    صحیح		flags,
    صحیح		format_only,
    صحیح		c) // character to be inserted (can be NUL)
{
    صحیح		cc;
    صحیح		skip_pos;
    صحیح		save_char = NUL;
    صحیح		haveto_redraw = FALSE;
    صحیح		fo_ins_blank = has_format_option(FO_INS_BLANK);
    صحیح		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    صحیح		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    صحیح		fo_white_par = has_format_option(FO_WHITE_PAR);
    صحیح		پہلا_سطر = TRUE;
    colnr_T	leader_len;
    صحیح		no_leader = FALSE;
    صحیح		do_comments = (flags & INSCHAR_DO_COM);
    صحیح		محفوظ_tw = trim_مقصد_int(8 * (vimlong_T)textwidth);
#اگر_تعریف_شدہ FEAT_LINEBREAK
    صحیح		has_lbr = curwin->w_p_lbr;
    صحیح		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#اگر_ختم

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    اگر (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	اگر (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    جب (!got_int)
    {
	صحیح	startcol;		// Cursor column at entry
	صحیح	wantcol;		// column at textwidth border
	صحیح	foundcol;		// column for start of spaces
	صحیح	انتہا_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	صحیح	orig_ستون = 0;
	char_u	*saved_متن = تہی;
	colnr_T	ستون;
	colnr_T	انتہا_ستون;
	صحیح	wcc;			// counter for whitespace chars
	صحیح	did_do_comment = FALSE;
	صحیح	پہلا_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	اگر (curwin->w_cursor.ستون < محفوظ_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    اگر (virtcol <= (colnr_T)textwidth)
		توڑو;
	}

	اگر (no_leader)
	    do_comments = FALSE;
	ورنہ اگر (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	اگر (do_comments)
	{
	    char_u *سطر = ml_get_curline();

	    leader_len = get_leader_len(سطر, تہی, FALSE, TRUE);
	    اگر (leader_len == 0 && curbuf->b_p_cin)
	    {
		صحیح		comment_آغاز;

		// Check for a line comment after code.
		comment_آغاز = check_linecomment(سطر);
		اگر (comment_آغاز != MAXCOL)
		{
		    leader_len = get_leader_len(
				      سطر + comment_آغاز, تہی, FALSE, TRUE);
		    اگر (leader_len != 0)
			leader_len += comment_آغاز;
		}
	    }
	}
	ورنہ
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	اگر (leader_len == 0)
	    no_leader = TRUE;
	اگر (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    توڑو;
	اگر ((startcol = curwin->w_cursor.ستون) == 0)
	    توڑو;

	// find column of textwidth border
	coladvance((colnr_T)textwidth);
	wantcol = curwin->w_cursor.ستون;

	// If startcol is large (a long line), formatting takes too much
	// time. The algorithm is O(n^2), it walks from the end of the
	// line to textwidth border every time for each line break.
	//
	// Ceil to 8 * textwidth to optimize.
	curwin->w_cursor.ستون = startcol < محفوظ_tw ? startcol : محفوظ_tw;

	foundcol = 0;
	skip_pos = 0;
	پہلا_pass = TRUE;

	// Find position to break at.
	// Stop at first entered white when 'formatoptions' has 'v'
	جب ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.ستون >= Insstart.ستون)
	{
	    اگر (پہلا_pass && c != NUL)
	    {
		cc = c;
		پہلا_pass = FALSE;
	    }
	    ورنہ
		cc = gchar_cursor();
	    اگر (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		انتہا_ستون = curwin->w_cursor.ستون;

		// find start of sequence of blanks
		wcc = 0;
		جب (curwin->w_cursor.ستون > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    اگر (wcc < 2)
			wcc++;
		}
		اگر (curwin->w_cursor.ستون == 0 && WHITECHAR(cc))
		    توڑو;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		اگر (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    جاری;

		// Don't break until after the comment leader
		اگر (curwin->w_cursor.ستون < leader_len)
		    توڑو;
		اگر (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    اگر (curwin->w_cursor.ستون == 0)
			توڑو;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    اگر (curwin->w_cursor.ستون <= leader_len)
			توڑو;
		    ستون = curwin->w_cursor.ستون;
		    dec_cursor();
		    cc = gchar_cursor();

		    اگر (WHITECHAR(cc))
			جاری;	// one-letter, continue
		    curwin->w_cursor.ستون = ستون;
		}

		inc_cursor();

		انتہا_foundcol = انتہا_ستون + 1;
		foundcol = curwin->w_cursor.ستون;
		اگر (curwin->w_cursor.ستون <= (colnr_T)wantcol)
		    توڑو;
	    }
	    ورنہ اگر ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		صحیح ncc;
		صحیح allow_break;

		// Break after or before a multi-byte character.
		اگر (curwin->w_cursor.ستون != startcol)
		{
		    // Don't break until after the comment leader
		    اگر (curwin->w_cursor.ستون < leader_len)
			توڑو;
		    ستون = curwin->w_cursor.ستون;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    اگر (curwin->w_cursor.ستون != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.ستون;
			انتہا_foundcol = foundcol;
			اگر (curwin->w_cursor.ستون <= (colnr_T)wantcol)
			    توڑو;
		    }
		    curwin->w_cursor.ستون = ستون;
		}

		اگر (curwin->w_cursor.ستون == 0)
		    توڑو;

		ncc = cc;
		ستون = curwin->w_cursor.ستون;

		dec_cursor();
		cc = gchar_cursor();

		اگر (WHITECHAR(cc))
		    جاری;		// break with space
		// Don't break until after the comment leader.
		اگر (curwin->w_cursor.ستون < leader_len)
		    توڑو;

		curwin->w_cursor.ستون = ستون;
		skip_pos = curwin->w_cursor.ستون;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		اگر (allow_break)
		{
		    foundcol = curwin->w_cursor.ستون;
		    انتہا_foundcol = foundcol;
		}
		اگر (curwin->w_cursor.ستون <= (colnr_T)wantcol)
		{
		    صحیح ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    اگر (allow_break)
			توڑو;
		    اگر (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			اگر (curwin->w_cursor.ستون == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    انتہا_foundcol = foundcol = 0;
			    توڑو;
			}

			// Neither cc nor ncc is NUL if we are here, so
			// it's safe to inc_cursor.
			ستون = curwin->w_cursor.ستون;

			inc_cursor();
			cc  = ncc;
			ncc = gchar_cursor();
			// handle insert
			ncc = (ncc != NUL) ? ncc : c;

			allow_break =
				(enc_utf8 && utf_allow_break(cc, ncc))
				|| enc_dbcs;

			اگر (allow_break)
			{
			    // Break only when we are not at end of line.
			    انتہا_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.ستون;
			    توڑو;
			}
			curwin->w_cursor.ستون = ستون;
		    }
		}
	    }
	    اگر (curwin->w_cursor.ستون == 0)
		توڑو;
	    dec_cursor();
	}

	اگر (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.ستون = startcol;
	    توڑو;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	اگر (State & VREPLACE_FLAG)
	    orig_ستون = startcol;	// Will start backspacing from here
	ورنہ
	    replace_انحراف = startcol - انتہا_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.ستون = foundcol;
	جب ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.ستون < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.ستون;
	اگر (startcol < 0)
	    startcol = 0;

	اگر (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_متن = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.ستون = orig_ستون;
	    اگر (saved_متن == تہی)
		توڑو;	// Can't do it, out of memory
	    saved_متن[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    اگر (!fo_white_par)
		backspace_until_عمود(foundcol);
	}
	ورنہ
	{
	    // put cursor after pos. to break line
	    اگر (!fo_white_par)
		curwin->w_cursor.ستون = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_سطر(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? دوسرا_indent : old_indent),
		&did_do_comment);
	اگر (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	اگر (did_do_comment)
	    no_leader = FALSE;

	replace_انحراف = 0;
	اگر (پہلا_سطر)
	{
	    اگر (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		اگر (دوسرا_indent < 0 && has_format_option(FO_Q_نمبر))
		    دوسرا_indent =
				 get_نمبر_indent(curwin->w_cursor.lnum - 1);
		اگر (دوسرا_indent >= 0)
		{
		    اگر (State & VREPLACE_FLAG)
			change_indent(INDENT_سیٹ, دوسرا_indent,
							    FALSE, NUL, TRUE);
		    ورنہ
			اگر (leader_len > 0 && دوسرا_indent - leader_len > 0)
		    {
			صحیح i;
			صحیح padding = دوسرا_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			برائے (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    ورنہ
		    {
			(خالی)سیٹ_indent(دوسرا_indent, SIN_بدلا);
		    }
		}
	    }
	    پہلا_سطر = FALSE;
	}

	اگر (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_بائٹیں(saved_متن);
	    vim_free(saved_متن);
	}
	ورنہ
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.ستون += startcol;
	    len = ml_get_curline_len();
	    اگر (curwin->w_cursor.ستون > len)
		curwin->w_cursor.ستون = len;
	}

	haveto_redraw = TRUE;
	سیٹ_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_عقب = FALSE;
	سطر_breakcheck();
    }

    اگر (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#اگر_تعریف_شدہ FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#اگر_ختم
    اگر (!format_only && haveto_redraw)
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
    ساکن صحیح
fmt_check_par(
    linenr_T	lnum,
    صحیح		*leader_len,
    char_u	**leader_flags,
    صحیح		do_comments)
{
    char_u	*flags = تہی;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    اگر (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    ورنہ
	*leader_len = 0;

    اگر (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	جب (*flags && *flags != ':' && *flags != COM_انتہا)
	    ++flags;
    }

    واپس (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_انتہا)
	    || آغاز_PS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    ساکن صحیح
سرے_اندر_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    اگر (*s == NUL)
	واپس FALSE;
    l = ml_get_len(lnum) - 1;
    واپس VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    ساکن صحیح
same_leader(
    linenr_T lnum,
    صحیح	    leader1_len,
    char_u  *leader1_flags,
    صحیح	    leader2_len,
    char_u  *leader2_flags)
{
    صحیح	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    اگر (leader1_len == 0)
	واپس (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    اگر (leader1_flags != تہی)
    {
	char_u	*p;

	برائے (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    اگر (*p == COM_پہلا)
		واپس (leader2_len == 0);
	    اگر (*p == COM_انتہا)
		واپس FALSE;
	    اگر (*p == COM_آغاز)
	    {
		صحیح سطر_len = ml_get_len(lnum);
		اگر (سطر_len <= leader1_len)
		    واپس FALSE;
		اگر (leader2_flags == تہی || leader2_len == 0)
		    واپس FALSE;
		برائے (p = leader2_flags; *p && *p != ':'; ++p)
		    اگر (*p == COM_درمیان)
			واپس TRUE;
		واپس FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    اگر (line1 != تہی)
    {
	char_u  *line2;

	برائے (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	برائے (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    اگر (!VIM_ISWHITE(line2[idx2]))
	    {
		اگر (line1[idx1++] != line2[idx2])
		    توڑو;
	    }
	    ورنہ
		جب (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    واپس (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    ساکن صحیح
paragraph_آغاز(linenr_T lnum)
{
    char_u	*p;
    صحیح		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = تہی;	// flags for leader of current line
    صحیح		اگلا_leader_len;	// leader len of next line
    char_u	*اگلا_leader_flags;	// flags for leader of next line
    صحیح		do_comments;		// format comments

    اگر (lnum <= 1)
	واپس TRUE;		// start of the file

    p = ml_get(lnum - 1);
    اگر (*p == NUL)
	واپس TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    اگر (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	واپس TRUE;		// after non-paragraph line

    اگر (fmt_check_par(lnum, &اگلا_leader_len, &اگلا_leader_flags, do_comments))
	واپس TRUE;		// "lnum" is not a paragraph line

    اگر (has_format_option(FO_WHITE_PAR) && !سرے_اندر_white(lnum - 1))
	واپس TRUE;		// missing trailing space in previous line.

    اگر (has_format_option(FO_Q_نمبر) && (get_نمبر_indent(lnum) > 0))
	واپس TRUE;		// numbered item starts in "lnum".

    اگر (!same_leader(lnum - 1, leader_len, leader_flags,
					  اگلا_leader_len, اگلا_leader_flags))
	واپس TRUE;		// change of comment leader.

    واپس FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    خالی
auto_format(
    صحیح		trailblank,	// when TRUE also format with trailing blank
    صحیح		پچھلا_سطر)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    صحیح		wasatend;

    اگر (!has_format_option(FO_AUTO))
	واپس;

    pos = curwin->w_cursor;
    old = ml_get_curline();

    // may remove added space
    check_auto_format(FALSE);

    // Don't format in Insert mode when the cursor is on a trailing blank, the
    // user might insert normal text next.  Also skip formatting when "1" is
    // in 'formatoptions' and there is a single character before the cursor.
    // Otherwise the line would be broken and when typing another non-white
    // next they are not joined back together.
    wasatend = (pos.ستون == ml_get_curline_len());
    اگر (*old != NUL && !trailblank && wasatend)
    {
	صحیح cc;

	dec_cursor();
	cc = gchar_cursor();
	اگر (!WHITECHAR(cc) && curwin->w_cursor.ستون > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	اگر (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    واپس;
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
    اگر (*old != NUL && !trailblank && !wasatend && pos.ستون > 0
	    && (State & منوال_داخل_کرو))
    {
	char_u *سطر = ml_get_curline();
	اگر (WHITECHAR(سطر[pos.ستون - 1]))
	{
	    curwin->w_cursor = pos;
	    واپس;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    اگر (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, تہی, FALSE, TRUE) == 0)
	واپس;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    اگر (پچھلا_سطر && !paragraph_آغاز(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	اگر (u_save_cursor() == FAIL)
	    واپس;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_سطریں((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	coladvance((colnr_T)MAXCOL);
    }
    ورنہ
	check_cursor_ستون();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    اگر (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	اگر (curwin->w_cursor.ستون == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    اگر (pnew == تہی)
		واپس;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_جوڑو_space = TRUE;
	}
	ورنہ
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
    خالی
check_auto_format(
    صحیح		انتہا_داخل_کرو)	    // TRUE when ending Insert mode
{
    صحیح		c = ' ';
    صحیح		cc;

    اگر (!did_جوڑو_space)
	واپس;

    cc = gchar_cursor();
    اگر (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_جوڑو_space = FALSE;
    ورنہ
    {
	اگر (!انتہا_داخل_کرو)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	اگر (c != NUL)
	{
	    // The space is no longer at the end of the line, delete it.
	    del_char(FALSE);
	    did_جوڑو_space = FALSE;
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
    صحیح
comp_textwidth(
    صحیح		ff)	// force formatting (for "gq" command)
{
    صحیح		textwidth;

    textwidth = curbuf->b_p_tw;
    اگر (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_چوڑائی - curbuf->b_p_wm;
	اگر (curbuf == cmdwin_buf)
	    textwidth -= 1;
#اگر_تعریف_شدہ FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SIGNS
	اگر (signcolumn_on(curwin))
	    textwidth -= 1;
#اگر_ختم
	اگر (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    اگر (textwidth < 0)
	textwidth = 0;
    اگر (ff && textwidth == 0)
    {
	textwidth = curwin->w_چوڑائی - 1;
	اگر (textwidth > 79)
	    textwidth = 79;
    }
    واپس textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    خالی
op_format(
    oparg_T	*oap,
    صحیح		keep_cursor)		// keep cursor on same text char
{
    طویل	old_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_آغاز;

    اگر (u_save((linenr_T)(oap->آغاز.lnum - 1),
				       (linenr_T)(oap->انتہا.lnum + 1)) == FAIL)
	واپس;
    curwin->w_cursor = oap->آغاز;

    اگر (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_آغاز = oap->آغاز;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    اگر (keep_cursor)
	saved_cursor = oap->cursor_آغاز;

    format_سطریں(oap->سطر_گنتی, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    اگر (oap->انتہا_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی - old_سطر_گنتی;
    msgmore(old_سطر_گنتی);

    اگر ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_انتہا = curwin->w_cursor;

    اگر (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    اگر (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		اگر (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_سطر_گنتی;
		ورنہ
		    wp->w_old_visual_lnum += old_سطر_گنتی;
	    }
	}
    }
}

#اگر defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    خالی
op_formatexpr(oparg_T *oap)
{
    اگر (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    اگر (fex_format(oap->آغاز.lnum, oap->سطر_گنتی, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    صحیح
fex_format(
    linenr_T	lnum,
    طویل	گنتی,
    صحیح		c)	// character to be inserted
{
    صحیح		use_sandbox = was_سیٹ_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    صحیح		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    سیٹ_vim_var_nr(VV_LNUM, lnum);
    سیٹ_vim_var_nr(VV_گنتی, گنتی);
    سیٹ_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    اگر (fex == تہی)
	واپس 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    اگر (use_sandbox)
	++sandbox;
    r = (صحیح)eval_مقصد_نمبر(fex, TRUE);
    اگر (use_sandbox)
	--sandbox;

    سیٹ_vim_var_string(VV_CHAR, تہی, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    واپس r;
}
#اگر_ختم

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    خالی
format_سطریں(
    linenr_T	سطر_گنتی,
    صحیح		avoid_fex)		// don't use 'formatexpr'
{
    صحیح		max_len;
    صحیح		is_not_par;		// current line not part of parag.
    صحیح		اگلا_is_not_par;	// next line not part of paragraph
    صحیح		is_انتہا_par;		// at end of paragraph
    صحیح		پچھلا_is_انتہا_par = FALSE;// prev. line not part of parag.
    صحیح		اگلا_is_آغاز_par = FALSE;
    صحیح		leader_len = 0;		// leader len of current line
    صحیح		اگلا_leader_len;	// leader len of next line
    char_u	*leader_flags = تہی;	// flags for leader of current line
    char_u	*اگلا_leader_flags = تہی; // flags for leader of next line
    صحیح		do_comments;		// format comments
    صحیح		do_comments_list = 0;	// format comments with 'n' or '2'
    صحیح		advance = TRUE;
    صحیح		دوسرا_indent = -1;	// indent for second line (comment
					// aware)
    صحیح		do_دوسرا_indent;
    صحیح		do_نمبر_indent;
    صحیح		do_trail_white;
    صحیح		پہلا_par_سطر = TRUE;
    صحیح		smd_save;
    طویل	گنتی;
    صحیح		need_سیٹ_indent = TRUE;	// set indent of next paragraph
    linenr_T	پہلا_سطر = curwin->w_cursor.lnum;
    صحیح		force_format = FALSE;
    صحیح		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_دوسرا_indent = has_format_option(FO_Q_دوسرا);
    do_نمبر_indent = has_format_option(FO_Q_نمبر);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    اگر (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    ورنہ
	is_not_par = TRUE;
    اگلا_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &اگلا_leader_len, &اگلا_leader_flags, do_comments);
    is_انتہا_par = (is_not_par || اگلا_is_not_par);
    اگر (!is_انتہا_par && do_trail_white)
	is_انتہا_par = !سرے_اندر_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    برائے (گنتی = سطر_گنتی; گنتی != 0 && !got_int; --گنتی)
    {
	// Advance to next paragraph.
	اگر (advance)
	{
	    curwin->w_cursor.lnum++;
	    پچھلا_is_انتہا_par = is_انتہا_par;
	    is_not_par = اگلا_is_not_par;
	    leader_len = اگلا_leader_len;
	    leader_flags = اگلا_leader_flags;
	}

	// The last line to be formatted.
	اگر (گنتی == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_سطر_گنتی)
	{
	    اگلا_is_not_par = TRUE;
	    اگلا_leader_len = 0;
	    اگلا_leader_flags = تہی;
	}
	ورنہ
	{
	    اگلا_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &اگلا_leader_len, &اگلا_leader_flags, do_comments);
	    اگر (do_نمبر_indent)
		اگلا_is_آغاز_par =
			   (get_نمبر_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_انتہا_par = (is_not_par || اگلا_is_not_par || اگلا_is_آغاز_par);
	اگر (!is_انتہا_par && do_trail_white)
	    is_انتہا_par = !سرے_اندر_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	اگر (is_not_par)
	{
	    اگر (سطر_گنتی < 0)
		توڑو;
	}
	ورنہ
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    اگر (پہلا_par_سطر
		    && (do_دوسرا_indent || do_نمبر_indent)
		    && پچھلا_is_انتہا_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	    {
		اگر (do_دوسرا_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    اگر (leader_len == 0 && اگلا_leader_len == 0)
		    {
			// no comment found
			دوسرا_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    ورنہ
		    {
			دوسرا_indent = اگلا_leader_len;
			do_comments_list = 1;
		    }
		}
		ورنہ اگر (do_نمبر_indent)
		{
		    اگر (leader_len == 0 && اگلا_leader_len == 0)
		    {
			// no comment found
			دوسرا_indent =
				     get_نمبر_indent(curwin->w_cursor.lnum);
		    }
		    ورنہ
		    {
			// get_number_indent() is now "comment aware"...
			دوسرا_indent =
				     get_نمبر_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    اگر (curwin->w_cursor.lnum >= curbuf->b_ml.ml_سطر_گنتی
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   اگلا_leader_len, اگلا_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		اگر (اگلا_leader_flags == تہی
			|| STRNCMP(اگلا_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_انتہا_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    اگر (is_انتہا_par || force_format)
	    {
		اگر (need_سیٹ_indent)
		{
		    صحیح		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    اگر (curwin->w_cursor.lnum == پہلا_سطر)
			indent = get_indent();
		    ورنہ اگر (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    ورنہ
		    {
			اگر (cindent_on())
			{
			    indent =
#اگر_تعریف_شدہ FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_اظہاریہ_indent() :
#اگر_ختم
				 get_c_indent();
			}
			ورنہ
			    indent = get_indent();
		    }
		    (خالی)سیٹ_indent(indent, SIN_بدلا);
		}

		// put cursor on last non-space
		State = منوال_معمول;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		جب (curwin->w_cursor.ستون && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = منوال_داخل_کرو;	// for open_line()
		smd_save = p_smd;
		p_smd = FALSE;

		insertchar(NUL, INSCHAR_FORMAT
			+ (do_comments ? INSCHAR_DO_COM : 0)
			+ (do_comments && do_comments_list
						       ? INSCHAR_COM_LIST : 0)
			+ (avoid_fex ? INSCHAR_NO_FEX : 0), دوسرا_indent);

		State = old_State;
		p_smd = smd_save;
		// Cursor and mouse shape shapes may have been updated (e.g. by
		// :normal) in insertchar(), so they need to be updated here.
#اگر_تعریف_شدہ CURSOR_SHAPE
		ui_cursor_shape();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
		update_mouseshape(-1);
#اگر_ختم

		دوسرا_indent = -1;
		// at end of par.: need to set indent of next par.
		need_سیٹ_indent = is_انتہا_par;
		اگر (is_انتہا_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    اگر (سطر_گنتی < 0)
			توڑو;
		    پہلا_par_سطر = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    اگر (!is_انتہا_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.ستون = 0;
		اگر (سطر_گنتی < 0 && u_save_cursor() == FAIL)
		    توڑو;
		اگر (اگلا_leader_len > 0)
		{
		    (خالی)del_بائٹیں((طویل)اگلا_leader_len, FALSE, FALSE);
		    mark_ستون_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (طویل)-اگلا_leader_len, 0);
		}
		ورنہ اگر (دوسرا_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    صحیح indent = getwhitecols_curline();

		    اگر (indent > 0)
		    {
			(خالی)del_بائٹیں(indent, FALSE, FALSE);
			mark_ستون_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (طویل)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		اگر (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    توڑو;
		}
		پہلا_par_سطر = FALSE;
		// If the line is getting long, format it next time
		اگر (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		ورنہ
		    force_format = FALSE;
	    }
	}
	سطر_breakcheck();
    }
}
