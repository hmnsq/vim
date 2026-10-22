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

#include "vim.h"

static จำนวนเต็ม	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    จำนวนเต็ม
has_format_option(จำนวนเต็ม x)
{
    ถ้า (p_paste)
	คืน FALSE;
    คืน (vim_strchr(curbuf->b_p_fo, x) != NULL);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    void
internal_format(
    จำนวนเต็ม		textwidth,
    จำนวนเต็ม		second_indent,
    จำนวนเต็ม		flags,
    จำนวนเต็ม		format_only,
    จำนวนเต็ม		c) // character to be inserted (can be NUL)
{
    จำนวนเต็ม		cc;
    จำนวนเต็ม		skip_pos;
    จำนวนเต็ม		save_char = NUL;
    จำนวนเต็ม		haveto_redraw = FALSE;
    จำนวนเต็ม		fo_ins_blank = has_format_option(FO_INS_BLANK);
    จำนวนเต็ม		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    จำนวนเต็ม		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    จำนวนเต็ม		fo_white_par = has_format_option(FO_WHITE_PAR);
    จำนวนเต็ม		first_line = TRUE;
    colnr_T	leader_len;
    จำนวนเต็ม		no_leader = FALSE;
    จำนวนเต็ม		do_comments = (flags & INSCHAR_DO_COM);
    จำนวนเต็ม		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    จำนวนเต็ม		has_lbr = curwin->w_p_lbr;
    จำนวนเต็ม		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ถ้า (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ถ้า (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    ขณะ (!got_int)
    {
	จำนวนเต็ม	startcol;		// Cursor column at entry
	จำนวนเต็ม	wantcol;		// column at textwidth border
	จำนวนเต็ม	foundcol;		// column for start of spaces
	จำนวนเต็ม	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	จำนวนเต็ม	orig_col = 0;
	char_u	*saved_text = NULL;
	colnr_T	col;
	colnr_T	end_col;
	จำนวนเต็ม	wcc;			// counter for whitespace chars
	จำนวนเต็ม	did_do_comment = FALSE;
	จำนวนเต็ม	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ถ้า (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ถ้า (virtcol <= (colnr_T)textwidth)
		bha_nga;
	}

	ถ้า (no_leader)
	    do_comments = FALSE;
	มิฉะนั้น ถ้า (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ถ้า (do_comments)
	{
	    char_u *line = ml_get_curline();

	    leader_len = get_leader_len(line, NULL, FALSE, TRUE);
	    ถ้า (leader_len == 0 && curbuf->b_p_cin)
	    {
		จำนวนเต็ม		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(line);
		ถ้า (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      line + comment_start, NULL, FALSE, TRUE);
		    ถ้า (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	มิฉะนั้น
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ถ้า (leader_len == 0)
	    no_leader = TRUE;
	ถ้า (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    bha_nga;
	ถ้า ((startcol = curwin->w_cursor.col) == 0)
	    bha_nga;

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
	ขณะ ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    ถ้า (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    มิฉะนั้น
		cc = gchar_cursor();
	    ถ้า (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		ขณะ (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ถ้า (wcc < 2)
			wcc++;
		}
		ถ้า (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    bha_nga;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ถ้า (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    continue;

		// Don't break until after the comment leader
		ถ้า (curwin->w_cursor.col < leader_len)
		    bha_nga;
		ถ้า (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ถ้า (curwin->w_cursor.col == 0)
			bha_nga;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ถ้า (curwin->w_cursor.col <= leader_len)
			bha_nga;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    ถ้า (WHITECHAR(cc))
			continue;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		ถ้า (curwin->w_cursor.col <= (colnr_T)wantcol)
		    bha_nga;
	    }
	    มิฉะนั้น ถ้า ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		จำนวนเต็ม ncc;
		จำนวนเต็ม allow_break;

		// Break after or before a multi-byte character.
		ถ้า (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    ถ้า (curwin->w_cursor.col < leader_len)
			bha_nga;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ถ้า (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			ถ้า (curwin->w_cursor.col <= (colnr_T)wantcol)
			    bha_nga;
		    }
		    curwin->w_cursor.col = col;
		}

		ถ้า (curwin->w_cursor.col == 0)
		    bha_nga;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		ถ้า (WHITECHAR(cc))
		    continue;		// break with space
		// Don't break until after the comment leader.
		ถ้า (curwin->w_cursor.col < leader_len)
		    bha_nga;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ถ้า (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		ถ้า (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    จำนวนเต็ม ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ถ้า (allow_break)
			bha_nga;
		    ถ้า (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ถ้า (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    bha_nga;
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

			ถ้า (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    bha_nga;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    ถ้า (curwin->w_cursor.col == 0)
		bha_nga;
	    dec_cursor();
	}

	ถ้า (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    bha_nga;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ถ้า (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	มิฉะนั้น
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	ขณะ ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	ถ้า (startcol < 0)
	    startcol = 0;

	ถ้า (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    ถ้า (saved_text == NULL)
		bha_nga;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ถ้า (!fo_white_par)
		backspace_until_column(foundcol);
	}
	มิฉะนั้น
	{
	    // put cursor after pos. to break line
	    ถ้า (!fo_white_par)
		curwin->w_cursor.col = foundcol;
	}

	// Split the line just before the margin.
	// Only insert/delete lines, but don't really redraw the window.
	open_line(FORWARD, OPENLINE_DELSPACES + OPENLINE_MARKFIX
		+ (fo_white_par ? OPENLINE_KEEPTRAIL : 0)
		+ (do_comments ? OPENLINE_DO_COM : 0)
		+ OPENLINE_FORMAT
		+ ((flags & INSCHAR_COM_LIST) ? OPENLINE_COM_LIST : 0)
		, ((flags & INSCHAR_COM_LIST) ? second_indent : old_indent),
		&did_do_comment);
	ถ้า (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ถ้า (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	ถ้า (first_line)
	{
	    ถ้า (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ถ้า (second_indent < 0 && has_format_option(FO_Q_NUMBER))
		    second_indent =
				 get_number_indent(curwin->w_cursor.lnum - 1);
		ถ้า (second_indent >= 0)
		{
		    ถ้า (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    มิฉะนั้น
			ถ้า (leader_len > 0 && second_indent - leader_len > 0)
		    {
			จำนวนเต็ม i;
			จำนวนเต็ม padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			สำหรับ (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    มิฉะนั้น
		    {
			(void)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_line = FALSE;
	}

	ถ้า (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	มิฉะนั้น
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    ถ้า (curwin->w_cursor.col > len)
		curwin->w_cursor.col = len;
	}

	haveto_redraw = TRUE;
	set_can_cindent(TRUE);
	// moved the cursor, don't autoindent or cindent now
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
	can_si_back = FALSE;
	line_breakcheck();
    }

    ถ้า (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    ถ้า (!format_only && haveto_redraw)
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
    static จำนวนเต็ม
fmt_check_par(
    linenr_T	lnum,
    จำนวนเต็ม		*leader_len,
    char_u	**leader_flags,
    จำนวนเต็ม		do_comments)
{
    char_u	*flags = NULL;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ถ้า (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    มิฉะนั้น
	*leader_len = 0;

    ถ้า (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	ขณะ (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    คืน (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    static จำนวนเต็ม
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ถ้า (*s == NUL)
	คืน FALSE;
    l = ml_get_len(lnum) - 1;
    คืน VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    static จำนวนเต็ม
same_leader(
    linenr_T lnum,
    จำนวนเต็ม	    leader1_len,
    char_u  *leader1_flags,
    จำนวนเต็ม	    leader2_len,
    char_u  *leader2_flags)
{
    จำนวนเต็ม	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ถ้า (leader1_len == 0)
	คืน (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ถ้า (leader1_flags != NULL)
    {
	char_u	*p;

	สำหรับ (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ถ้า (*p == COM_FIRST)
		คืน (leader2_len == 0);
	    ถ้า (*p == COM_END)
		คืน FALSE;
	    ถ้า (*p == COM_START)
	    {
		จำนวนเต็ม line_len = ml_get_len(lnum);
		ถ้า (line_len <= leader1_len)
		    คืน FALSE;
		ถ้า (leader2_flags == NULL || leader2_len == 0)
		    คืน FALSE;
		สำหรับ (p = leader2_flags; *p && *p != ':'; ++p)
		    ถ้า (*p == COM_MIDDLE)
			คืน TRUE;
		คืน FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ถ้า (line1 != NULL)
    {
	char_u  *line2;

	สำหรับ (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	สำหรับ (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ถ้า (!VIM_ISWHITE(line2[idx2]))
	    {
		ถ้า (line1[idx1++] != line2[idx2])
		    bha_nga;
	    }
	    มิฉะนั้น
		ขณะ (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    คืน (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    static จำนวนเต็ม
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    จำนวนเต็ม		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    จำนวนเต็ม		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    จำนวนเต็ม		do_comments;		// format comments

    ถ้า (lnum <= 1)
	คืน TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ถ้า (*p == NUL)
	คืน TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ถ้า (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	คืน TRUE;		// after non-paragraph line

    ถ้า (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	คืน TRUE;		// "lnum" is not a paragraph line

    ถ้า (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	คืน TRUE;		// missing trailing space in previous line.

    ถ้า (has_format_option(FO_Q_NUMBER) && (get_number_indent(lnum) > 0))
	คืน TRUE;		// numbered item starts in "lnum".

    ถ้า (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	คืน TRUE;		// change of comment leader.

    คืน FALSE;
}

/*
 * Called after inserting or deleting text: When 'formatoptions' includes the
 * 'a' flag format from the current line until the end of the paragraph.
 * Keep the cursor at the same position relative to the text.
 * The caller must have saved the cursor line for undo, following ones will be
 * saved here.
 */
    void
auto_format(
    จำนวนเต็ม		trailblank,	// when TRUE also format with trailing blank
    จำนวนเต็ม		prev_line)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    จำนวนเต็ม		wasatend;

    ถ้า (!has_format_option(FO_AUTO))
	คืน;

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
    ถ้า (*old != NUL && !trailblank && wasatend)
    {
	จำนวนเต็ม cc;

	dec_cursor();
	cc = gchar_cursor();
	ถ้า (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ถ้า (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    คืน;
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
    ถ้า (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_INSERT))
    {
	char_u *line = ml_get_curline();
	ถ้า (WHITECHAR(line[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    คืน;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ถ้า (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, NULL, FALSE, TRUE) == 0)
	คืน;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ถ้า (prev_line && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ถ้า (u_save_cursor() == FAIL)
	    คืน;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_lines((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ถ้า (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
    }
    มิฉะนั้น
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ถ้า (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ถ้า (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ถ้า (pnew == NULL)
		คืน;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	มิฉะนั้น
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
    void
check_auto_format(
    จำนวนเต็ม		end_insert)	    // TRUE when ending Insert mode
{
    จำนวนเต็ม		c = ' ';
    จำนวนเต็ม		cc;

    ถ้า (!did_add_space)
	คืน;

    cc = gchar_cursor();
    ถ้า (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    มิฉะนั้น
    {
	ถ้า (!end_insert)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ถ้า (c != NUL)
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
    จำนวนเต็ม
comp_textwidth(
    จำนวนเต็ม		ff)	// force formatting (for "gq" command)
{
    จำนวนเต็ม		textwidth;

    textwidth = curbuf->b_p_tw;
    ถ้า (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	ถ้า (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	ถ้า (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	ถ้า (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ถ้า (textwidth < 0)
	textwidth = 0;
    ถ้า (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	ถ้า (textwidth > 79)
	    textwidth = 79;
    }
    คืน textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    void
op_format(
    oparg_T	*oap,
    จำนวนเต็ม		keep_cursor)		// keep cursor on same text char
{
    long	old_line_count = curbuf->b_ml.ml_line_count;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    ถ้า (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	คืน;
    curwin->w_cursor = oap->start;

    ถ้า (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ถ้า ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ถ้า (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_lines(oap->line_count, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ถ้า (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_line_count = curbuf->b_ml.ml_line_count - old_line_count;
    msgmore(old_line_count);

    ถ้า ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    ถ้า (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ถ้า (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ถ้า (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ถ้า (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_line_count;
		มิฉะนั้น
		    wp->w_old_visual_lnum += old_line_count;
	    }
	}
    }
}

#ถ้า defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    void
op_formatexpr(oparg_T *oap)
{
    ถ้า (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ถ้า (fex_format(oap->start.lnum, oap->line_count, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    จำนวนเต็ม
fex_format(
    linenr_T	lnum,
    long	count,
    จำนวนเต็ม		c)	// character to be inserted
{
    จำนวนเต็ม		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    จำนวนเต็ม		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_COUNT, count);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ถ้า (fex == NULL)
	คืน 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ถ้า (use_sandbox)
	++sandbox;
    r = (จำนวนเต็ม)eval_to_number(fex, TRUE);
    ถ้า (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, NULL, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    คืน r;
}
#endif

/*
 * Format "line_count" lines, starting at the cursor position.
 * When "line_count" is negative, format until the end of the paragraph.
 * Lines after the cursor line are saved for undo, caller must have saved the
 * first line.
 */
    void
format_lines(
    linenr_T	line_count,
    จำนวนเต็ม		avoid_fex)		// don't use 'formatexpr'
{
    จำนวนเต็ม		max_len;
    จำนวนเต็ม		is_not_par;		// current line not part of parag.
    จำนวนเต็ม		next_is_not_par;	// next line not part of paragraph
    จำนวนเต็ม		is_end_par;		// at end of paragraph
    จำนวนเต็ม		prev_is_end_par = FALSE;// prev. line not part of parag.
    จำนวนเต็ม		next_is_start_par = FALSE;
    จำนวนเต็ม		leader_len = 0;		// leader len of current line
    จำนวนเต็ม		next_leader_len;	// leader len of next line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    char_u	*next_leader_flags = NULL; // flags for leader of next line
    จำนวนเต็ม		do_comments;		// format comments
    จำนวนเต็ม		do_comments_list = 0;	// format comments with 'n' or '2'
    จำนวนเต็ม		advance = TRUE;
    จำนวนเต็ม		second_indent = -1;	// indent for second line (comment
					// aware)
    จำนวนเต็ม		do_second_indent;
    จำนวนเต็ม		do_number_indent;
    จำนวนเต็ม		do_trail_white;
    จำนวนเต็ม		first_par_line = TRUE;
    จำนวนเต็ม		smd_save;
    long	count;
    จำนวนเต็ม		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_line = curwin->w_cursor.lnum;
    จำนวนเต็ม		force_format = FALSE;
    จำนวนเต็ม		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_number_indent = has_format_option(FO_Q_NUMBER);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ถ้า (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    มิฉะนั้น
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    ถ้า (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    สำหรับ (count = line_count; count != 0 && !got_int; --count)
    {
	// Advance to next paragraph.
	ถ้า (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	ถ้า (count == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = NULL;
	}
	มิฉะนั้น
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    ถ้า (do_number_indent)
		next_is_start_par =
			   (get_number_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	ถ้า (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ถ้า (is_not_par)
	{
	    ถ้า (line_count < 0)
		bha_nga;
	}
	มิฉะนั้น
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ถ้า (first_par_line
		    && (do_second_indent || do_number_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		ถ้า (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ถ้า (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    มิฉะนั้น
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		มิฉะนั้น ถ้า (do_number_indent)
		{
		    ถ้า (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
		    }
		    มิฉะนั้น
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ถ้า (curwin->w_cursor.lnum >= curbuf->b_ml.ml_line_count
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ถ้า (next_leader_flags == NULL
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ถ้า (is_end_par || force_format)
	    {
		ถ้า (need_set_indent)
		{
		    จำนวนเต็ม		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ถ้า (curwin->w_cursor.lnum == first_line)
			indent = get_indent();
		    มิฉะนั้น ถ้า (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    มิฉะนั้น
		    {
			ถ้า (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_expr_indent() :
#endif
				 get_c_indent();
			}
			มิฉะนั้น
			    indent = get_indent();
		    }
		    (void)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		ขณะ (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		    dec_cursor();

		// do the formatting, without 'showmode'
		State = MODE_INSERT;	// for open_line()
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
		ถ้า (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ถ้า (line_count < 0)
			bha_nga;
		    first_par_line = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ถ้า (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		ถ้า (line_count < 0 && u_save_cursor() == FAIL)
		    bha_nga;
		ถ้า (next_leader_len > 0)
		{
		    (void)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		มิฉะนั้น ถ้า (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    จำนวนเต็ม indent = getwhitecols_curline();

		    ถ้า (indent > 0)
		    {
			(void)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ถ้า (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    bha_nga;
		}
		first_par_line = FALSE;
		// If the line is getting long, format it next time
		ถ้า (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		มิฉะนั้น
		    force_format = FALSE;
	    }
	}
	line_breakcheck();
    }
}
