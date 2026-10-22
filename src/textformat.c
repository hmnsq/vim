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

static 정수	did_add_space = FALSE;	// auto_format() added an extra space
					// under the cursor

#define WHITECHAR(cc) (VIM_ISWHITE(cc) && (!enc_utf8 || !utf_iscomposing(utf_ptr2char(ml_get_cursor() + 1))))

/*
 * Return TRUE if format option 'x' is in effect.
 * Take care of no formatting when 'paste' is set.
 */
    정수
has_format_option(정수 x)
{
    만약 (p_paste)
	반환 FALSE;
    반환 (vim_strchr(curbuf->b_p_fo, x) != NULL);
}

/*
 * Format text at the current insert position.
 *
 * If the INSCHAR_COM_LIST flag is present, then the value of second_indent
 * will be the comment leader length sent to open_line().
 */
    void
internal_format(
    정수		textwidth,
    정수		second_indent,
    정수		flags,
    정수		format_only,
    정수		c) // character to be inserted (can be NUL)
{
    정수		cc;
    정수		skip_pos;
    정수		save_char = NUL;
    정수		haveto_redraw = FALSE;
    정수		fo_ins_blank = has_format_option(FO_INS_BLANK);
    정수		fo_multibyte = has_format_option(FO_MBYTE_BREAK);
    정수		fo_rigor_tw  = has_format_option(FO_RIGOROUS_TW);
    정수		fo_white_par = has_format_option(FO_WHITE_PAR);
    정수		first_line = TRUE;
    colnr_T	leader_len;
    정수		no_leader = FALSE;
    정수		do_comments = (flags & INSCHAR_DO_COM);
    정수		safe_tw = trim_to_int(8 * (vimlong_T)textwidth);
#ifdef FEAT_LINEBREAK
    정수		has_lbr = curwin->w_p_lbr;
    정수		has_bri = curwin->w_p_bri;

    // make sure win_lbr_chartabsize() counts correctly
    curwin->w_p_lbr = FALSE;
    curwin->w_p_bri = FALSE;
#endif

    // When 'ai' is off we don't want a space under the cursor to be
    // deleted.  Replace it with an 'x' temporarily.
    만약 (!curbuf->b_p_ai && !(State & VREPLACE_FLAG))
    {
	cc = gchar_cursor();
	만약 (VIM_ISWHITE(cc))
	{
	    save_char = cc;
	    pchar_cursor('x');
	}
    }

    // Repeat breaking lines, until the current line is not too long.
    동안 (!got_int)
    {
	정수	startcol;		// Cursor column at entry
	정수	wantcol;		// column at textwidth border
	정수	foundcol;		// column for start of spaces
	정수	end_foundcol = 0;	// column for start of word
	colnr_T	len;
	colnr_T	virtcol;
	정수	orig_col = 0;
	char_u	*saved_text = NULL;
	colnr_T	col;
	colnr_T	end_col;
	정수	wcc;			// counter for whitespace chars
	정수	did_do_comment = FALSE;
	정수	first_pass;

	// Cursor is currently at the end of line. No need to format
	// if line length is less than textwidth (8 * textwidth for
	// utf safety)
	만약 (curwin->w_cursor.col < safe_tw)
	{
	    virtcol = get_nolist_virtcol()
		+ char2cells(c != NUL ? c : gchar_cursor());
	    만약 (virtcol <= (colnr_T)textwidth)
		break;
	}

	만약 (no_leader)
	    do_comments = FALSE;
	아니면 만약 (!(flags & INSCHAR_FORMAT)
				       && has_format_option(FO_WRAP_COMS))
	    do_comments = TRUE;

	// Don't break until after the comment leader
	만약 (do_comments)
	{
	    char_u *line = ml_get_curline();

	    leader_len = get_leader_len(line, NULL, FALSE, TRUE);
	    만약 (leader_len == 0 && curbuf->b_p_cin)
	    {
		정수		comment_start;

		// Check for a line comment after code.
		comment_start = check_linecomment(line);
		만약 (comment_start != MAXCOL)
		{
		    leader_len = get_leader_len(
				      line + comment_start, NULL, FALSE, TRUE);
		    만약 (leader_len != 0)
			leader_len += comment_start;
		}
	    }
	}
	아니면
	    leader_len = 0;

	// If the line doesn't start with a comment leader, then don't
	// start one in a following broken line.  Avoids that a %word
	// moved to the start of the next line causes all following lines
	// to start with %.
	만약 (leader_len == 0)
	    no_leader = TRUE;
	만약 (!(flags & INSCHAR_FORMAT)
		&& leader_len == 0
		&& !has_format_option(FO_WRAP))

	    break;
	만약 ((startcol = curwin->w_cursor.col) == 0)
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
	동안 ((!fo_ins_blank && !has_format_option(FO_INS_VI))
		    || (flags & INSCHAR_FORMAT)
		    || curwin->w_cursor.lnum != Insstart.lnum
		    || curwin->w_cursor.col >= Insstart.col)
	{
	    만약 (first_pass && c != NUL)
	    {
		cc = c;
		first_pass = FALSE;
	    }
	    아니면
		cc = gchar_cursor();
	    만약 (WHITECHAR(cc))
	    {
		// remember position of blank just before text
		end_col = curwin->w_cursor.col;

		// find start of sequence of blanks
		wcc = 0;
		동안 (curwin->w_cursor.col > 0 && WHITECHAR(cc))
		{
		    dec_cursor();
		    cc = gchar_cursor();

		    // Increment count of how many whitespace chars in this
		    // group; we only need to know if it's more than one.
		    만약 (wcc < 2)
			wcc++;
		}
		만약 (curwin->w_cursor.col == 0 && WHITECHAR(cc))
		    break;		// only spaces in front of text

		// Don't break after a period when 'formatoptions' has 'p' and
		// there are less than two spaces.
		만약 (has_format_option(FO_PERIOD_ABBR) && cc == '.' && wcc < 2)
		    continue;

		// Don't break until after the comment leader
		만약 (curwin->w_cursor.col < leader_len)
		    break;
		만약 (has_format_option(FO_ONE_LETTER))
		{
		    // do not break after one-letter words
		    만약 (curwin->w_cursor.col == 0)
			break;	// one-letter word at begin
		    // do not break "#a b" when 'tw' is 2
		    만약 (curwin->w_cursor.col <= leader_len)
			break;
		    col = curwin->w_cursor.col;
		    dec_cursor();
		    cc = gchar_cursor();

		    만약 (WHITECHAR(cc))
			continue;	// one-letter, continue
		    curwin->w_cursor.col = col;
		}

		inc_cursor();

		end_foundcol = end_col + 1;
		foundcol = curwin->w_cursor.col;
		만약 (curwin->w_cursor.col <= (colnr_T)wantcol)
		    break;
	    }
	    아니면 만약 ((cc >= 0x100 || !utf_allow_break_before(cc))
							       && fo_multibyte)
	    {
		정수 ncc;
		정수 allow_break;

		// Break after or before a multi-byte character.
		만약 (curwin->w_cursor.col != startcol)
		{
		    // Don't break until after the comment leader
		    만약 (curwin->w_cursor.col < leader_len)
			break;
		    col = curwin->w_cursor.col;
		    inc_cursor();
		    ncc = gchar_cursor();

		    allow_break =
			(enc_utf8 && utf_allow_break(cc, ncc))
			|| enc_dbcs;

		    // If we have already checked this position, skip!
		    만약 (curwin->w_cursor.col != skip_pos && allow_break)
		    {
			foundcol = curwin->w_cursor.col;
			end_foundcol = foundcol;
			만약 (curwin->w_cursor.col <= (colnr_T)wantcol)
			    break;
		    }
		    curwin->w_cursor.col = col;
		}

		만약 (curwin->w_cursor.col == 0)
		    break;

		ncc = cc;
		col = curwin->w_cursor.col;

		dec_cursor();
		cc = gchar_cursor();

		만약 (WHITECHAR(cc))
		    continue;		// break with space
		// Don't break until after the comment leader.
		만약 (curwin->w_cursor.col < leader_len)
		    break;

		curwin->w_cursor.col = col;
		skip_pos = curwin->w_cursor.col;

		allow_break =
		    (enc_utf8 && utf_allow_break(cc, ncc))
		    || enc_dbcs;

		// Must handle this to respect line break prohibition.
		만약 (allow_break)
		{
		    foundcol = curwin->w_cursor.col;
		    end_foundcol = foundcol;
		}
		만약 (curwin->w_cursor.col <= (colnr_T)wantcol)
		{
		    정수 ncc_allow_break =
			 (enc_utf8 && utf_allow_break_before(ncc)) || enc_dbcs;

		    만약 (allow_break)
			break;
		    만약 (!ncc_allow_break && !fo_rigor_tw)
		    {
			// Enable at most 1 punct hang outside of textwidth.
			만약 (curwin->w_cursor.col == startcol)
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

			만약 (allow_break)
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
	    만약 (curwin->w_cursor.col == 0)
		break;
	    dec_cursor();
	}

	만약 (foundcol == 0)		// no spaces, cannot break line
	{
	    curwin->w_cursor.col = startcol;
	    break;
	}

	// Going to break the line, remove any "$" now.
	undisplay_dollar();

	// Offset between cursor position and line break is used by replace
	// stack functions.  MODE_VREPLACE does not use this, and backspaces
	// over the text instead.
	만약 (State & VREPLACE_FLAG)
	    orig_col = startcol;	// Will start backspacing from here
	아니면
	    replace_offset = startcol - end_foundcol;

	// adjust startcol for spaces that will be deleted and
	// characters that will remain on top line
	curwin->w_cursor.col = foundcol;
	동안 ((cc = gchar_cursor(), WHITECHAR(cc))
		    && (!fo_white_par || curwin->w_cursor.col < startcol))
	    inc_cursor();
	startcol -= curwin->w_cursor.col;
	만약 (startcol < 0)
	    startcol = 0;

	만약 (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state, we will backspace over the text to be
	    // wrapped, so save a copy now to put on the next line.
	    saved_text = vim_strnsave(ml_get_cursor(), ml_get_cursor_len());
	    curwin->w_cursor.col = orig_col;
	    만약 (saved_text == NULL)
		break;	// Can't do it, out of memory
	    saved_text[startcol] = NUL;

	    // Backspace over characters that will move to the next line
	    만약 (!fo_white_par)
		backspace_until_column(foundcol);
	}
	아니면
	{
	    // put cursor after pos. to break line
	    만약 (!fo_white_par)
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
	만약 (!(flags & INSCHAR_COM_LIST))
	    old_indent = 0;

	// If a comment leader was inserted, may also do this on a following
	// line.
	만약 (did_do_comment)
	    no_leader = FALSE;

	replace_offset = 0;
	만약 (first_line)
	{
	    만약 (!(flags & INSCHAR_COM_LIST))
	    {
		// This section is for auto-wrap of numeric lists.  When not
		// in insert mode (i.e. format_lines()), the INSCHAR_COM_LIST
		// flag will be set and open_line() will handle it (as seen
		// above).  The code here (and in get_number_indent()) will
		// recognize comments if needed...
		만약 (second_indent < 0 && has_format_option(FO_Q_NUMBER))
		    second_indent =
				 get_number_indent(curwin->w_cursor.lnum - 1);
		만약 (second_indent >= 0)
		{
		    만약 (State & VREPLACE_FLAG)
			change_indent(INDENT_SET, second_indent,
							    FALSE, NUL, TRUE);
		    아니면
			만약 (leader_len > 0 && second_indent - leader_len > 0)
		    {
			정수 i;
			정수 padding = second_indent - leader_len;

			// We started at the first_line of a numbered list
			// that has a comment.  the open_line() function has
			// inserted the proper comment leader and positioned
			// the cursor at the end of the split line.  Now we
			// add the additional whitespace needed after the
			// comment leader for the numbered list.
			위해 (i = 0; i < padding; i++)
			    ins_str((char_u *)" ", 1);
		    }
		    아니면
		    {
			(void)set_indent(second_indent, SIN_CHANGED);
		    }
		}
	    }
	    first_line = FALSE;
	}

	만약 (State & VREPLACE_FLAG)
	{
	    // In MODE_VREPLACE state we have backspaced over the text to be
	    // moved, now we re-insert it into the new line.
	    ins_bytes(saved_text);
	    vim_free(saved_text);
	}
	아니면
	{
	    // Check if cursor is not past the NUL off the line, cindent
	    // may have added or removed indent.
	    curwin->w_cursor.col += startcol;
	    len = ml_get_curline_len();
	    만약 (curwin->w_cursor.col > len)
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

    만약 (save_char != NUL)		// put back space after cursor
	pchar_cursor(save_char);

#ifdef FEAT_LINEBREAK
    curwin->w_p_lbr = has_lbr;
    curwin->w_p_bri = has_bri;
#endif
    만약 (!format_only && haveto_redraw)
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
    static 정수
fmt_check_par(
    linenr_T	lnum,
    정수		*leader_len,
    char_u	**leader_flags,
    정수		do_comments)
{
    char_u	*flags = NULL;	    // init for GCC
    char_u	*ptr;

    ptr = ml_get(lnum);
    만약 (do_comments)
	*leader_len = get_leader_len(ptr, leader_flags, FALSE, TRUE);
    아니면
	*leader_len = 0;

    만약 (*leader_len > 0)
    {
	// Search for 'e' flag in comment leader flags.
	flags = *leader_flags;
	동안 (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    반환 (*skipwhite(ptr + *leader_len) == NUL
	    || (*leader_len > 0 && *flags == COM_END)
	    || startPS(lnum, NUL, FALSE));
}

/*
 * Return TRUE if line "lnum" ends in a white character.
 */
    static 정수
ends_in_white(linenr_T lnum)
{
    char_u	*s = ml_get(lnum);
    size_t	l;

    만약 (*s == NUL)
	반환 FALSE;
    l = ml_get_len(lnum) - 1;
    반환 VIM_ISWHITE(s[l]);
}

/*
 * Return TRUE if the two comment leaders given are the same.  "lnum" is
 * the first line.  White-space is ignored.  Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    static 정수
same_leader(
    linenr_T lnum,
    정수	    leader1_len,
    char_u  *leader1_flags,
    정수	    leader2_len,
    char_u  *leader2_flags)
{
    정수	    idx1 = 0, idx2 = 0;
    char_u  *line1;

    만약 (leader1_len == 0)
	반환 (leader2_len == 0);

    // If first leader has 'f' flag, the lines can be joined only if the
    // second line does not have a leader.
    // If first leader has 'e' flag, the lines can never be joined.
    // If first leader has 's' flag, the lines can only be joined if there is
    // some text after it and the second line has the 'm' flag.
    만약 (leader1_flags != NULL)
    {
	char_u	*p;

	위해 (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    만약 (*p == COM_FIRST)
		반환 (leader2_len == 0);
	    만약 (*p == COM_END)
		반환 FALSE;
	    만약 (*p == COM_START)
	    {
		정수 line_len = ml_get_len(lnum);
		만약 (line_len <= leader1_len)
		    반환 FALSE;
		만약 (leader2_flags == NULL || leader2_len == 0)
		    반환 FALSE;
		위해 (p = leader2_flags; *p && *p != ':'; ++p)
		    만약 (*p == COM_MIDDLE)
			반환 TRUE;
		반환 FALSE;
	    }
	}
    }

    // Get current line and next line, compare the leaders.
    // The first line has to be saved, only one line can be locked at a time.
    line1 = vim_strnsave(ml_get(lnum), ml_get_len(lnum));
    만약 (line1 != NULL)
    {
	char_u  *line2;

	위해 (idx1 = 0; VIM_ISWHITE(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(lnum + 1);
	위해 (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    만약 (!VIM_ISWHITE(line2[idx2]))
	    {
		만약 (line1[idx1++] != line2[idx2])
		    break;
	    }
	    아니면
		동안 (VIM_ISWHITE(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    반환 (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * Return TRUE when a paragraph starts in line "lnum".  Return FALSE when the
 * previous line is in the same paragraph.  Used for auto-formatting.
 */
    static 정수
paragraph_start(linenr_T lnum)
{
    char_u	*p;
    정수		leader_len = 0;		// leader len of current line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    정수		next_leader_len;	// leader len of next line
    char_u	*next_leader_flags;	// flags for leader of next line
    정수		do_comments;		// format comments

    만약 (lnum <= 1)
	반환 TRUE;		// start of the file

    p = ml_get(lnum - 1);
    만약 (*p == NUL)
	반환 TRUE;		// after empty line

    do_comments = has_format_option(FO_Q_COMS);
    만약 (fmt_check_par(lnum - 1, &leader_len, &leader_flags, do_comments))
	반환 TRUE;		// after non-paragraph line

    만약 (fmt_check_par(lnum, &next_leader_len, &next_leader_flags, do_comments))
	반환 TRUE;		// "lnum" is not a paragraph line

    만약 (has_format_option(FO_WHITE_PAR) && !ends_in_white(lnum - 1))
	반환 TRUE;		// missing trailing space in previous line.

    만약 (has_format_option(FO_Q_NUMBER) && (get_number_indent(lnum) > 0))
	반환 TRUE;		// numbered item starts in "lnum".

    만약 (!same_leader(lnum - 1, leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	반환 TRUE;		// change of comment leader.

    반환 FALSE;
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
    정수		trailblank,	// when TRUE also format with trailing blank
    정수		prev_line)	// may start in previous line
{
    pos_T	pos;
    char_u	*old;
    정수		wasatend;

    만약 (!has_format_option(FO_AUTO))
	반환;

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
    만약 (*old != NUL && !trailblank && wasatend)
    {
	정수 cc;

	dec_cursor();
	cc = gchar_cursor();
	만약 (!WHITECHAR(cc) && curwin->w_cursor.col > 0
					  && has_format_option(FO_ONE_LETTER))
	    dec_cursor();
	cc = gchar_cursor();
	만약 (WHITECHAR(cc))
	{
	    curwin->w_cursor = pos;
	    반환;
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
    만약 (*old != NUL && !trailblank && !wasatend && pos.col > 0
	    && (State & MODE_INSERT))
    {
	char_u *line = ml_get_curline();
	만약 (WHITECHAR(line[pos.col - 1]))
	{
	    curwin->w_cursor = pos;
	    반환;
	}
    }

    // With the 'c' flag in 'formatoptions' and 't' missing: only format
    // comments.
    만약 (has_format_option(FO_WRAP_COMS) && !has_format_option(FO_WRAP)
				&& get_leader_len(old, NULL, FALSE, TRUE) == 0)
	반환;

    // May start formatting in a previous line, so that after "x" a word is
    // moved to the previous line if it fits there now.  Only when this is not
    // the start of a paragraph.
    만약 (prev_line && !paragraph_start(curwin->w_cursor.lnum))
    {
	--curwin->w_cursor.lnum;
	만약 (u_save_cursor() == FAIL)
	    반환;
    }

    // Do the formatting and restore the cursor position.  "saved_cursor" will
    // be adjusted for the text formatting.
    saved_cursor = pos;
    format_lines((linenr_T)-1, FALSE);
    curwin->w_cursor = saved_cursor;
    saved_cursor.lnum = 0;

    만약 (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
    {
	// "cannot happen"
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	coladvance((colnr_T)MAXCOL);
    }
    아니면
	check_cursor_col();

    // Insert mode: If the cursor is now after the end of the line while it
    // previously wasn't, the line was broken.  Because of the rule above we
    // need to add a space when 'w' is in 'formatoptions' to keep a paragraph
    // formatted.
    만약 (!wasatend && has_format_option(FO_WHITE_PAR))
    {
	char_u	*new = ml_get_curline();
	colnr_T	len = ml_get_curline_len();
	만약 (curwin->w_cursor.col == len)
	{
	    char_u *pnew = vim_strnsave(new, len + 2);
	    만약 (pnew == NULL)
		반환;
	    pnew[len] = ' ';
	    pnew[len + 1] = NUL;
	    ml_replace(curwin->w_cursor.lnum, pnew, FALSE);
	    // remove the space later
	    did_add_space = TRUE;
	}
	아니면
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
    정수		end_insert)	    // TRUE when ending Insert mode
{
    정수		c = ' ';
    정수		cc;

    만약 (!did_add_space)
	반환;

    cc = gchar_cursor();
    만약 (!WHITECHAR(cc))
	// Somehow the space was removed already.
	did_add_space = FALSE;
    아니면
    {
	만약 (!end_insert)
	{
	    inc_cursor();
	    c = gchar_cursor();
	    dec_cursor();
	}
	만약 (c != NUL)
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
    정수
comp_textwidth(
    정수		ff)	// force formatting (for "gq" command)
{
    정수		textwidth;

    textwidth = curbuf->b_p_tw;
    만약 (textwidth == 0 && curbuf->b_p_wm)
    {
	// The width is the window width minus 'wrapmargin' minus all the
	// things that add to the margin.
	textwidth = curwin->w_width - curbuf->b_p_wm;
	만약 (curbuf == cmdwin_buf)
	    textwidth -= 1;
#ifdef FEAT_FOLDING
	textwidth -= curwin->w_p_fdc;
#endif
#ifdef FEAT_SIGNS
	만약 (signcolumn_on(curwin))
	    textwidth -= 1;
#endif
	만약 (curwin->w_p_nu || curwin->w_p_rnu)
	    textwidth -= 8;
    }
    만약 (textwidth < 0)
	textwidth = 0;
    만약 (ff && textwidth == 0)
    {
	textwidth = curwin->w_width - 1;
	만약 (textwidth > 79)
	    textwidth = 79;
    }
    반환 textwidth;
}

/*
 * Implementation of the format operator 'gq'.
 */
    void
op_format(
    oparg_T	*oap,
    정수		keep_cursor)		// keep cursor on same text char
{
    long	old_line_count = curbuf->b_ml.ml_line_count;

    // Place the cursor where the "gq" or "gw" command was given, so that "u"
    // can put it back there.
    curwin->w_cursor = oap->cursor_start;

    만약 (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	반환;
    curwin->w_cursor = oap->start;

    만약 (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    만약 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// Set '[ mark at the start of the formatted area
	curbuf->b_op_start = oap->start;

    // For "gw" remember the cursor position and put it back below (adjusted
    // for joined and split lines).
    만약 (keep_cursor)
	saved_cursor = oap->cursor_start;

    format_lines(oap->line_count, keep_cursor);

    // Leave the cursor at the first non-blank of the last formatted line.
    // If the cursor was moved one line back (e.g. with "Q}") go to the next
    // line, so "." will do the next lines.
    만약 (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    old_line_count = curbuf->b_ml.ml_line_count - old_line_count;
    msgmore(old_line_count);

    만약 ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
	// put '] mark on the end of the formatted area
	curbuf->b_op_end = curwin->w_cursor;

    만약 (keep_cursor)
    {
	curwin->w_cursor = saved_cursor;
	saved_cursor.lnum = 0;

	// formatting may have made the cursor position invalid
	check_cursor();
    }

    만약 (oap->is_VIsual)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    만약 (wp->w_old_cursor_lnum != 0)
	    {
		// When lines have been inserted or deleted, adjust the end of
		// the Visual area to be redrawn.
		만약 (wp->w_old_cursor_lnum > wp->w_old_visual_lnum)
		    wp->w_old_cursor_lnum += old_line_count;
		아니면
		    wp->w_old_visual_lnum += old_line_count;
	    }
	}
    }
}

#만약 defined(FEAT_EVAL)
/*
 * Implementation of the format operator 'gq' for when using 'formatexpr'.
 */
    void
op_formatexpr(oparg_T *oap)
{
    만약 (oap->is_VIsual)
	// When there is no change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    만약 (fex_format(oap->start.lnum, oap->line_count, NUL) != 0)
	// As documented: when 'formatexpr' returns non-zero fall back to
	// internal formatting.
	op_format(oap, FALSE);
}

    정수
fex_format(
    linenr_T	lnum,
    long	count,
    정수		c)	// character to be inserted
{
    정수		use_sandbox = was_set_insecurely(curwin,
					    (char_u *)"formatexpr", OPT_LOCAL);
    정수		r;
    char_u	*fex;
    sctx_T	save_sctx = current_sctx;

    // Set v:lnum to the first line number and v:count to the number of lines.
    // Set v:char to the character to be inserted (can be NUL).
    set_vim_var_nr(VV_LNUM, lnum);
    set_vim_var_nr(VV_COUNT, count);
    set_vim_var_char(c);

    // Make a copy, the option could be changed while calling it.
    fex = vim_strsave(curbuf->b_p_fex);
    만약 (fex == NULL)
	반환 0;
    current_sctx = curbuf->b_p_script_ctx[BV_FEX];

    // Evaluate the function.
    만약 (use_sandbox)
	++sandbox;
    r = (정수)eval_to_number(fex, TRUE);
    만약 (use_sandbox)
	--sandbox;

    set_vim_var_string(VV_CHAR, NULL, -1);
    vim_free(fex);
    current_sctx = save_sctx;

    반환 r;
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
    정수		avoid_fex)		// don't use 'formatexpr'
{
    정수		max_len;
    정수		is_not_par;		// current line not part of parag.
    정수		next_is_not_par;	// next line not part of paragraph
    정수		is_end_par;		// at end of paragraph
    정수		prev_is_end_par = FALSE;// prev. line not part of parag.
    정수		next_is_start_par = FALSE;
    정수		leader_len = 0;		// leader len of current line
    정수		next_leader_len;	// leader len of next line
    char_u	*leader_flags = NULL;	// flags for leader of current line
    char_u	*next_leader_flags = NULL; // flags for leader of next line
    정수		do_comments;		// format comments
    정수		do_comments_list = 0;	// format comments with 'n' or '2'
    정수		advance = TRUE;
    정수		second_indent = -1;	// indent for second line (comment
					// aware)
    정수		do_second_indent;
    정수		do_number_indent;
    정수		do_trail_white;
    정수		first_par_line = TRUE;
    정수		smd_save;
    long	count;
    정수		need_set_indent = TRUE;	// set indent of next paragraph
    linenr_T	first_line = curwin->w_cursor.lnum;
    정수		force_format = FALSE;
    정수		old_State = State;

    // length of a line to force formatting: 3 * 'tw'
    max_len = comp_textwidth(TRUE) * 3;

    // check for 'q', '2', 'n' and 'w' in 'formatoptions'
    do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);
    do_number_indent = has_format_option(FO_Q_NUMBER);
    do_trail_white = has_format_option(FO_WHITE_PAR);

    // Get info about the previous and current line.
    만약 (curwin->w_cursor.lnum > 1)
	is_not_par = fmt_check_par(curwin->w_cursor.lnum - 1
				, &leader_len, &leader_flags, do_comments);
    아니면
	is_not_par = TRUE;
    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum
			  , &next_leader_len, &next_leader_flags, do_comments);
    is_end_par = (is_not_par || next_is_not_par);
    만약 (!is_end_par && do_trail_white)
	is_end_par = !ends_in_white(curwin->w_cursor.lnum - 1);

    curwin->w_cursor.lnum--;
    위해 (count = line_count; count != 0 && !got_int; --count)
    {
	// Advance to next paragraph.
	만약 (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_end_par = is_end_par;
	    is_not_par = next_is_not_par;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	// The last line to be formatted.
	만약 (count == 1 || curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	{
	    next_is_not_par = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = NULL;
	}
	아니면
	{
	    next_is_not_par = fmt_check_par(curwin->w_cursor.lnum + 1
			  , &next_leader_len, &next_leader_flags, do_comments);
	    만약 (do_number_indent)
		next_is_start_par =
			   (get_number_indent(curwin->w_cursor.lnum + 1) > 0);
	}
	advance = TRUE;
	is_end_par = (is_not_par || next_is_not_par || next_is_start_par);
	만약 (!is_end_par && do_trail_white)
	    is_end_par = !ends_in_white(curwin->w_cursor.lnum);

	// Skip lines that are not in a paragraph.
	만약 (is_not_par)
	{
	    만약 (line_count < 0)
		break;
	}
	아니면
	{
	    // For the first line of a paragraph, check indent of second line.
	    // Don't do this for comments and empty lines.
	    만약 (first_par_line
		    && (do_second_indent || do_number_indent)
		    && prev_is_end_par
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		만약 (do_second_indent && !LINEEMPTY(curwin->w_cursor.lnum + 1))
		{
		    만약 (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				   get_indent_lnum(curwin->w_cursor.lnum + 1);
		    }
		    아니면
		    {
			second_indent = next_leader_len;
			do_comments_list = 1;
		    }
		}
		아니면 만약 (do_number_indent)
		{
		    만약 (leader_len == 0 && next_leader_len == 0)
		    {
			// no comment found
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
		    }
		    아니면
		    {
			// get_number_indent() is now "comment aware"...
			second_indent =
				     get_number_indent(curwin->w_cursor.lnum);
			do_comments_list = 1;
		    }
		}
	    }

	    // When the comment leader changes, it's the end of the paragraph.
	    만약 (curwin->w_cursor.lnum >= curbuf->b_ml.ml_line_count
		    || !same_leader(curwin->w_cursor.lnum,
					leader_len, leader_flags,
					   next_leader_len, next_leader_flags))
	    {
		// Special case: If the next line starts with a line comment
		// and this line has a line comment after some text, the
		// paragraph doesn't really end.
		만약 (next_leader_flags == NULL
			|| STRNCMP(next_leader_flags, "://", 3) != 0
			|| check_linecomment(ml_get_curline()) == MAXCOL)
		is_end_par = TRUE;
	    }

	    // If we have got to the end of a paragraph, or the line is
	    // getting long, format it.
	    만약 (is_end_par || force_format)
	    {
		만약 (need_set_indent)
		{
		    정수		indent = 0; // amount of indent needed

		    // Replace indent in first line of a paragraph with minimal
		    // number of tabs and spaces, according to current options.
		    // For the very first formatted line keep the current
		    // indent.
		    만약 (curwin->w_cursor.lnum == first_line)
			indent = get_indent();
		    아니면 만약 (curbuf->b_p_lisp)
			indent = get_lisp_indent();
		    아니면
		    {
			만약 (cindent_on())
			{
			    indent =
#ifdef FEAT_EVAL
				 *curbuf->b_p_inde != NUL ? get_expr_indent() :
#endif
				 get_c_indent();
			}
			아니면
			    indent = get_indent();
		    }
		    (void)set_indent(indent, SIN_CHANGED);
		}

		// put cursor on last non-space
		State = MODE_NORMAL;	// don't go past end-of-line
		coladvance((colnr_T)MAXCOL);
		동안 (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
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
		만약 (is_end_par)
		{
		    // When called with a negative line count, break at the
		    // end of the paragraph.
		    만약 (line_count < 0)
			break;
		    first_par_line = TRUE;
		}
		force_format = FALSE;
	    }

	    // When still in same paragraph, join the lines together.  But
	    // first delete the leader from the second line.
	    만약 (!is_end_par)
	    {
		advance = FALSE;
		curwin->w_cursor.lnum++;
		curwin->w_cursor.col = 0;
		만약 (line_count < 0 && u_save_cursor() == FAIL)
		    break;
		만약 (next_leader_len > 0)
		{
		    (void)del_bytes((long)next_leader_len, FALSE, FALSE);
		    mark_col_adjust(curwin->w_cursor.lnum, (colnr_T)0, 0L,
						    (long)-next_leader_len, 0);
		}
		아니면 만약 (second_indent > 0)  // the "leader" for FO_Q_SECOND
		{
		    정수 indent = getwhitecols_curline();

		    만약 (indent > 0)
		    {
			(void)del_bytes(indent, FALSE, FALSE);
			mark_col_adjust(curwin->w_cursor.lnum,
					     (colnr_T)0, 0L, (long)-indent, 0);
		    }
		}
		curwin->w_cursor.lnum--;
		만약 (do_join(2, TRUE, FALSE, FALSE, FALSE) == FAIL)
		{
		    beep_flush();
		    break;
		}
		first_par_line = FALSE;
		// If the line is getting long, format it next time
		만약 (ml_get_curline_len() > max_len)
		    force_format = TRUE;
		아니면
		    force_format = FALSE;
	    }
	}
	line_breakcheck();
    }
}
