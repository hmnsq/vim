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

static цяло	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    цяло
has_format_option(цяло x)
{
    ако (p_paste)
	върни FALSE;
    върни (vim_strchr(curbuf->b_p_fo, x) != NULL);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    void
internal_format(
    цяло		textwidth,
    цяло		second_indent,
    цяло		flags,
    цяло		format_only,
    цяло		c) // character to be inserted (can be NUL)
{
    цяло		cc;
    цяло		skip_pos;
    цяло		save_char = NUL;
    цяло		haveto_redraw = FALSE;
    цяло		fo_ins_blank = has_format_option(FO_INS_BLANK);
    цяло		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    цяло		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    цяло		fo_white_par = has_format_option(FO_WHITE_PAR);
    цяло		first_line = TRUE;
    colnr_T	leader_len;
    цяло		no_leader = FALSE;
    цяло		do_comments = (flags & INSCHAR_DO_COM);
    цяло		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    цяло		has_lbr = curwin->w_p_lbr;
    цяло		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    ако (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	ако (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    докато (!got_int)
    {
	цяло	startcol;		// Cursor column at entry
	цяло	wantcol;		// column at textwidth border
	цяло	foundcol;		// column for start of spaces
	цяло	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	цяло	orig_col = 0;
	char_u	*saved_text = NULL;
	colnr_T	col;
	colnr_T	end_col;
	цяло	wcc;			// counter for whitespace chars
	цяло	did_do_comment = FALSE;
	цяло	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	ако (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    ако (virtcol <= (colnr_T)textwidth)
		break;
	}

	ако (no_leader)
	    do_comments = FALSE;
	иначе ако (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	ако (do_comments)
	{
	    char_u *line = ml_get_curline();

	    leader_len = get_leader_len(line, NULL, FALSE, TRUE);
	    ако (leader_len == 0 && curbuf->b_p_cin)
	    {
		цяло		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(line);
		ако (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      line + comment_start, NULL, FALSE, TRUE);
		    ако (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	иначе
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	ако (leader_len == 0)
	    no_leader = TRUE;
	ако (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    break;
	ако ((startcol = curwin->w_cursor.col) == 0)
	    break;

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
	докато ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    ако (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    иначе
		cc = gchar_cursor();
	    ако (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		докато (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    ако (wcc < 2)
			wcc++;
		}
		ако (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    break;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		ако (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    continue;

		// Don't break until after the comment leader
		ако (curwin->w_cursor.col < leader_len)
		    break;
		ако (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    ако (curwin->w_cursor.col == 0)
			break;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    ако (curwin->w_cursor.col <= leader_len)
			break;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    ако (WHITECHAR(cc))
			continue;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		ако (curwin->w_cursor.col <= (colnr_T)wantcol)
		    break;
	    }
	    иначе ако ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		цяло ncc;
		цяло allow_break;

		// Break after or before a multi-byte character.
		ако (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    ако (curwin->w_cursor.col < leader_len)
			break;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    ако (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			ако (curwin->w_cursor.col <= (colnr_T)wantcol)
			    break;
		    }
		    curwin->w_cursor.col = col;
		}

		ако (curwin->w_cursor.col == 0)
		    break;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		ако (WHITECHAR(cc))
		    continue;		// break with space
		// Don't break until after the comment leader.
		ако (curwin->w_cursor.col < leader_len)
		    break;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		ако (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		ако (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    цяло ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    ако (allow_break)
			break;
		    ако (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			ако (curwin->w_cursor.col == startcol)
			{
			    // We are inserting a non-breakable char, postpone
			    // line break check to next insert.
			    end_foundcol = foundcol = 0;
			    break;
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

			ако (allow_break)
			{
			    // Break only when we are not at end of line.
			    end_foundcol = foundcol =
				      ncc == NUL? 0 : curwin->w_cursor.col;
			    break;
			}
			curwin->w_cursor.col = col;
		    }
		}
	    }
	    ако (curwin->w_cursor.col == 0)
		break;
	    dec_cursor();
	}

	ако (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    break;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	ако (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	иначе
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	докато ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	ако (startcol < 0)
	    startcol = 0;

	ако (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    ако (saved_text == NULL)
		break;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    ако (!fo_white_par)
		backspace_until_column(foundcol);
	}
	иначе
	{
	    // put cursor after pos. to break line
	    ако (!fo_white_par)
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
	ако (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	ако (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	ако (first_line)
	{
	    ако (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		ако (second_indent < 0 && has_format_option(FO_Q_NUMBER))
		    second_indent =
				 get_number_indent(curwin->w_cursor.lnum - 1);
		ако (second_indent >= 0)
		{
		    ако (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    иначе
			ако (leader_len > 0 && second_indent - leader_len > 0)
		    {
			цяло i;
			цяло padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			за (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    иначе
		    {
			(void)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_line = FALSE;
	}

	ако (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	иначе
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    ако (curwin->w_cursor.col > len)
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

    ако (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    ако (!format_only && haveto_redraw)
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
    static цяло
fmt_check_par(
    linenr_T	lnum,
    цяло		*leader_len,
    char_u	**leader_flags,
    цяло		do_comments)
{
    char_u	*flags = NULL;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    ако (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    иначе
	*leader_len = 0;

    ако (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	докато (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    върни (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    static цяло
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    ако (*s == NUL)
	върни FALSE;
    l = ml_get_len(lnum) - 1;
    върни VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    static цяло
same_leader(
    linenr_T lnum,
    цяло	    leader1_len,
    char_u  *leader1_flags,
    цяло	    leader2_len,
    char_u  *leader2_flags)
{
    цяло	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    ако (leader1_len == 0)
	върни (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    ако (leader1_flags != NULL)
    {
	char_u	*p;

	за (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    ако (*p == COM_FIRST)
		върни (leader2_len == 0);
	    ако (*p == COM_END)
		върни FALSE;
	    ако (*p == COM_START)
	    {
		цяло line_len = ml_get_len(lnum);
		ако (line_len <= leader1_len)
		    върни FALSE;
		ако (leader2_flags == NULL || leader2_len == 0)
		    върни FALSE;
		за (p = leader2_flags; *p && *p != ':'; ++p)
		    ако (*p == COM_MIDDLE)
			върни TRUE;
		върни FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    ако (line1 != NULL)
    {
	char_u  *line2;

	за (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	за (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    ако (!VIM_ISWHITE(line2[idx2]))
	    {
		ако (line1[idx1++] != line2[idx2])
		    break;
	    }
	    иначе
		докато (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    върни (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    static цяло
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    цяло		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    цяло		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    цяло		do_comments;		// format comments

    ако (lnum <= 1)
	върни TRUE;		// start of the file

    p = ml_get(lnum - 1);
    ако (*p == NUL)
	върни TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    ако (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	върни TRUE;		// after non-paragraph line

    ако (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	върни TRUE;		// "lnum" is not a paragraph line

    ако (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	върни TRUE;		// missing trailing space in previous line.

    ако (has_format_option(FO_Q_NUMBER) && (get_number_indent(lnum) > 0))
	върни TRUE;		// numbered item starts in "lnum".

    ако (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	върни TRUE;		// change of comment leader.

    върни FALSE;
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
    цяло		trailblank,	// when TRUE also format with trailing blank
    цяло		prev_line)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    цяло		wasatend;

    ако (!has_format_option(FO_AUTO))
	върни;

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
    ако (*old != NUL && !trailblank && wasatend)
    {
	цяло cc;

	dec_cursor();
	cc = gchar_cursor();
	ако (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	ако (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    върни;
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
    ако (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_INSERT))
    {
	char_u *line = ml_get_curline();
	ако (WHITECHAR(line[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    върни;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    ако (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, NULL, FALSE, TRUE) == 0)
	върни;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    ако (prev_line && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	ако (u_save_cursor() == FAIL)
	    върни;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_lines((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    ако (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
    }
    иначе
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    ако (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	ако (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    ако (pnew == NULL)
		върни;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	иначе
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
    цяло		end_insert)	    // TRUE when ending Insert mode
{
    цяло		c = ' ';
    цяло		cc;

    ако (!did_add_space)
	върни;

    cc = gchar_cursor();
    ако (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    иначе
    {
	ако (!end_insert)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	ако (c != NUL)
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
    цяло
comp_textwidth(
    цяло		ff)	// force formatting (for "gq" command)
{
    цяло		textwidth;

    textwidth = curbuf->b_p_tw;
    ако (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	ако (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	ако (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	ако (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    ако (textwidth < 0)
	textwidth = 0;
    ако (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	ако (textwidth > 79)
	    textwidth = 79;
    }
    върни textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    void
op_format(
    oparg_T	*oap,
    цяло		keep_cursor)		// keep cursor on same text char
{
    long	old_line_count = curbuf->b_ml.ml_line_count;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    ако (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	върни;
    curwin->w_cursor = oap->start;

    ако (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ако ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    ако (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_lines(oap->line_count, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    ако (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_line_count = curbuf->b_ml.ml_line_count - old_line_count;
    msgmore(old_line_count);

    ако ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    ако (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    ако (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ако (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		ако (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_line_count;
		иначе
		    wp->w_old_visual_lnum += old_line_count;
	    }
	}
    }
}

#ако defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    void
op_formatexpr(oparg_T *oap)
{
    ако (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    ако (fex_format(oap->start.lnum, oap->line_count, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    цяло
fex_format(
    linenr_T	lnum,
    long	count,
    цяло		c)	// character to be inserted
{
    цяло		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    цяло		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_COUNT, count);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    ако (fex == NULL)
	върни 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    ако (use_sandbox)
	++sandbox;
    r = (цяло)eval_to_number(fex, TRUE);
    ако (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, NULL, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    върни r;
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
    цяло		avoid_fex)		// don't use 'formatexpr'
{
    цяло		max_len;
    цяло		is_not_par;		// current line not part of parag.
    цяло		next_is_not_par;	// next line not part of paragraph
    цяло		is_end_par;		// at end of paragraph
    цяло		prev_is_end_par = FALSE;// prev. line not part of parag.
    цяло		next_is_start_par = FALSE;
    цяло		leader_len = 0;		// leader len of current line
    цяло		next_leader_len;	// leader len of next line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    char_u	*next_leader_flags = NULL; // flags for leader of next line
    цяло		do_comments;		// format comments
    цяло		do_comments_list = 0;	// format comments with 'n' or '2'
    цяло		advance = TRUE;
    цяло		second_indent = -1;	// indent for second line (comment
					// aware)
    цяло		do_second_indent;
    цяло		do_number_indent;
    цяло		do_trail_white;
    цяло		first_par_line = TRUE;
    цяло		smd_save;
    long	count;
    цяло		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_line = curwin->w_cursor.lnum;
    цяло		force_format = FALSE;
    цяло		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_number_indent = has_format_option(FO_Q_NUMBER);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    ако (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    иначе
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    ако (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    за (count = line_count; count != 0 && !got_int; --count)
    {
	// Advance to next paragraph.
	ако (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	ако (count == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = NULL;
	}
	иначе
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    ако (do_number_indent)
		next_is_start_par =
			   (get_number_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	ако (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	ако (is_not_par)
	{
	    ако (line_count < 0)
		break;
	}
	иначе
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    ако (first_par_line
		    && (do_second_indent || do_number_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		ако (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    ако (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    иначе
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		иначе ако (do_number_indent)
		{
		    ако (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
		    }
		    иначе
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    ако (curwin->w_cursor.lnum >= curbuf->b_ml.ml_line_count
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		ако (next_leader_flags == NULL
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    ако (is_end_par || force_format)
	    {
		ако (need_set_indent)
		{
		    цяло		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    ако (curwin->w_cursor.lnum == first_line)
			indent = get_indent();
		    иначе ако (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    иначе
		    {
			ако (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_expr_indent() :
#endif
				 get_c_indent();
			}
			иначе
			    indent = get_indent();
		    }
		    (void)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		докато (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
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
		ако (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    ако (line_count < 0)
			break;
		    first_par_line = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    ако (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		ако (line_count < 0 && u_save_cursor() == FAIL)
		    break;
		ако (next_leader_len > 0)
		{
		    (void)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		иначе ако (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    цяло indent = getwhitecols_curline();

		    ако (indent > 0)
		    {
			(void)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		ако (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    break;
		}
		first_par_line = FALSE;
		// If the line is getting long, format it next time
		ако (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		иначе
		    force_format = FALSE;
	    }
	}
	line_breakcheck();
    }
}
