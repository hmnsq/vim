/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * misc1.c: functions that didn't seem to fit elsewhere
 */

#include "vim.h"
#include "version.h"

#ᮚᮦᮔ᮪ defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#ᮚᮦᮔ᮪ defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
static garray_T	ga_users;

/*
 * get_leader_len() returns the length in bytes of the prefix of the given
 * string which introduces a comment.  If this string is not a comment then
 * 0 is returned.
 * When "flags" is not NULL, it is set to point to the flags of the recognized
 * comment leader.
 * "backward" must be true for the "O" command.
 * If "include_space" is set, include trailing whitespace while calculating the
 * length.
 */
    puur_naa_nka
get_leader_len(
    char_u	*line,
    char_u	**flags,
    puur_naa_nka		backward,
    puur_naa_nka		include_space)
{
    puur_naa_nka		i, j;
    puur_naa_nka		result;
    puur_naa_nka		got_com = FALSE;
    puur_naa_nka		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    puur_naa_nka		middle_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = NULL;

    result = i = 0;
    ᮞᮨᮓᮨᮜ (VIM_ISWHITE(line[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    ᮞᮨᮓᮨᮜ (line[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	for (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ᮚᮦᮔ᮪ (!got_com && flags != NULL)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (void)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ᮚᮦᮔ᮪ (string == NULL)	    // missing ':', ignore this part
		continue;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ᮚᮦᮔ᮪ (middle_match_len != 0
		    && vim_strchr(part_buf, COM_MIDDLE) == NULL
		    && vim_strchr(part_buf, COM_END) == NULL)
		bha_nga;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ᮚᮦᮔ᮪ (got_com && vim_strchr(part_buf, COM_NEST) == NULL)
		continue;

	    // When 'O' flag present and using "O" command skip this one.
	    ᮚᮦᮔ᮪ (backward && vim_strchr(part_buf, COM_NOBACK) != NULL)
		continue;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ᮚᮦᮔ᮪ (VIM_ISWHITE(string[0]))
	    {
		ᮚᮦᮔ᮪ (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    continue;  // missing white space
		ᮞᮨᮓᮨᮜ (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    for (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		;
	    ᮚᮦᮔ᮪ (string[j] != NUL)
		continue;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_BLANK) != NULL
			   && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		continue;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_MIDDLE) != NULL)
	    {
		ᮚᮦᮔ᮪ (middle_match_len == 0)
		{
		    middle_match_len = j;
		    saved_flags = prev_list;
		}
		continue;
	    }
	    ᮚᮦᮔ᮪ (middle_match_len != 0 && j > middle_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		middle_match_len = 0;

	    ᮚᮦᮔ᮪ (middle_match_len == 0)
		i += j;
	    found_one = TRUE;
	    bha_nga;
	}

	ᮚᮦᮔ᮪ (middle_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ᮚᮦᮔ᮪ (!got_com && flags != NULL)
		*flags = saved_flags;
	    i += middle_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	ᮚᮦᮔ᮪ (!found_one)
	    bha_nga;

	result = i;

	// Include any trailing white space.
	ᮞᮨᮓᮨᮜ (VIM_ISWHITE(line[i]))
	    ++i;

	ᮚᮦᮔ᮪ (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_NEST) == NULL)
	    bha_nga;
    }
    ᮝᮜᮤ result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    puur_naa_nka
get_last_leader_offset(char_u *line, char_u **flags)
{
    puur_naa_nka		result = -1;
    puur_naa_nka		i, j;
    puur_naa_nka		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    puur_naa_nka		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (puur_naa_nka)STRLEN(line);
    ᮞᮨᮓᮨᮜ (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	for (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (void)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ᮚᮦᮔ᮪ (string == NULL)	// If everything is fine, this cannot actually
				// happen.
		continue;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ᮚᮦᮔ᮪ (VIM_ISWHITE(string[0]))
	    {
		ᮚᮦᮔ᮪ (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    continue;
		ᮞᮨᮓᮨᮜ (VIM_ISWHITE(*string))
		    ++string;
	    }
	    for (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		/* do nothing */;
	    ᮚᮦᮔ᮪ (string[j] != NUL)
		continue;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_BLANK) != NULL
		    && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		continue;

	    ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_MIDDLE) != NULL)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		for (j = 0; VIM_ISWHITE(line[j]) && j <= i; j++)
		    ;
		ᮚᮦᮔ᮪ (j < i)
		    continue;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    ᮚᮦᮔ᮪ (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    bha_nga;
	}

	ᮚᮦᮔ᮪ (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    puur_naa_nka     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ᮚᮦᮔ᮪ (vim_strchr(part_buf, COM_NEST) != NULL)
		continue;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    ᮞᮨᮓᮨᮜ (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (puur_naa_nka)STRLEN(com_leader);

	    for (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(void)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ᮚᮦᮔ᮪ (flags_save == com_flags)
		    continue;
		string = vim_strchr(part_buf2, ':');
		++string;
		ᮞᮨᮓᮨᮜ (VIM_ISWHITE(*string))
		    ++string;
		len2 = (puur_naa_nka)STRLEN(string);
		ᮚᮦᮔ᮪ (len2 == 0)
		    continue;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		for (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ᮚᮦᮔ᮪ (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ᮚᮦᮔ᮪ (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ᮝᮜᮤ result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    puur_naa_nka
plines(linenr_T lnum)
{
    ᮝᮜᮤ plines_win(curwin, lnum, TRUE);
}

    puur_naa_nka
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    puur_naa_nka		limit_winheight)	// when TRUE limit to window height
{
#ᮚᮦᮔ᮪ defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ᮝᮜᮤ plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    puur_naa_nka
plines_nofill(linenr_T lnum)
{
    ᮝᮜᮤ plines_win_nofill(curwin, lnum, TRUE);
}

    puur_naa_nka
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    puur_naa_nka		limit_winheight)	// when TRUE limit to window height
{
#endif
    puur_naa_nka		lines;

    ᮚᮦᮔ᮪ (wp->w_width == 0)
	ᮝᮜᮤ 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ᮚᮦᮔ᮪ (lineFolded(wp, lnum) == TRUE)
	ᮝᮜᮤ 1;
#endif

    ᮚᮦᮔ᮪ (!wp->w_p_wrap)
	lines = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_count_above_below(wp->w_buffer, lnum)
#endif
	;
    ᮜᮤᮚ
	lines = plines_win_nofold(wp, lnum);

    ᮚᮦᮔ᮪ (limit_winheight && lines > wp->w_height)
	ᮝᮜᮤ wp->w_height;
    ᮝᮜᮤ lines;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    puur_naa_nka
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    puur_naa_nka		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ᮚᮦᮔ᮪ (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	ᮝᮜᮤ 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (puur_naa_nka)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ᮚᮦᮔ᮪ (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    ᮚᮦᮔ᮪ (width <= 0)
	ᮝᮜᮤ 32000;
    ᮚᮦᮔ᮪ (col <= width)
	ᮝᮜᮤ 1;
    col -= width;
    width += win_col_off2(wp);
    ᮝᮜᮤ (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    puur_naa_nka
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    puur_naa_nka		lines = 0;
    puur_naa_nka		width;
    char_u	*line;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    lines = diff_check_fill(wp, lnum);
#endif

    ᮚᮦᮔ᮪ (!wp->w_p_wrap)
	ᮝᮜᮤ lines + 1;

    ᮚᮦᮔ᮪ (wp->w_width == 0)
	ᮝᮜᮤ lines + 1;

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    // "column" is a byte index, advance the pointer until it is reached.
    ᮞᮨᮓᮨᮜ (*cts.cts_ptr != NUL && cts.cts_ptr < line + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, NULL, NULL);
	MB_PTR_ADV(cts.cts_ptr);
    }

    /*
     * If *cts.cts_ptr is a TAB, and the TAB is not displayed as ^I, and we're
     * not in MODE_INSERT state, then col must be adjusted so that it
     * represents the last screen position of the TAB.  This only fixes an
     * error when the TAB wraps from one screen line to the next (when
     * 'columns' is not a multiple of 'ts') -- webb.
     */
    col = cts.cts_vcol;
    ᮚᮦᮔ᮪ (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, NULL, NULL) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    ᮚᮦᮔ᮪ (width <= 0)
	ᮝᮜᮤ 9999;

    lines += 1;
    ᮚᮦᮔ᮪ (col > width)
	lines += (col - width) / (width + win_col_off2(wp)) + 1;
    ᮝᮜᮤ lines;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    puur_naa_nka
plines_m_win(win_T *wp, linenr_T first, linenr_T last, puur_naa_nka max)
{
    puur_naa_nka		count = 0;

    ᮞᮨᮓᮨᮜ (first <= last && count < max)
    {
#ifdef FEAT_FOLDING
	puur_naa_nka	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = foldedCount(wp, first, NULL);
	ᮚᮦᮔ᮪ (x > 0)
	{
	    ++count;	    // count 1 for "+-- folded" line
	    first += x;
	}
	ᮜᮤᮚ
#endif
	{
#ifdef FEAT_DIFF
	    ᮚᮦᮔ᮪ (first == wp->w_topline)
		count += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    ᮜᮤᮚ
#endif
		count += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    ᮚᮦᮔ᮪ (first == wp->w_buffer->b_ml.ml_line_count + 1)
	count += diff_check_fill(wp, first);
#endif
    ᮝᮜᮤ MIN(max, count);
}

    puur_naa_nka
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    puur_naa_nka		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ᮚᮦᮔ᮪ (pos->col == MAXCOL)
	ᮝᮜᮤ NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ᮚᮦᮔ᮪ (pos->col > ptrlen)
	ᮝᮜᮤ NUL;
    ᮚᮦᮔ᮪ (has_mbyte)
	ᮝᮜᮤ (*mb_ptr2char)(ptr);
    ᮝᮜᮤ (puur_naa_nka)*ptr;
}

    puur_naa_nka
gchar_cursor(void)
{
    ᮚᮦᮔ᮪ (has_mbyte)
	ᮝᮜᮤ (*mb_ptr2char)(ml_get_cursor());
    ᮝᮜᮤ (puur_naa_nka)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    puur_naa_nka
char_before_cursor(void)
{
    ᮚᮦᮔ᮪ (curwin->w_cursor.col == 0)
	ᮝᮜᮤ -1;

    char_u *line = ml_get_curline();

    ᮚᮦᮔ᮪ (has_mbyte)
    {
	char_u *p = line + curwin->w_cursor.col;
	puur_naa_nka prev_len = (*mb_head_off)(line, p - 1) + 1;
	ᮝᮜᮤ mb_ptr2char(p - prev_len);
    }
    ᮝᮜᮤ line[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    void
pchar_cursor(puur_naa_nka c)
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE)
						  + curwin->w_cursor.col) = c;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_to_option_part(char_u *p)
{
    ᮚᮦᮔ᮪ (*p == ',')
	++p;
    ᮞᮨᮓᮨᮜ (*p == ' ')
	++p;
    ᮝᮜᮤ p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    void
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ᮚᮦᮔ᮪ (wp->w_buffer == buf && wp->w_status_height)
	{
	    wp->w_redr_status = true;
	    set_must_redraw(UPD_VALID);
	}
}

/*
 * Ask for a reply from the user, a 'y' or a 'n', with prompt "str" (which
 * should have been translated already).
 * No other characters are accepted, the message is repeated until a valid
 * reply is entered or CTRL-C is hit.
 * If direct is TRUE, don't use vgetc() but ui_inchar(), don't get characters
 * from any buffers but directly from the user.
 *
 * return the 'y' or 'n'
 */
    puur_naa_nka
ask_yesno(char_u *str, puur_naa_nka direct)
{
    puur_naa_nka	    r = ' ';
    puur_naa_nka	    save_State = State;

    ᮚᮦᮔ᮪ (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys

    ᮞᮨᮓᮨᮜ (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ᮚᮦᮔ᮪ (direct)
	    r = get_keystroke();
	ᮜᮤᮚ
	    r = plain_vgetc();
	ᮚᮦᮔ᮪ (r == Ctrl_C || r == ESC)
	    r = 'n';
	msg_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_keys;

    ᮝᮜᮤ r;
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    void
get_mode(char_u *buf)
{
    puur_naa_nka		i = 0;

    ᮚᮦᮔ᮪ (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ᮜᮤᮚ ᮚᮦᮔ᮪ (term_use_loop())
    {
	ᮚᮦᮔ᮪ (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	ᮚᮦᮔ᮪ (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	ᮜᮤᮚ ᮚᮦᮔ᮪ (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    ᮜᮤᮚ ᮚᮦᮔ᮪ (State & MODE_INSERT)
    {
	ᮚᮦᮔ᮪ (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ᮜᮤᮚ
	{
	    ᮚᮦᮔ᮪ (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ᮜᮤᮚ
		buf[i++] = 'i';
	}

	ᮚᮦᮔ᮪ (ins_compl_active())
	    buf[i++] = 'c';
	ᮜᮤᮚ ᮚᮦᮔ᮪ (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	ᮚᮦᮔ᮪ (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	ᮜᮤᮚ ᮚᮦᮔ᮪ (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	ᮚᮦᮔ᮪ ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (VIsual_active)
    {
	ᮚᮦᮔ᮪ (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	ᮜᮤᮚ
	{
	    buf[i++] = VIsual_mode;
	    ᮚᮦᮔ᮪ (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ᮜᮤᮚ
    {
	buf[i++] = 'n';
	ᮚᮦᮔ᮪ (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ᮜᮤᮚ ᮚᮦᮔ᮪ (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    void
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    ᮚᮦᮔ᮪ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ᮝᮜᮤ;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ᮚᮦᮔ᮪ (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    static void
may_add_state_char(garray_T *gap, char_u *include, puur_naa_nka c)
{
    ᮚᮦᮔ᮪ (include == NULL || vim_strchr(include, c) != NULL)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    void
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = NULL;
    puur_naa_nka		i;

    ᮚᮦᮔ᮪ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ᮝᮜᮤ;

    ga_init2(&ga, 1, 20);
    ᮚᮦᮔ᮪ (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ᮚᮦᮔ᮪ (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == NULL))
	may_add_state_char(&ga, include, 'm');
    ᮚᮦᮔ᮪ (op_pending())
	may_add_state_char(&ga, include, 'o');
    ᮚᮦᮔ᮪ (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    ᮚᮦᮔ᮪ (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    ᮚᮦᮔ᮪ (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    ᮚᮦᮔ᮪ (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    for (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    ᮚᮦᮔ᮪ (msg_scrolled > 0)
	may_add_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

#endif // FEAT_EVAL

/*
 * Get a key stroke directly from the user.
 * Ignores mouse clicks and scrollbar events, except a click for the left
 * button (used at the more prompt).
 * Doesn't use vgetc(), because it syncs undo and eats mapped characters.
 * Disadvantage: typeahead is ignored.
 * Translates the interrupt character for unix to ESC.
 */
    puur_naa_nka
get_keystroke(void)
{
    char_u	*buf = NULL;
    puur_naa_nka		buflen = 150;
    puur_naa_nka		maxlen;
    puur_naa_nka		len = 0;
    puur_naa_nka		n;
    puur_naa_nka		save_mapped_ctrl_c = mapped_ctrl_c;
    puur_naa_nka		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    for (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ᮚᮦᮔ᮪ (buf == NULL)
	    buf = alloc(buflen);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ᮚᮦᮔ᮪ (buf == NULL)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ᮚᮦᮔ᮪ (buf == NULL)
	{
	    do_outofmem_msg((long_u)buflen);
	    ᮝᮜᮤ ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ᮚᮦᮔ᮪ (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_input_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ᮚᮦᮔ᮪ ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    continue;

	ᮚᮦᮔ᮪ (n == KEYLEN_REMOVED)  // key code removed
	{
	    ᮚᮦᮔ᮪ (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    continue;
	}
	ᮚᮦᮔ᮪ (n > 0)		// found a termcode: adjust length
	    len = n;
	ᮚᮦᮔ᮪ (len == 0)		// nothing typed yet
	    continue;

	// Handle modifier and/or special key code.
	n = buf[0];
	ᮚᮦᮔ᮪ (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    ᮚᮦᮔ᮪ (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_key(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		ᮚᮦᮔ᮪ (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		ᮚᮦᮔ᮪ (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		continue;
	    }
	    bha_nga;
	}
	ᮚᮦᮔ᮪ (has_mbyte)
	{
	    ᮚᮦᮔ᮪ (MB_BYTE2LEN(n) > len)
		continue;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	ᮚᮦᮔ᮪ (n == intr_char)
	    n = ESC;
#endif
	bha_nga;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ᮝᮜᮤ merge_modifyOtherKeys(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    static puur_naa_nka
vim_append_digit_int(puur_naa_nka *value, puur_naa_nka digit)
{
    puur_naa_nka x = *value;
    ᮚᮦᮔ᮪ (x > ((INT_MAX - digit) / 10))
	ᮝᮜᮤ FAIL;
    *value = x * 10 + digit;
    ᮝᮜᮤ OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    puur_naa_nka
get_number(
    puur_naa_nka	    colon,			// allow colon to abort
    puur_naa_nka	    *mouse_used)
{
    puur_naa_nka	n = 0;
    puur_naa_nka	c;
    puur_naa_nka typed = 0;

    ᮚᮦᮔ᮪ (mouse_used != NULL)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ᮚᮦᮔ᮪ (msg_silent != 0)
	ᮝᮜᮤ 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys
    for (;;)
    {
	windgoto(msg_row, cmdline_col_off + msg_col);
	c = safe_vgetc();
	ᮚᮦᮔ᮪ (VIM_ISDIGIT(c))
	{
	    ᮚᮦᮔ᮪ (vim_append_digit_int(&n, c - '0') == FAIL)
		ᮝᮜᮤ 0;
	    msg_putchar(c);
	    ++typed;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ᮚᮦᮔ᮪ (typed > 0)
	    {
		msg_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (mouse_used != NULL && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    bha_nga;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ᮚᮦᮔ᮪ (!exmode_active)
		cmdline_row = msg_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    bha_nga;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    bha_nga;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (c == CAR || c == NL )
	    bha_nga;
    }
    --no_mapping;
    --allow_keys;
    ᮝᮜᮤ n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    puur_naa_nka
prompt_for_number(puur_naa_nka *mouse_used)
{
    puur_naa_nka		i;
    puur_naa_nka		save_cmdline_row;
    puur_naa_nka		save_State;

    // When using ":silent" assume that <CR> was entered.
    ᮚᮦᮔ᮪ (mouse_used != NULL)
	msg_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ᮜᮤᮚ
	msg_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_number(TRUE, mouse_used);
    ᮚᮦᮔ᮪ (KeyTyped)
    {
	// don't call wait_return() now
	ᮚᮦᮔ᮪ (msg_row > 0)
	    cmdline_row = msg_row - 1;
	need_wait_return = FALSE;
	msg_didany = FALSE;
	msg_didout = FALSE;
    }
    ᮜᮤᮚ
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ᮝᮜᮤ i;
}

    void
msgmore(long n)
{
    long pn;

    ᮚᮦᮔ᮪ (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ᮝᮜᮤ;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ᮚᮦᮔ᮪ (keep_msg != NULL && !keep_msg_more)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (n > 0)
	pn = n;
    ᮜᮤᮚ
	pn = -n;

    ᮚᮦᮔ᮪ (pn > p_report)
    {
	ᮚᮦᮔ᮪ (n > 0)
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ᮜᮤᮚ
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ᮚᮦᮔ᮪ (got_int)
	    vim_strcat((char_u *)msg_buf, (char_u *)_(" (Interrupted)"),
								  MSG_BUF_LEN);
	ᮚᮦᮔ᮪ (msg(msg_buf))
	{
	    set_keep_msg((char_u *)msg_buf, 0);
	    keep_msg_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    void
beep_flush(void)
{
    ᮚᮦᮔ᮪ (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    void
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    ᮚᮦᮔ᮪ (emsg_silent != 0 || in_assert_fails)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	static puur_naa_nka		did_init = FALSE;
	static elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ᮚᮦᮔ᮪ (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    ᮚᮦᮔ᮪ (p_vb
#ifdef FEAT_GUI
		    // While the GUI is starting up the termcap is set for
		    // the GUI but the output still goes to a terminal.
		    && !(gui.in_use && gui.starting)
#endif
	       )
	    {
		out_str_cf(T_VB);
#ifdef FEAT_VTP
		// No restore color information, refresh the screen.
		ᮚᮦᮔ᮪ (has_vtp_working() != 0
# ifdef FEAT_TERMGUICOLORS
			&& (p_tgc || (!p_tgc && t_colors >= 256))
# endif
		   )
		{
		    redraw_later(UPD_CLEAR);
		    update_screen(0);
		    redrawcmd();
		}
#endif
	    }
	    ᮜᮤᮚ
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ᮚᮦᮔ᮪ (vim_strchr(p_debug, 'e') != NULL)
    {
	msg_source(HL_ATTR(HLF_W));
	msg_attr(_("Beep!"), HL_ATTR(HLF_W));
    }
}

/*
 * To get the "real" home directory:
 * - get value of $HOME
 * For Unix:
 *  - go to that directory
 *  - do mch_dirname() to get the real name of that directory.
 *  This also works with mounts and links.
 *  Don't do this for MS-DOS, it will change the "current dir" for a drive.
 * For Windows:
 *  This code is duplicated in init_homedir() in dosinst.c.  Keep in sync!
 */
    void
init_homedir(void)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ᮜᮤᮚ
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ᮚᮦᮔ᮪ (var == NULL || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ᮚᮦᮔ᮪ (homepath == NULL || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ᮚᮦᮔ᮪ (homedrive != NULL
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((var_na *)NameBuff, "%s%s", homedrive, homepath);
	    ᮚᮦᮔ᮪ (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ᮚᮦᮔ᮪ (var == NULL)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ᮚᮦᮔ᮪ (var != NULL && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    ᮚᮦᮔ᮪ (exp != NULL && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((var_na *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    ᮚᮦᮔ᮪ (var != NULL && *var == NUL)	// empty is same as not set
	var = NULL;

    ᮚᮦᮔ᮪ (enc_utf8 && var != NULL)
    {
	puur_naa_nka	len;
	char_u  *pp = NULL;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (puur_naa_nka)STRLEN(var), &pp, &len);
	ᮚᮦᮔ᮪ (pp != NULL)
	{
	    homedir = pp;
	    ᮝᮜᮤ;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ᮚᮦᮔ᮪ (var == NULL)
	var = (char_u *)"C:/";
#endif

    ᮚᮦᮔ᮪ (var != NULL)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ᮚᮦᮔ᮪ (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((var_na *)NameBuff) == 0)
	{
	    ᮚᮦᮔ᮪ (!mch_chdir((var_na *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ᮚᮦᮔ᮪ (mch_chdir((var_na *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#ᮚᮦᮔ᮪ defined(EXITFREE)
    void
free_homedir(void)
{
    vim_free(homedir);
}

    void
free_users(void)
{
    ga_clear_strings(&ga_users);
}
#endif

#ᮚᮦᮔ᮪ defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    void
init_vimdir(void)
{
    puur_naa_nka	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ᮚᮦᮔ᮪ (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ᮚᮦᮔ᮪ (mustfree)
	vim_free(p);
}
#endif

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(char_u *src)
{
    ᮝᮜᮤ expand_env_save_opt(src, FALSE, NULL);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, puur_naa_nka one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ᮚᮦᮔ᮪ (p != NULL)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, NULL);
    ᮝᮜᮤ p;
}

/*
 * Expand environment variable with path name.
 * "~/" is also expanded, using $HOME.	For Unix "~user/" is expanded.
 * Skips over "\ ", "\~" and "\$" (not for Win32 though).
 * If anything fails no expansion is done and dst equals src.
 */
    size_t
expand_env(
    char_u	*src,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    puur_naa_nka		dstlen)		// maximum length of the result
{
    ᮝᮜᮤ expand_env_esc(src, dst, dstlen, NULL, FALSE, NULL);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    puur_naa_nka		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    puur_naa_nka		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    puur_naa_nka		c;
    char_u	*var;
    puur_naa_nka		copy_char;
    puur_naa_nka		mustfree;	// var was allocated, need to free it later
    puur_naa_nka		at_start = TRUE; // at start of a name
    puur_naa_nka		startstr_len = 0;
    char_u	*dst_start = dst;

    ᮚᮦᮔ᮪ (startstr != NULL)
	startstr_len = (puur_naa_nka)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    ᮞᮨᮓᮨᮜ (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	ᮚᮦᮔ᮪ (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (void)skip_expr(&src, NULL);
	    ᮚᮦᮔ᮪ (*src == '`')
		++src;
	    len = src - var;
	    ᮚᮦᮔ᮪ (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (puur_naa_nka)len;
	    continue;
	}
#endif
	copy_char = TRUE;
	ᮚᮦᮔ᮪ ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#ᮚᮦᮔ᮪ defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ᮚᮦᮔ᮪ (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		ᮚᮦᮔ᮪ (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    ᮞᮨᮓᮨᮜ (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		ᮜᮤᮚ
#endif
		{
		    ᮞᮨᮓᮨᮜ (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#ᮚᮦᮔ᮪ defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#ᮚᮦᮔ᮪ defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ᮚᮦᮔ᮪ (src[1] == '{' && *tail != '}')
# ᮜᮤᮚ
		ᮚᮦᮔ᮪ (*src == '%' && *tail != '%')
# endif
		    var = NULL;
		ᮜᮤᮚ
		{
# ifdef UNIX
		    ᮚᮦᮔ᮪ (src[1] == '{')
# ᮜᮤᮚ
		    ᮚᮦᮔ᮪ (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ᮚᮦᮔ᮪ defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != NULL)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    ᮜᮤᮚ					// user directory
	    {
#ᮚᮦᮔ᮪ defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		ᮞᮨᮓᮨᮜ (	   c-- > 0
			&& *tail
			&& vim_isfilec(*tail)
			&& !vim_ispathsep(*tail))
		    *var++ = *tail++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  ᮚᮦᮔ᮪ defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    sa_nracanaa passwd *pw = (*dst == NUL)
					? NULL : getpwnam((var_na *)dst + 1);

		    var = (pw == NULL) ? NULL : (char_u *)pw->pw_dir;
		}
		ᮚᮦᮔ᮪ (var == NULL)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, NULL,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ᮜᮤᮚ	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*path, *next_path, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    next_path = paths;
		    ᮞᮨᮓᮨᮜ (*next_path)
		    {
			for (path = next_path; *next_path && *next_path != ',';
				next_path++);
			ᮚᮦᮔ᮪ (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (var_na *)test,
			    parimaa_na(test),
			    "%s/%s",
			    path,
			    dst + 1);
			ᮚᮦᮔ᮪ (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ᮚᮦᮔ᮪ (var != NULL)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    bha_nga;
			}
		    }
		}
# endif // UNIX
#ᮜᮤᮚ
		// cannot expand user's home directory, so don't try
		var = NULL;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ᮚᮦᮔ᮪ (p_ssl && var != NULL && vim_strchr(var, '\\') != NULL)
	    {
		char_u	*p = vim_strsave(var);

		ᮚᮦᮔ᮪ (p != NULL)
		{
		    ᮚᮦᮔ᮪ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		    forward_slash(var);
		}
	    }
#endif

	    // If "var" contains any character from "esc_chars", escape it
	    // with a backslash.  The historical use is escaping spaces so
	    // that ":e ~/tt" works when $HOME contains a space.
	    ᮚᮦᮔ᮪ (esc_chars != NULL && var != NULL
		    && vim_strpbrk(var, esc_chars) != NULL)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ᮚᮦᮔ᮪ (p != NULL)
		{
		    ᮚᮦᮔ᮪ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ᮚᮦᮔ᮪ (var != NULL && *var != NUL)
	    {
		c = (puur_naa_nka)STRLEN(var);

		ᮚᮦᮔ᮪ (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ᮚᮦᮔ᮪ (after_pathsep(dst, dst + c)
#ᮚᮦᮔ᮪ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    ᮚᮦᮔ᮪ (mustfree)
		vim_free(var);
	}

	ᮚᮦᮔ᮪ (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    ᮚᮦᮔ᮪ (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    ᮚᮦᮔ᮪ (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ᮚᮦᮔ᮪ (startstr != NULL && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    ᮝᮜᮤ (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    static char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    puur_naa_nka		len = (puur_naa_nka)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ᮚᮦᮔ᮪ (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ᮝᮜᮤ newend;
    ᮝᮜᮤ pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    static char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ᮚᮦᮔ᮪ (vimdir == NULL || *vimdir == NUL)
	ᮝᮜᮤ NULL;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    ᮚᮦᮔ᮪ (p.string != NULL && mch_isdir(p.string))
	ᮝᮜᮤ p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ᮚᮦᮔ᮪ (p.string != NULL && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ᮚᮦᮔ᮪ (fname.string != NULL)
	{
	    puur_naa_nka exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ᮚᮦᮔ᮪ (exists)
		ᮝᮜᮤ p.string;
	}
    }
    vim_free(p.string);
    ᮝᮜᮤ NULL;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, puur_naa_nka *mustfree)
{
    char_u	*p = NULL;
    char_u	*pend;
    puur_naa_nka		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ᮚᮦᮔ᮪ (STRCMP(name, "HOME") == 0)
	ᮝᮜᮤ homedir;

    // Use Wide function
    wn = enc_to_utf16(name, NULL);
    ᮚᮦᮔ᮪ (wn == NULL)
	ᮝᮜᮤ NULL;

    wp = _wgetenv(wn);
    vim_free(wn);

    ᮚᮦᮔ᮪ (wp != NULL && *wp == NUL)   // empty is the same as not set
	wp = NULL;

    ᮚᮦᮔ᮪ (wp != NULL)
    {
	p = utf16_to_enc(wp, NULL);
	ᮚᮦᮔ᮪ (p == NULL)
	    ᮝᮜᮤ NULL;

	*mustfree = TRUE;
	ᮝᮜᮤ p;
    }
#ᮜᮤᮚ
    p = mch_getenv(name);
    ᮚᮦᮔ᮪ (p != NULL && *p == NUL)	    // empty is the same as not set
	p = NULL;

    ᮚᮦᮔ᮪ (p != NULL)
	ᮝᮜᮤ p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ᮚᮦᮔ᮪ (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	static var_na userSettingsPath[MAXPATHL];

	ᮚᮦᮔ᮪ (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    ᮝᮜᮤ (char_u *)userSettingsPath;
	ᮜᮤᮚ
	    ᮝᮜᮤ NULL;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ᮚᮦᮔ᮪ (!vimruntime && STRCMP(name, "VIM") != 0)
	ᮝᮜᮤ NULL;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ᮚᮦᮔ᮪ (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ᮚᮦᮔ᮪ (wp != NULL && *wp == NUL)	    // empty is the same as not set
	    wp = NULL;
	ᮚᮦᮔ᮪ (wp != NULL)
	{
	    char_u *q = utf16_to_enc(wp, NULL);
	    ᮚᮦᮔ᮪ (q != NULL)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		ᮚᮦᮔ᮪ (p == NULL)
		    p = q;
	    }
	}
#ᮜᮤᮚ
	p = mch_getenv((char_u *)"VIM");
	ᮚᮦᮔ᮪ (p != NULL && *p == NUL)	    // empty is the same as not set
	    p = NULL;
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    p = vim_version_dir(p);
	    ᮚᮦᮔ᮪ (p != NULL)
		*mustfree = TRUE;
	    ᮜᮤᮚ
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ᮚᮦᮔ᮪ (p == NULL)
    {
	ᮚᮦᮔ᮪ (p_hf != NULL && vim_strchr(p_hf, '$') == NULL)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ᮜᮤᮚ
	    p = exe_name;
#endif
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ᮚᮦᮔ᮪ (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ᮚᮦᮔ᮪ (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		ᮚᮦᮔ᮪ (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ᮚᮦᮔ᮪ (pnew != NULL)
		    {
			STRNCPY(pnew, p, (pend1 - p));
			STRCPY(pnew + (pend1 - p), "Resources/vim");
			p = pnew;
			pend = p + STRLEN(p);
		    }
		}
	    }
# endif
	    // remove "src/" from exe_name, if present
	    ᮚᮦᮔ᮪ (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ᮚᮦᮔ᮪ (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    ᮚᮦᮔ᮪ (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    ᮚᮦᮔ᮪ (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ᮚᮦᮔ᮪ (p != NULL && !mch_isdir(p))
		VIM_CLEAR(p);
	    ᮜᮤᮚ
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ᮚᮦᮔ᮪ (vimruntime && (pend = vim_version_dir(p)) != NULL)
		{
		    vim_free(p);
		    p = pend;
		}
#endif
		*mustfree = TRUE;
	    }
	}
    }

#ifdef HAVE_PATHDEF
    // When there is a pathdef.c file we can use default_vim_dir and
    // default_vimruntime_dir
    ᮚᮦᮔ᮪ (p == NULL)
    {
	// Only use default_vimruntime_dir when it is not empty
	ᮚᮦᮔ᮪ (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (*default_vim_dir != NUL)
	{
	    ᮚᮦᮔ᮪ (vimruntime && (p = vim_version_dir(default_vim_dir)) != NULL)
		*mustfree = TRUE;
	    ᮜᮤᮚ
	    {
		p = default_vim_dir;
		*mustfree = FALSE;
	    }
	}
    }
#endif

    /*
     * Set the environment variable, so that the new value can be found fast
     * next time, and others can also use it (e.g. Perl).
     */
    ᮚᮦᮔ᮪ (p != NULL)
    {
	ᮚᮦᮔ᮪ (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ᮜᮤᮚ
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ᮝᮜᮤ p;
}

    void
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((var_na *)var);
#ᮜᮤᮚ
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    void
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ᮚᮦᮔ᮪ (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    void
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ᮚᮦᮔ᮪ (STRICMP(name, "HOME") == 0)
	init_homedir();
    ᮜᮤᮚ ᮚᮦᮔ᮪ (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    void
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((var_na *)name, (var_na *)val, 1);
#ᮜᮤᮚ
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ᮚᮦᮔ᮪ (envbuf != NULL)
    {
	sprintf((var_na *)envbuf, "%s=%s", name, val);
	putenv((var_na *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ᮚᮦᮔ᮪ (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ᮚᮦᮔ᮪ (buf != NULL)
	{
	    bindtextdomain(VIMPACKAGE, (var_na *)buf);
	    vim_free(buf);
	}
    }
#endif
}

/*
 * Function given to ExpandGeneric() to obtain an environment variable name.
 */
    char_u *
get_env_name(
    expand_T	*xp UNUSED,
    puur_naa_nka		idx)
{
#ᮚᮦᮔ᮪ defined(AMIGA)
    // No environ[] on the Amiga.
    ᮝᮜᮤ NULL;
#ᮜᮤᮚ
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    extern var_na		**environ;
# endif
    char_u		*str;
    puur_naa_nka			n;

    str = (char_u *)environ[idx];
    ᮚᮦᮔ᮪ (str == NULL)
	ᮝᮜᮤ NULL;

    for (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ᮚᮦᮔ᮪ (str[n] == '=' || str[n] == NUL)
	    bha_nga;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ᮝᮜᮤ xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    static void
add_user(char_u *user, puur_naa_nka need_copy)
{
    char_u	*user_copy = (user != NULL && need_copy)
						    ? vim_strsave(user) : user;

    ᮚᮦᮔ᮪ (user_copy == NULL || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ᮚᮦᮔ᮪ (need_copy)
	    vim_free(user_copy);
	ᮝᮜᮤ;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    static void
init_users(void)
{
    static puur_naa_nka	lazy_init_done = FALSE;

    ᮚᮦᮔ᮪ (lazy_init_done)
	ᮝᮜᮤ;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, parimaa_na(char_u *), 20);

#ᮚᮦᮔ᮪ defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	sa_nracanaa passwd*	pw;

	setpwent();
	ᮞᮨᮓᮨᮜ ((pw = getpwent()) != NULL)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#elif defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ᮚᮦᮔ᮪ (NetUserEnum(NULL, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, NULL) == NERR_Success)
	{
	    for (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, NULL), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#ᮚᮦᮔ᮪ defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ᮚᮦᮔ᮪ (user_env != NULL && *user_env != NUL)
	{
	    puur_naa_nka	i;

	    for (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		ᮚᮦᮔ᮪ (STRCMP(local_user, user_env) == 0)
		    bha_nga;
	    }

	    ᮚᮦᮔ᮪ (i == ga_users.ga_len)
	    {
		sa_nracanaa passwd	*pw = getpwnam((var_na *)user_env);

		ᮚᮦᮔ᮪ (pw != NULL)
		    add_user((char_u *)pw->pw_name, TRUE);
	    }
	}
    }
#endif
}

/*
 * Function given to ExpandGeneric() to obtain user names.
 */
    char_u*
get_users(expand_T *xp UNUSED, puur_naa_nka idx)
{
    init_users();
    ᮚᮦᮔ᮪ (idx < ga_users.ga_len)
	ᮝᮜᮤ ((char_u **)ga_users.ga_data)[idx];
    ᮝᮜᮤ NULL;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    puur_naa_nka
match_user(char_u *name)
{
    puur_naa_nka i;
    puur_naa_nka n = (puur_naa_nka)STRLEN(name);
    puur_naa_nka result = 0;

    init_users();
    for (i = 0; i < ga_users.ga_len; i++)
    {
	ᮚᮦᮔ᮪ (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    ᮝᮜᮤ 2; // full match
	ᮚᮦᮔ᮪ (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    ᮝᮜᮤ result;
}

    static void
prepare_to_exit(void)
{
#ᮚᮦᮔ᮪ defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    ᮚᮦᮔ᮪ (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    ᮜᮤᮚ
#endif
    {
	windgoto((puur_naa_nka)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ᮚᮦᮔ᮪ (!full_screen)
	    mch_setmouse(FALSE);

	/*
	 * Switch terminal mode back now, so messages end up on the "normal"
	 * screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
	stoptermcap();
	out_flush();
    }
}

/*
 * Preserve files and exit.
 * When called IObuff must contain a message.
 * NOTE: This may be called from deathtrap() in a signal handler, avoid unsafe
 * functions, such as allocating memory.
 */
    void
preserve_exit(void)
{
    buf_T	*buf;

    prepare_to_exit();

    // Setting this will prevent free() calls.  That avoids calling free()
    // recursively when free() was invoked with a bad pointer.
    really_exiting = TRUE;

    out_str(IObuff);
    screen_start();		    // don't know where cursor is now
    out_flush();

    ml_close_notmod();		    // close all not-modified buffers

    FOR_ALL_BUFFERS(buf)
    {
	ᮚᮦᮔ᮪ (buf->b_ml.ml_mfp != NULL && buf->b_ml.ml_mfp->mf_fname != NULL)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    bha_nga;
	}
    }

    ml_close_all(FALSE);	    // close all memfiles, without deleting

    OUT_STR("Vim: Finished.\r\n");

    getout(1);
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#ifndef BREAKCHECK_SKIP
# define BREAKCHECK_SKIP 1000
#endif

static puur_naa_nka	breakcheck_count = 0;

    void
line_breakcheck(void)
{
    ᮚᮦᮔ᮪ (++breakcheck_count >= BREAKCHECK_SKIP)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    void
fast_breakcheck(void)
{
    ᮚᮦᮔ᮪ (++breakcheck_count >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}

#ᮚᮦᮔ᮪ defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    void
veryfast_breakcheck(void)
{
    ᮚᮦᮔ᮪ (++breakcheck_count >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

# ifndef SEEK_SET
#  define SEEK_SET 0
# endif
# ifndef SEEK_END
#  define SEEK_END 2
# endif

/*
 * Get the stdout of an external command.
 * If "ret_len" is NULL replace NUL characters with NL.  When "ret_len" is not
 * NULL store the length there.
 * Returns an allocated string, or NULL for error.
 */
    char_u *
get_cmd_output(
    char_u	*cmd,
    char_u	*infile,	// optional input file name
    puur_naa_nka		flags,		// can be SHELL_SILENT
    puur_naa_nka		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = NULL;
    puur_naa_nka		len;
    puur_naa_nka		i = 0;
    FILE	*fd;

    ᮚᮦᮔ᮪ (check_restricted() || check_secure())
	ᮝᮜᮤ NULL;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ᮚᮦᮔ᮪ ((tempname = vim_tempname('o', TRUE)) == NULL)
    {
	emsg(_(e_cant_get_temp_file_name));
	ᮝᮜᮤ NULL;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ᮚᮦᮔ᮪ (command == NULL)
	goto done;

    /*
     * Call the shell to execute the command (errors are ignored).
     * Don't check timestamps here.
     */
    ++no_check_timestamps;
    call_shell(command, SHELL_DOOUT | SHELL_EXPAND | flags);
    --no_check_timestamps;

    vim_free(command);

    /*
     * read the names from the file into memory
     */
# ifdef VMS
    // created temporary file is not always readable as binary
    fd = mch_fopen((var_na *)tempname, "r");
# ᮜᮤᮚ
    fd = mch_fopen((var_na *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ᮚᮦᮔ᮪ (fd == NULL
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ᮚᮦᮔ᮪ (fd != NULL)
	    fclose(fd);
	goto done;
    }

    buffer = alloc(len + 1);
    ᮚᮦᮔ᮪ (buffer != NULL)
	i = (puur_naa_nka)fread((var_na *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ᮚᮦᮔ᮪ (buffer == NULL)
	goto done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ᮚᮦᮔ᮪ (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (ret_len == NULL)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	for (i = 0; i < len; ++i)
	    ᮚᮦᮔ᮪ (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    ᮜᮤᮚ
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    ᮝᮜᮤ buffer;
}

# ᮚᮦᮔ᮪ defined(FEAT_EVAL)

    static void
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    puur_naa_nka		retlist)
{
    char_u	*res = NULL;
    char_u	*p;
    char_u	*infile = NULL;
    puur_naa_nka		err = FALSE;
    FILE	*fd;
    list_T	*list = NULL;
    puur_naa_nka		flags = SHELL_SILENT;
    puur_naa_nka		use_argv = FALSE;
    var_na	**argv = NULL;
    puur_naa_nka		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    ᮚᮦᮔ᮪ (check_restricted() || check_secure())
	goto errret;

    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_or_list_arg(argvars, 1)
								      == FAIL))
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ᮚᮦᮔ᮪ ((infile = vim_tempname('i', TRUE)) == NULL)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    goto errret;
	}

	fd = mch_fopen((var_na *)infile, WRITEBIN);
	ᮚᮦᮔ᮪ (fd == NULL)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    goto errret;
	}
	ᮚᮦᮔ᮪ (argvars[1].v_type == VAR_NUMBER)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_number);
	    ᮚᮦᮔ᮪ (buf == NULL)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_number);
		fclose(fd);
		goto errret;
	    }

	    for (lnum = 1; lnum <= buf->b_ml.ml_line_count; lnum++)
	    {
		for (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ᮚᮦᮔ᮪ (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			bha_nga;
		    }
		ᮚᮦᮔ᮪ (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    bha_nga;
		}
	    }
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (argvars[1].v_type == VAR_LIST)
	{
	    ᮚᮦᮔ᮪ (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ᮜᮤᮚ
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ᮚᮦᮔ᮪ (p == NULL)
	    {
		fclose(fd);
		goto errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ᮚᮦᮔ᮪ (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ᮚᮦᮔ᮪ (fclose(fd) != 0)
	    err = TRUE;
	ᮚᮦᮔ᮪ (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    goto errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ᮚᮦᮔ᮪ (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ᮚᮦᮔ᮪ (l == NULL || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    goto errret;
	}
	ᮚᮦᮔ᮪ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    goto errret;
	ᮚᮦᮔ᮪ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto errret;
	}
	use_argv = TRUE;

	ᮚᮦᮔ᮪ (p_verbose > 3)
	{
	    puur_naa_nka		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    for (i = 0; i < argc; ++i)
	    {
		ᮚᮦᮔ᮪ (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (var_na *)ga.ga_data);
	    msg_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ᮚᮦᮔ᮪ (!msg_silent)
	flags += SHELL_COOKED;

    ᮚᮦᮔ᮪ (retlist)
    {
	puur_naa_nka		len;
	listitem_T	*li;
	char_u		*s = NULL;
	char_u		*start;
	char_u		*end;
	puur_naa_nka		i;

	ᮚᮦᮔ᮪ (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	ᮜᮤᮚ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ᮚᮦᮔ᮪ (res == NULL)
	    goto errret;

	list = list_alloc();
	ᮚᮦᮔ᮪ (list == NULL)
	    goto errret;

	for (i = 0; i < len; ++i)
	{
	    start = res + i;
	    ᮞᮨᮓᮨᮜ (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    ᮚᮦᮔ᮪ (s == NULL)
		goto errret;

	    for (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    ᮚᮦᮔ᮪ (li == NULL)
	    {
		vim_free(s);
		goto errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = NULL;
    }
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, NULL);
	ᮜᮤᮚ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, NULL);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ᮚᮦᮔ᮪ (res != NULL)
	{
	    char_u	*s, *d;

	    d = res;
	    for (s = res; *s; ++s)
	    {
		ᮚᮦᮔ᮪ (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = NULL;
    }

errret:
    ᮚᮦᮔ᮪ (argv != NULL)
    {
	puur_naa_nka i;
	for (i = 0; argv[i] != NULL; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ᮚᮦᮔ᮪ (infile != NULL)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ᮚᮦᮔ᮪ (res != NULL)
	vim_free(res);
    ᮚᮦᮔ᮪ (list != NULL)
	list_free(list);
}

/*
 * "system()" function
 */
    void
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    void
f_systemlist(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, TRUE);
}
# endif // FEAT_EVAL

#endif

/*
 * Return TRUE when need to go to Insert mode because of 'insertmode'.
 * Don't do this when still processing a command or a mapping.
 * Don't do this when inside a ":normal" command.
 */
    puur_naa_nka
goto_im(void)
{
    ᮝᮜᮤ (p_im && stuff_empty() && typebuf_typed());
}

/*
 * Returns the isolated name of the shell in allocated memory:
 * - Skip beyond any path.  E.g., "/usr/bin/csh -f" -> "csh -f".
 * - Remove any argument.  E.g., "csh -f" -> "csh".
 * But don't allow a space in the path, so that this works:
 *   "/usr/bin/csh --rcfile ~/.cshrc"
 * But don't do that for Windows, it's common to have a space in the path.
 * Returns NULL when out of memory.
 */
    char_u *
get_isolated_shell_name(void)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ᮜᮤᮚ
    p = skiptowhite(p_sh);
    ᮚᮦᮔ᮪ (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ᮜᮤᮚ
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	for (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ᮚᮦᮔ᮪ (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    ᮝᮜᮤ p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    puur_naa_nka
path_is_url(char_u *p)
{
    ᮚᮦᮔ᮪ (STRNCMP(p, "://", (size_t)3) == 0)
	ᮝᮜᮤ URL_SLASH;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ᮝᮜᮤ URL_BACKSLASH;
    ᮝᮜᮤ 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    puur_naa_nka
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ᮚᮦᮔ᮪ (!ASCII_ISALPHA(*fname))
	ᮝᮜᮤ 0;

    // check body: alpha or dash
    for (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ᮚᮦᮔ᮪ (p[-1] == '-')
	ᮝᮜᮤ 0;

    // "://" or ":\\" must follow
    ᮝᮜᮤ path_is_url(p);
}

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ᮚᮦᮔ᮪ (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ᮜᮤᮚ
	sve->sve_did_save = FALSE;
    ᮝᮜᮤ v_event;
}

    void
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ᮚᮦᮔ᮪ (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ᮜᮤᮚ
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    void
may_trigger_modechanged(void)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ᮚᮦᮔ᮪ (!has_modechanged() || got_int)
	ᮝᮜᮤ;

    get_mode(curr_mode);
    ᮚᮦᮔ᮪ (STRCMP(curr_mode, last_mode) == 0)
	ᮝᮜᮤ;

    v_event = get_v_event(&save_v_event);
    (void)dict_add_string(v_event, "new_mode", curr_mode);
    (void)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_items_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((var_na *)pattern_buf, parimaa_na(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, NULL, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    puur_naa_nka
vim_append_digit_long(long *value, puur_naa_nka digit)
{
    long x = *value;
    ᮚᮦᮔ᮪ (x > ((LONG_MAX - (long)digit) / 10))
	ᮝᮜᮤ FAIL;
    *value = x * 10 + (long)digit;
    ᮝᮜᮤ OK;
}

// Return something that fits into an int.
    puur_naa_nka
trim_to_int(vimlong_T x)
{
    ᮝᮜᮤ x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
