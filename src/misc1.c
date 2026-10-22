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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

#যদি defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#যদি defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
স্থির garray_T	ga_users;

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
    পূর্ণসংখ্যা
get_leader_len(
    char_u	*সারি,
    char_u	**flags,
    পূর্ণসংখ্যা		backward,
    পূর্ণসংখ্যা		include_space)
{
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		ফলাফল;
    পূর্ণসংখ্যা		got_com = FALSE;
    পূর্ণসংখ্যা		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    পূর্ণসংখ্যা		মধ্য_match_len = 0;
    char_u	*পূর্ব_list;
    char_u	*saved_flags = ফাঁকা;

    ফলাফল = i = 0;
    যখন (VIM_ISWHITE(সারি[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    যখন (সারি[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	জন্য (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    যদি (!got_com && flags != ফাঁকা)
		*flags = list;	    // remember where flags started
	    পূর্ব_list = list;
	    (শূন্য)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    যদি (string == ফাঁকা)	    // missing ':', ignore this part
		চালু;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    যদি (মধ্য_match_len != 0
		    && vim_strchr(part_buf, COM_মধ্য) == ফাঁকা
		    && vim_strchr(part_buf, COM_END) == ফাঁকা)
		বিরতি;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    যদি (got_com && vim_strchr(part_buf, COM_NEST) == ফাঁকা)
		চালু;

	    // When 'O' flag present and using "O" command skip this one.
	    যদি (backward && vim_strchr(part_buf, COM_NOBACK) != ফাঁকা)
		চালু;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    যদি (VIM_ISWHITE(string[0]))
	    {
		যদি (i == 0 || !VIM_ISWHITE(সারি[i - 1]))
		    চালু;  // missing white space
		যখন (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    জন্য (j = 0; string[j] != NUL && string[j] == সারি[i + j]; ++j)
		;
	    যদি (string[j] != NUL)
		চালু;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    যদি (vim_strchr(part_buf, COM_BLANK) != ফাঁকা
			   && !VIM_ISWHITE(সারি[i + j]) && সারি[i + j] != NUL)
		চালু;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    যদি (vim_strchr(part_buf, COM_মধ্য) != ফাঁকা)
	    {
		যদি (মধ্য_match_len == 0)
		{
		    মধ্য_match_len = j;
		    saved_flags = পূর্ব_list;
		}
		চালু;
	    }
	    যদি (মধ্য_match_len != 0 && j > মধ্য_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		মধ্য_match_len = 0;

	    যদি (মধ্য_match_len == 0)
		i += j;
	    found_one = TRUE;
	    বিরতি;
	}

	যদি (মধ্য_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    যদি (!got_com && flags != ফাঁকা)
		*flags = saved_flags;
	    i += মধ্য_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	যদি (!found_one)
	    বিরতি;

	ফলাফল = i;

	// Include any trailing white space.
	যখন (VIM_ISWHITE(সারি[i]))
	    ++i;

	যদি (include_space)
	    ফলাফল = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	যদি (vim_strchr(part_buf, COM_NEST) == ফাঁকা)
	    বিরতি;
    }
    ফেরত ফলাফল;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    পূর্ণসংখ্যা
get_last_leader_offset(char_u *সারি, char_u **flags)
{
    পূর্ণসংখ্যা		ফলাফল = -1;
    পূর্ণসংখ্যা		i, j;
    পূর্ণসংখ্যা		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    পূর্ণসংখ্যা		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (পূর্ণসংখ্যা)STRLEN(সারি);
    যখন (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	জন্য (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (শূন্য)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    যদি (string == ফাঁকা)	// If everything is fine, this cannot actually
				// happen.
		চালু;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    যদি (VIM_ISWHITE(string[0]))
	    {
		যদি (i == 0 || !VIM_ISWHITE(সারি[i - 1]))
		    চালু;
		যখন (VIM_ISWHITE(*string))
		    ++string;
	    }
	    জন্য (j = 0; string[j] != NUL && string[j] == সারি[i + j]; ++j)
		/* do nothing */;
	    যদি (string[j] != NUL)
		চালু;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    যদি (vim_strchr(part_buf, COM_BLANK) != ফাঁকা
		    && !VIM_ISWHITE(সারি[i + j]) && সারি[i + j] != NUL)
		চালু;

	    যদি (vim_strchr(part_buf, COM_মধ্য) != ফাঁকা)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		জন্য (j = 0; VIM_ISWHITE(সারি[j]) && j <= i; j++)
		    ;
		যদি (j < i)
		    চালু;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    যদি (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    বিরতি;
	}

	যদি (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    পূর্ণসংখ্যা     len1, len2, off;

	    ফলাফল = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    যদি (vim_strchr(part_buf, COM_NEST) != ফাঁকা)
		চালু;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    যখন (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (পূর্ণসংখ্যা)STRLEN(com_leader);

	    জন্য (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(শূন্য)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		যদি (flags_save == com_flags)
		    চালু;
		string = vim_strchr(part_buf2, ':');
		++string;
		যখন (VIM_ISWHITE(*string))
		    ++string;
		len2 = (পূর্ণসংখ্যা)STRLEN(string);
		যদি (len2 == 0)
		    চালু;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		জন্য (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    যদি (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			যদি (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ফেরত ফলাফল;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    পূর্ণসংখ্যা
plines(linenr_T lnum)
{
    ফেরত plines_win(curwin, lnum, TRUE);
}

    পূর্ণসংখ্যা
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    পূর্ণসংখ্যা		limit_winheight)	// when TRUE limit to window height
{
#যদি defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ফেরত plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    পূর্ণসংখ্যা
plines_nofill(linenr_T lnum)
{
    ফেরত plines_win_nofill(curwin, lnum, TRUE);
}

    পূর্ণসংখ্যা
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    পূর্ণসংখ্যা		limit_winheight)	// when TRUE limit to window height
{
#endif
    পূর্ণসংখ্যা		সারিগুলি;

    যদি (wp->w_width == 0)
	ফেরত 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    যদি (সারি_Folded(wp, lnum) == TRUE)
	ফেরত 1;
#endif

    যদি (!wp->w_p_wrap)
	সারিগুলি = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_গুনতি_above_below(wp->w_বাফার, lnum)
#endif
	;
    অন্যথা
	সারিগুলি = plines_win_nofold(wp, lnum);

    যদি (limit_winheight && সারিগুলি > wp->w_height)
	ফেরত wp->w_height;
    ফেরত সারিগুলি;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    পূর্ণসংখ্যা
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    পূর্ণসংখ্যা		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_বাফার, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    যদি (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	ফেরত 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (পূর্ণসংখ্যা)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    যদি (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    যদি (width <= 0)
	ফেরত 32000;
    যদি (col <= width)
	ফেরত 1;
    col -= width;
    width += win_col_off2(wp);
    ফেরত (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    পূর্ণসংখ্যা
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    পূর্ণসংখ্যা		সারিগুলি = 0;
    পূর্ণসংখ্যা		width;
    char_u	*সারি;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    সারিগুলি = diff_check_fill(wp, lnum);
#endif

    যদি (!wp->w_p_wrap)
	ফেরত সারিগুলি + 1;

    যদি (wp->w_width == 0)
	ফেরত সারিগুলি + 1;

    সারি = ml_get_buf(wp->w_বাফার, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, সারি, সারি);
    // "column" is a byte index, advance the pointer until it is reached.
    যখন (*cts.cts_ptr != NUL && cts.cts_ptr < সারি + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ফাঁকা, ফাঁকা);
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
    যদি (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, ফাঁকা, ফাঁকা) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    যদি (width <= 0)
	ফেরত 9999;

    সারিগুলি += 1;
    যদি (col > width)
	সারিগুলি += (col - width) / (width + win_col_off2(wp)) + 1;
    ফেরত সারিগুলি;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    পূর্ণসংখ্যা
plines_m_win(win_T *wp, linenr_T first, linenr_T last, পূর্ণসংখ্যা max)
{
    পূর্ণসংখ্যা		গুনতি = 0;

    যখন (first <= last && গুনতি < max)
    {
#ifdef FEAT_FOLDING
	পূর্ণসংখ্যা	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_গুনতি(wp, first, ফাঁকা);
	যদি (x > 0)
	{
	    ++গুনতি;	    // count 1 for "+-- folded" line
	    first += x;
	}
	অন্যথা
#endif
	{
#ifdef FEAT_DIFF
	    যদি (first == wp->w_topline)
		গুনতি += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    অন্যথা
#endif
		গুনতি += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    যদি (first == wp->w_বাফার->b_ml.ml_সারি_গুনতি + 1)
	গুনতি += diff_check_fill(wp, first);
#endif
    ফেরত MIN(max, গুনতি);
}

    পূর্ণসংখ্যা
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    পূর্ণসংখ্যা		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    যদি (pos->col == MAXCOL)
	ফেরত NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    যদি (pos->col > ptrlen)
	ফেরত NUL;
    যদি (has_mbyte)
	ফেরত (*mb_ptr2char)(ptr);
    ফেরত (পূর্ণসংখ্যা)*ptr;
}

    পূর্ণসংখ্যা
gchar_cursor(শূন্য)
{
    যদি (has_mbyte)
	ফেরত (*mb_ptr2char)(ml_get_cursor());
    ফেরত (পূর্ণসংখ্যা)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    পূর্ণসংখ্যা
char_before_cursor(শূন্য)
{
    যদি (curwin->w_cursor.col == 0)
	ফেরত -1;

    char_u *সারি = ml_get_curline();

    যদি (has_mbyte)
    {
	char_u *p = সারি + curwin->w_cursor.col;
	পূর্ণসংখ্যা পূর্ব_len = (*mb_শীর্ষ_off)(সারি, p - 1) + 1;
	ফেরত mb_ptr2char(p - পূর্ব_len);
    }
    ফেরত সারি[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    শূন্য
pchar_cursor(পূর্ণসংখ্যা c)
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
    যদি (*p == ',')
	++p;
    যখন (*p == ' ')
	++p;
    ফেরত p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    শূন্য
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_বাফার == buf && wp->w_status_height)
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
    পূর্ণসংখ্যা
ask_yesno(char_u *str, পূর্ণসংখ্যা direct)
{
    পূর্ণসংখ্যা	    r = ' ';
    পূর্ণসংখ্যা	    save_State = State;

    যদি (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_চাবিগুলি;		// no mapping here, but recognize keys

    যখন (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	যদি (direct)
	    r = get_keystroke();
	অন্যথা
	    r = plain_vgetc();
	যদি (r == Ctrl_C || r == ESC)
	    r = 'n';
	বার্তা_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_চাবিগুলি;

    ফেরত r;
}

#যদি defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    শূন্য
get_mode(char_u *buf)
{
    পূর্ণসংখ্যা		i = 0;

    যদি (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    অন্যথা যদি (term_use_loop())
    {
	যদি (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    অন্যথা যদি (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	যদি (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	অন্যথা যদি (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    অন্যথা যদি (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    অন্যথা যদি (State & MODE_ঢোকাও)
    {
	যদি (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	অন্যথা
	{
	    যদি (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    অন্যথা
		buf[i++] = 'i';
	}

	যদি (ins_compl_active())
	    buf[i++] = 'c';
	অন্যথা যদি (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    অন্যথা যদি ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	যদি (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	অন্যথা যদি (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	যদি ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    অন্যথা যদি (VIsual_active)
    {
	যদি (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	অন্যথা
	{
	    buf[i++] = VIsual_mode;
	    যদি (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    অন্যথা
    {
	buf[i++] = 'n';
	যদি (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	অন্যথা যদি (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	অন্যথা যদি (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    শূন্য
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_দৈর্ঘ্য];

    যদি (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ফেরত;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    যদি (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    স্থির শূন্য
may_add_state_char(garray_T *gap, char_u *include, পূর্ণসংখ্যা c)
{
    যদি (include == ফাঁকা || vim_strchr(include, c) != ফাঁকা)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    শূন্য
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ফাঁকা;
    পূর্ণসংখ্যা		i;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    ga_init2(&ga, 1, 20);
    যদি (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    যদি (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == ফাঁকা))
	may_add_state_char(&ga, include, 'm');
    যদি (op_pending())
	may_add_state_char(&ga, include, 'o');
    যদি (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    যদি (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    যদি (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    যদি (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    জন্য (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    যদি (বার্তা_scrolled > 0)
	may_add_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_তথ্য;
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
    পূর্ণসংখ্যা
get_keystroke(শূন্য)
{
    char_u	*buf = ফাঁকা;
    পূর্ণসংখ্যা		buflen = 150;
    পূর্ণসংখ্যা		maxlen;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		save_mapped_ctrl_c = mapped_ctrl_c;
    পূর্ণসংখ্যা		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    জন্য (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	যদি (buf == ফাঁকা)
	    buf = alloc(buflen);
	অন্যথা যদি (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    যদি (buf == ফাঁকা)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	যদি (buf == ফাঁকা)
	{
	    do_outofmem_বার্তা((long_u)buflen);
	    ফেরত ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	যদি (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_প্রবেশ_বাফার(buf + len, n);
	    len += n;
	    waited = 0;
	}
	অন্যথা যদি (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	যদি ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    চালু;

	যদি (n == KEYLEN_REMOVED)  // key code removed
	{
	    যদি (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    চালু;
	}
	যদি (n > 0)		// found a termcode: adjust length
	    len = n;
	যদি (len == 0)		// nothing typed yet
	    চালু;

	// Handle modifier and/or special key code.
	n = buf[0];
	যদি (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    যদি (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_চাবি(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		যদি (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		যদি (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		চালু;
	    }
	    বিরতি;
	}
	যদি (has_mbyte)
	{
	    যদি (MB_BYTE2LEN(n) > len)
		চালু;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	যদি (n == intr_char)
	    n = ESC;
#endif
	বিরতি;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ফেরত merge_modify_Other_চাবিগুলি(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    স্থির পূর্ণসংখ্যা
vim_append_digit_int(পূর্ণসংখ্যা *মান, পূর্ণসংখ্যা digit)
{
    পূর্ণসংখ্যা x = *মান;
    যদি (x > ((INT_MAX - digit) / 10))
	ফেরত FAIL;
    *মান = x * 10 + digit;
    ফেরত OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    পূর্ণসংখ্যা
get_সংখ্যা(
    পূর্ণসংখ্যা	    colon,			// allow colon to abort
    পূর্ণসংখ্যা	    *mouse_used)
{
    পূর্ণসংখ্যা	n = 0;
    পূর্ণসংখ্যা	c;
    পূর্ণসংখ্যা typed = 0;

    যদি (mouse_used != ফাঁকা)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    যদি (বার্তা_silent != 0)
	ফেরত 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_চাবিগুলি;		// no mapping here, but recognize keys
    জন্য (;;)
    {
	windgoto(বার্তা_row, cmdline_col_off + বার্তা_col);
	c = safe_vgetc();
	যদি (VIM_ISDIGIT(c))
	{
	    যদি (vim_append_digit_int(&n, c - '0') == FAIL)
		ফেরত 0;
	    বার্তা_putchar(c);
	    ++typed;
	}
	অন্যথা যদি (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    যদি (typed > 0)
	    {
		বার্তা_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	অন্যথা যদি (mouse_used != ফাঁকা && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    বিরতি;
	}
	অন্যথা যদি (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    যদি (!exmode_active)
		cmdline_row = বার্তা_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    বিরতি;
	}
	অন্যথা যদি (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    বিরতি;
	}
	অন্যথা যদি (c == CAR || c == NL )
	    বিরতি;
    }
    --no_mapping;
    --allow_চাবিগুলি;
    ফেরত n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    পূর্ণসংখ্যা
prompt_for_সংখ্যা(পূর্ণসংখ্যা *mouse_used)
{
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		save_cmdline_row;
    পূর্ণসংখ্যা		save_State;

    // When using ":silent" assume that <CR> was entered.
    যদি (mouse_used != ফাঁকা)
	বার্তা_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    অন্যথা
	বার্তা_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_সংখ্যা(TRUE, mouse_used);
    যদি (চাবি_Typed)
    {
	// don't call wait_return() now
	যদি (বার্তা_row > 0)
	    cmdline_row = বার্তা_row - 1;
	need_wait_return = FALSE;
	বার্তা_didany = FALSE;
	বার্তা_didout = FALSE;
    }
    অন্যথা
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ফেরত i;
}

    শূন্য
msgmore(long n)
{
    long pn;

    যদি (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ফেরত;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    যদি (keep_বার্তা != ফাঁকা && !keep_বার্তা_more)
	ফেরত;

    যদি (n > 0)
	pn = n;
    অন্যথা
	pn = -n;

    যদি (pn > p_report)
    {
	যদি (n > 0)
	    vim_snprintf(বার্তা_buf, বার্তা_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	অন্যথা
	    vim_snprintf(বার্তা_buf, বার্তা_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	যদি (got_int)
	    vim_strcat((char_u *)বার্তা_buf, (char_u *)_(" (Interrupted)"),
								  বার্তা_BUF_LEN);
	যদি (বার্তা(বার্তা_buf))
	{
	    set_keep_বার্তা((char_u *)বার্তা_buf, 0);
	    keep_বার্তা_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    শূন্য
beep_flush(শূন্য)
{
    যদি (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    শূন্য
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    যদি (emsg_silent != 0 || in_assert_fails)
	ফেরত;

    যদি (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	স্থির পূর্ণসংখ্যা		did_init = FALSE;
	স্থির elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	যদি (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    যদি (p_vb
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
		যদি (has_vtp_working() != 0
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
	    অন্যথা
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    যদি (vim_strchr(p_debug, 'e') != ফাঁকা)
    {
	বার্তা_source(HL_ATTR(HLF_W));
	বার্তা_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    শূন্য
init_homedir(শূন্য)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#অন্যথা
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    যদি (var == ফাঁকা || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	যদি (homepath == ফাঁকা || *homepath == NUL)
	    homepath = (char_u *)"\\";
	যদি (homedrive != ফাঁকা
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((অক্ষর *)NameBuff, "%s%s", homedrive, homepath);
	    যদি (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    যদি (var == ফাঁকা)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    যদি (var != ফাঁকা && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	যদি (p != ফাঁকা)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    যদি (exp != ফাঁকা && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((অক্ষর *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    যদি (var != ফাঁকা && *var == NUL)	// empty is same as not set
	var = ফাঁকা;

    যদি (enc_utf8 && var != ফাঁকা)
    {
	পূর্ণসংখ্যা	len;
	char_u  *pp = ফাঁকা;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (পূর্ণসংখ্যা)STRLEN(var), &pp, &len);
	যদি (pp != ফাঁকা)
	{
	    homedir = pp;
	    ফেরত;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    যদি (var == ফাঁকা)
	var = (char_u *)"C:/";
#endif

    যদি (var != ফাঁকা)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	যদি (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((অক্ষর *)NameBuff) == 0)
	{
	    যদি (!mch_chdir((অক্ষর *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    যদি (mch_chdir((অক্ষর *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#যদি defined(EXITFREE)
    শূন্য
free_homedir(শূন্য)
{
    vim_free(homedir);
}

    শূন্য
free_users(শূন্য)
{
    ga_clear_strings(&ga_users);
}
#endif

#যদি defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    শূন্য
init_vimdir(শূন্য)
{
    পূর্ণসংখ্যা	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    যদি (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    যদি (mustfree)
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
    ফেরত expand_env_save_opt(src, FALSE, ফাঁকা);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, পূর্ণসংখ্যা one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    যদি (p != ফাঁকা)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ফাঁকা);
    ফেরত p;
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
    পূর্ণসংখ্যা		dstlen)		// maximum length of the result
{
    ফেরত expand_env_esc(src, dst, dstlen, ফাঁকা, FALSE, ফাঁকা);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    পূর্ণসংখ্যা		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    পূর্ণসংখ্যা		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*লেজ;
    পূর্ণসংখ্যা		c;
    char_u	*var;
    পূর্ণসংখ্যা		copy_char;
    পূর্ণসংখ্যা		mustfree;	// var was allocated, need to free it later
    পূর্ণসংখ্যা		at_start = TRUE; // at start of a name
    পূর্ণসংখ্যা		startstr_len = 0;
    char_u	*dst_start = dst;

    যদি (startstr != ফাঁকা)
	startstr_len = (পূর্ণসংখ্যা)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    যখন (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	যদি (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (শূন্য)skip_রাশিমালা(&src, ফাঁকা);
	    যদি (*src == '`')
		++src;
	    len = src - var;
	    যদি (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (পূর্ণসংখ্যা)len;
	    চালু;
	}
#endif
	copy_char = TRUE;
	যদি ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#যদি defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    যদি (*src != '~')				// environment var
	    {
		লেজ = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		যদি (*লেজ == '{' && !vim_isIDc('{'))
		{
		    লেজ++;	// ignore '{'
		    যখন (c-- > 0 && *লেজ && *লেজ != '}')
			*var++ = *লেজ++;
		}
		অন্যথা
#endif
		{
		    যখন (c-- > 0 && *লেজ != NUL && ((vim_isIDc(*লেজ))
#যদি defined(MSWIN)
			    || (*src == '%' && *লেজ != '%')
#endif
			    ))
			*var++ = *লেজ++;
		}

#যদি defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		যদি (src[1] == '{' && *লেজ != '}')
# অন্যথা
		যদি (*src == '%' && *লেজ != '%')
# endif
		    var = ফাঁকা;
		অন্যথা
		{
# ifdef UNIX
		    যদি (src[1] == '{')
# অন্যথা
		    যদি (*src == '%')
# endif
			++লেজ;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#যদি defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    অন্যথা যদি (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ফাঁকা)
	    {
		var = homedir;
		লেজ = src + 1;
	    }
	    অন্যথা					// user directory
	    {
#যদি defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		লেজ = src;
		var = dst;
		c = dstlen - 1;
		যখন (	   c-- > 0
			&& *লেজ
			&& vim_isfilec(*লেজ)
			&& !vim_ispathsep(*লেজ))
		    *var++ = *লেজ++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  যদি defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    কাঠামো passwd *pw = (*dst == NUL)
					? ফাঁকা : getpwnam((অক্ষর *)dst + 1);

		    var = (pw == ফাঁকা) ? ফাঁকা : (char_u *)pw->pw_dir;
		}
		যদি (var == ফাঁকা)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ফাঁকা,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# অন্যথা	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*path, *পরবর্তী_path, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    পরবর্তী_path = paths;
		    যখন (*পরবর্তী_path)
		    {
			জন্য (path = পরবর্তী_path; *পরবর্তী_path && *পরবর্তী_path != ',';
				পরবর্তী_path++);
			যদি (*পরবর্তী_path)
			    *পরবর্তী_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (অক্ষর *)test,
			    আকার(test),
			    "%s/%s",
			    path,
			    dst + 1);
			যদি (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    যদি (var != ফাঁকা)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    বিরতি;
			}
		    }
		}
# endif // UNIX
#অন্যথা
		// cannot expand user's home directory, so don't try
		var = ফাঁকা;
		লেজ = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    যদি (p_ssl && var != ফাঁকা && vim_strchr(var, '\\') != ফাঁকা)
	    {
		char_u	*p = vim_strsave(var);

		যদি (p != ফাঁকা)
		{
		    যদি (mustfree)
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
	    যদি (esc_chars != ফাঁকা && var != ফাঁকা
		    && vim_strpbrk(var, esc_chars) != ফাঁকা)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		যদি (p != ফাঁকা)
		{
		    যদি (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    যদি (var != ফাঁকা && *var != NUL)
	    {
		c = (পূর্ণসংখ্যা)STRLEN(var);

		যদি (c + STRLEN(লেজ) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    যদি (after_pathsep(dst, dst + c)
#যদি defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*লেজ))
			++লেজ;
		    dst += c;
		    src = লেজ;
		    copy_char = FALSE;
		}
	    }
	    যদি (mustfree)
		vim_free(var);
	}

	যদি (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    যদি (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    অন্যথা যদি ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    যদি (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		যদি (startstr != ফাঁকা && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    ফেরত (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    স্থির char_u *
remove_লেজ(char_u *p, char_u *pend, char_u *name)
{
    পূর্ণসংখ্যা		len = (পূর্ণসংখ্যা)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    যদি (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ফেরত newend;
    ফেরত pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    স্থির char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    যদি (vimdir == ফাঁকা || *vimdir == NUL)
	ফেরত ফাঁকা;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    যদি (p.string != ফাঁকা && mch_isdir(p.string))
	ফেরত p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    যদি (p.string != ফাঁকা && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.দৈর্ঘ্য,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	যদি (fname.string != ফাঁকা)
	{
	    পূর্ণসংখ্যা exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    যদি (exists)
		ফেরত p.string;
	}
    }
    vim_free(p.string);
    ফেরত ফাঁকা;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, পূর্ণসংখ্যা *mustfree)
{
    char_u	*p = ফাঁকা;
    char_u	*pend;
    পূর্ণসংখ্যা		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    যদি (STRCMP(name, "HOME") == 0)
	ফেরত homedir;

    // Use Wide function
    wn = enc_to_utf16(name, ফাঁকা);
    যদি (wn == ফাঁকা)
	ফেরত ফাঁকা;

    wp = _wgetenv(wn);
    vim_free(wn);

    যদি (wp != ফাঁকা && *wp == NUL)   // empty is the same as not set
	wp = ফাঁকা;

    যদি (wp != ফাঁকা)
    {
	p = utf16_to_enc(wp, ফাঁকা);
	যদি (p == ফাঁকা)
	    ফেরত ফাঁকা;

	*mustfree = TRUE;
	ফেরত p;
    }
#অন্যথা
    p = mch_getenv(name);
    যদি (p != ফাঁকা && *p == NUL)	    // empty is the same as not set
	p = ফাঁকা;

    যদি (p != ফাঁকা)
	ফেরত p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    যদি (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	স্থির অক্ষর userSettingsPath[MAXPATHL];

	যদি (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    ফেরত (char_u *)userSettingsPath;
	অন্যথা
	    ফেরত ফাঁকা;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    যদি (!vimruntime && STRCMP(name, "VIM") != 0)
	ফেরত ফাঁকা;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    যদি (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	যদি (wp != ফাঁকা && *wp == NUL)	    // empty is the same as not set
	    wp = ফাঁকা;
	যদি (wp != ফাঁকা)
	{
	    char_u *q = utf16_to_enc(wp, ফাঁকা);
	    যদি (q != ফাঁকা)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		যদি (p == ফাঁকা)
		    p = q;
	    }
	}
#অন্যথা
	p = mch_getenv((char_u *)"VIM");
	যদি (p != ফাঁকা && *p == NUL)	    // empty is the same as not set
	    p = ফাঁকা;
	যদি (p != ফাঁকা)
	{
	    p = vim_version_dir(p);
	    যদি (p != ফাঁকা)
		*mustfree = TRUE;
	    অন্যথা
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    যদি (p == ফাঁকা)
    {
	যদি (p_hf != ফাঁকা && vim_strchr(p_hf, '$') == ফাঁকা)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	অন্যথা
	    p = exe_name;
#endif
	যদি (p != ফাঁকা)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    যদি (p == p_hf)
		pend = remove_লেজ(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    যদি (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_লেজ(p, pend, (char_u *)"MacOS");
		যদি (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    যদি (pnew != ফাঁকা)
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
	    যদি (p == exe_name)
		pend = remove_লেজ(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    যদি (!vimruntime)
	    {
		pend = remove_লেজ(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_লেজ(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    যদি (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    যদি (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    যদি (p != ফাঁকা && !mch_isdir(p))
		VIM_CLEAR(p);
	    অন্যথা
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		যদি (vimruntime && (pend = vim_version_dir(p)) != ফাঁকা)
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
    যদি (p == ফাঁকা)
    {
	// Only use default_vimruntime_dir when it is not empty
	যদি (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	অন্যথা যদি (*default_vim_dir != NUL)
	{
	    যদি (vimruntime && (p = vim_version_dir(default_vim_dir)) != ফাঁকা)
		*mustfree = TRUE;
	    অন্যথা
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
    যদি (p != ফাঁকা)
    {
	যদি (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	অন্যথা
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ফেরত p;
}

    শূন্য
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((অক্ষর *)var);
#অন্যথা
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    শূন্য
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    যদি (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    অন্যথা যদি (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#যদি defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    শূন্য
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    যদি (STRICMP(name, "HOME") == 0)
	init_homedir();
    অন্যথা যদি (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    অন্যথা যদি (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    শূন্য
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((অক্ষর *)name, (অক্ষর *)val, 1);
#অন্যথা
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    যদি (envbuf != ফাঁকা)
    {
	sprintf((অক্ষর *)envbuf, "%s=%s", name, val);
	putenv((অক্ষর *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    যদি (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	যদি (buf != ফাঁকা)
	{
	    bindtextdomain(VIMPACKAGE, (অক্ষর *)buf);
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
    পূর্ণসংখ্যা		idx)
{
#যদি defined(AMIGA)
    // No environ[] on the Amiga.
    ফেরত ফাঁকা;
#অন্যথা
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    বাহ্যিক অক্ষর		**environ;
# endif
    char_u		*str;
    পূর্ণসংখ্যা			n;

    str = (char_u *)environ[idx];
    যদি (str == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	যদি (str[n] == '=' || str[n] == NUL)
	    বিরতি;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ফেরত xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    স্থির শূন্য
add_user(char_u *user, পূর্ণসংখ্যা need_copy)
{
    char_u	*user_copy = (user != ফাঁকা && need_copy)
						    ? vim_strsave(user) : user;

    যদি (user_copy == ফাঁকা || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	যদি (need_copy)
	    vim_free(user_copy);
	ফেরত;
    }
    ((char_u **)(ga_users.ga_তথ্য))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    স্থির শূন্য
init_users(শূন্য)
{
    স্থির পূর্ণসংখ্যা	lazy_init_done = FALSE;

    যদি (lazy_init_done)
	ফেরত;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, আকার(char_u *), 20);

#যদি defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	কাঠামো passwd*	pw;

	setpwent();
	যখন ((pw = getpwent()) != ফাঁকা)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#নতুবা_যদি defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	যদি (NetUserEnum(ফাঁকা, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_দৈর্ঘ্য,
				       &nusers, &ntotal, ফাঁকা) == NERR_Success)
	{
	    জন্য (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, ফাঁকা), FALSE);

	    Net_Api_বাফার_Free(uinfo);
	}
    }
#endif
#যদি defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	যদি (user_env != ফাঁকা && *user_env != NUL)
	{
	    পূর্ণসংখ্যা	i;

	    জন্য (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_তথ্য)[i];

		যদি (STRCMP(local_user, user_env) == 0)
		    বিরতি;
	    }

	    যদি (i == ga_users.ga_len)
	    {
		কাঠামো passwd	*pw = getpwnam((অক্ষর *)user_env);

		যদি (pw != ফাঁকা)
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
get_users(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    init_users();
    যদি (idx < ga_users.ga_len)
	ফেরত ((char_u **)ga_users.ga_তথ্য)[idx];
    ফেরত ফাঁকা;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    পূর্ণসংখ্যা
match_user(char_u *name)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা n = (পূর্ণসংখ্যা)STRLEN(name);
    পূর্ণসংখ্যা ফলাফল = 0;

    init_users();
    জন্য (i = 0; i < ga_users.ga_len; i++)
    {
	যদি (STRCMP(((char_u **)ga_users.ga_তথ্য)[i], name) == 0)
	    ফেরত 2; // full match
	যদি (STRNCMP(((char_u **)ga_users.ga_তথ্য)[i], name, n) == 0)
	    ফলাফল = 1; // partial match
    }
    ফেরত ফলাফল;
}

    স্থির শূন্য
prepare_to_exit(শূন্য)
{
#যদি defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    অন্যথা
#endif
    {
	windgoto((পূর্ণসংখ্যা)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	যদি (!full_screen)
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
    শূন্য
preserve_exit(শূন্য)
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
	যদি (buf->b_ml.ml_mfp != ফাঁকা && buf->b_ml.ml_mfp->mf_fname != ফাঁকা)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    বিরতি;
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

স্থির পূর্ণসংখ্যা	breakcheck_গুনতি = 0;

    শূন্য
সারি_breakcheck(শূন্য)
{
    যদি (++breakcheck_গুনতি >= BREAKCHECK_SKIP)
    {
	breakcheck_গুনতি = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    শূন্য
fast_breakcheck(শূন্য)
{
    যদি (++breakcheck_গুনতি >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_গুনতি = 0;
	ui_breakcheck();
    }
}

#যদি defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    শূন্য
veryfast_breakcheck(শূন্য)
{
    যদি (++breakcheck_গুনতি >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_গুনতি = 0;
	ui_breakcheck();
    }
}
#endif

#যদি defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    পূর্ণসংখ্যা		flags,		// can be SHELL_SILENT
    পূর্ণসংখ্যা		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*বাফার = ফাঁকা;
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i = 0;
    FILE	*fd;

    যদি (check_restricted() || check_secure())
	ফেরত ফাঁকা;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    যদি ((tempname = vim_tempname('o', TRUE)) == ফাঁকা)
    {
	emsg(_(e_cant_get_অস্থায়ী_file_name));
	ফেরত ফাঁকা;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    যদি (command == ফাঁকা)
	যাও done;

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
    fd = mch_fopen((অক্ষর *)tempname, "r");
# অন্যথা
    fd = mch_fopen((অক্ষর *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    যদি (fd == ফাঁকা
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	যদি (fd != ফাঁকা)
	    fclose(fd);
	যাও done;
    }

    বাফার = alloc(len + 1);
    যদি (বাফার != ফাঁকা)
	i = (পূর্ণসংখ্যা)fread((অক্ষর *)বাফার, (size_t)1, (size_t)len, fd);
    fclose(fd);
    যদি (বাফার == ফাঁকা)
	যাও done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    যদি (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(বাফার);
    }
    অন্যথা যদি (ret_len == ফাঁকা)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	জন্য (i = 0; i < len; ++i)
	    যদি (বাফার[i] == NUL)
		বাফার[i] = 1;

	বাফার[len] = NUL;	// make sure the buffer is terminated
    }
    অন্যথা
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    ফেরত বাফার;
}

# যদি defined(FEAT_EVAL)

    স্থির শূন্য
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    পূর্ণসংখ্যা		retlist)
{
    char_u	*res = ফাঁকা;
    char_u	*p;
    char_u	*infile = ফাঁকা;
    পূর্ণসংখ্যা		err = FALSE;
    FILE	*fd;
    list_T	*list = ফাঁকা;
    পূর্ণসংখ্যা		flags = SHELL_SILENT;
    পূর্ণসংখ্যা		use_argv = FALSE;
    অক্ষর	**argv = ফাঁকা;
    পূর্ণসংখ্যা		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
    যদি (check_restricted() || check_secure())
	যাও errret;

    যদি (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_সংখ্যা_or_list_arg(argvars, 1)
								      == FAIL))
	ফেরত;

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	যদি ((infile = vim_tempname('i', TRUE)) == ফাঁকা)
	{
	    emsg(_(e_cant_get_অস্থায়ী_file_name));
	    যাও errret;
	}

	fd = mch_fopen((অক্ষর *)infile, WRITEBIN);
	যদি (fd == ফাঁকা)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    যাও errret;
	}
	যদি (argvars[1].v_type == VAR_সংখ্যা)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_সংখ্যা);
	    যদি (buf == ফাঁকা)
	    {
		semsg(_(e_বাফার_nr_does_not_exist), argvars[1].vval.v_সংখ্যা);
		fclose(fd);
		যাও errret;
	    }

	    জন্য (lnum = 1; lnum <= buf->b_ml.ml_সারি_গুনতি; lnum++)
	    {
		জন্য (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    যদি (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			বিরতি;
		    }
		যদি (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    বিরতি;
		}
	    }
	}
	অন্যথা যদি (argvars[1].v_type == VAR_LIST)
	{
	    যদি (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	অন্যথা
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    যদি (p == ফাঁকা)
	    {
		fclose(fd);
		যাও errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    যদি (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	যদি (fclose(fd) != 0)
	    err = TRUE;
	যদি (err)
	{
	    emsg(_(e_error_writing_অস্থায়ী_file));
	    যাও errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    যদি (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	যদি (l == ফাঁকা || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    যাও errret;
	}
	যদি (build_argv_from_list(l, &argv, &argc) == FAIL)
	    যাও errret;
	যদি (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    যাও errret;
	}
	use_argv = TRUE;

	যদি (p_verbose > 3)
	{
	    পূর্ণসংখ্যা		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    জন্য (i = 0; i < argc; ++i)
	    {
		যদি (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (অক্ষর *)ga.ga_তথ্য);
	    বার্তা_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    যদি (!বার্তা_silent)
	flags += SHELL_COOKED;

    যদি (retlist)
    {
	পূর্ণসংখ্যা		len;
	listitem_T	*li;
	char_u		*s = ফাঁকা;
	char_u		*start;
	char_u		*end;
	পূর্ণসংখ্যা		i;

	যদি (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	অন্যথা
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	যদি (res == ফাঁকা)
	    যাও errret;

	list = list_alloc();
	যদি (list == ফাঁকা)
	    যাও errret;

	জন্য (i = 0; i < len; ++i)
	{
	    start = res + i;
	    যখন (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    যদি (s == ফাঁকা)
		যাও errret;

	    জন্য (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    যদি (li == ফাঁকা)
	    {
		vim_free(s);
		যাও errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = ফাঁকা;
    }
    অন্যথা
    {
	যদি (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, ফাঁকা);
	অন্যথা
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ফাঁকা);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	যদি (res != ফাঁকা)
	{
	    char_u	*s, *d;

	    d = res;
	    জন্য (s = res; *s; ++s)
	    {
		যদি (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ফাঁকা;
    }

errret:
    যদি (argv != ফাঁকা)
    {
	পূর্ণসংখ্যা i;
	জন্য (i = 0; argv[i] != ফাঁকা; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    যদি (infile != ফাঁকা)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    যদি (res != ফাঁকা)
	vim_free(res);
    যদি (list != ফাঁকা)
	list_free(list);
}

/*
 * "system()" function
 */
    শূন্য
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    শূন্য
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
    পূর্ণসংখ্যা
goto_im(শূন্য)
{
    ফেরত (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(শূন্য)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#অন্যথা
    p = skiptowhite(p_sh);
    যদি (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    অন্যথা
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	জন্য (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    যদি (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    ফেরত p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    পূর্ণসংখ্যা
path_is_url(char_u *p)
{
    যদি (STRNCMP(p, "://", (size_t)3) == 0)
	ফেরত URL_SLASH;
    অন্যথা যদি (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ফেরত URL_BACKSLASH;
    ফেরত 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    পূর্ণসংখ্যা
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    যদি (!ASCII_ISALPHA(*fname))
	ফেরত 0;

    // check body: alpha or dash
    জন্য (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    যদি (p[-1] == '-')
	ফেরত 0;

    // "://" or ":\\" must follow
    ফেরত path_is_url(p);
}

#যদি defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    যদি (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    অন্যথা
	sve->sve_did_save = FALSE;
    ফেরত v_event;
}

    শূন্য
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    যদি (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    অন্যথা
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    শূন্য
may_trigger_modechanged(শূন্য)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_দৈর্ঘ্য];
    char_u	    pattern_buf[2 * MODE_MAX_দৈর্ঘ্য];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    যদি (!has_modechanged() || got_int)
	ফেরত;

    get_mode(curr_mode);
    যদি (STRCMP(curr_mode, last_mode) == 0)
	ফেরত;

    v_event = get_v_event(&save_v_event);
    (শূন্য)dict_add_string(v_event, "new_mode", curr_mode);
    (শূন্য)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_বস্তুসমূহ_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((অক্ষর *)pattern_buf, আকার(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, ফাঁকা, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    পূর্ণসংখ্যা
vim_append_digit_long(long *মান, পূর্ণসংখ্যা digit)
{
    long x = *মান;
    যদি (x > ((LONG_MAX - (long)digit) / 10))
	ফেরত FAIL;
    *মান = x * 10 + (long)digit;
    ফেরত OK;
}

// Return something that fits into an int.
    পূর্ণসংখ্যা
trim_to_int(vimlong_T x)
{
    ফেরত x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
