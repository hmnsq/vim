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

#شامل "vim.h"
#شامل "version.h"

#اگر defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#اگر defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ایستا garray_T	ga_users;

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
    صحیح
get_leader_len(
    char_u	*سطر,
    char_u	**flags,
    صحیح		backward,
    صحیح		include_space)
{
    صحیح		i, j;
    صحیح		result;
    صحیح		got_com = FALSE;
    صحیح		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    صحیح		میانه_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = پوچ;

    result = i = 0;
    وقتی (VIM_ISWHITE(سطر[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    وقتی (سطر[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	برای (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    اگر (!got_com && flags != پوچ)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (تهی)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    اگر (string == پوچ)	    // missing ':', ignore this part
		ادامه;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    اگر (میانه_match_len != 0
		    && vim_strchr(part_buf, COM_میانه) == پوچ
		    && vim_strchr(part_buf, COM_END) == پوچ)
		بشکن;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    اگر (got_com && vim_strchr(part_buf, COM_NEST) == پوچ)
		ادامه;

	    // When 'O' flag present and using "O" command skip this one.
	    اگر (backward && vim_strchr(part_buf, COM_NOBACK) != پوچ)
		ادامه;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    اگر (VIM_ISWHITE(string[0]))
	    {
		اگر (i == 0 || !VIM_ISWHITE(سطر[i - 1]))
		    ادامه;  // missing white space
		وقتی (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    برای (j = 0; string[j] != NUL && string[j] == سطر[i + j]; ++j)
		;
	    اگر (string[j] != NUL)
		ادامه;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    اگر (vim_strchr(part_buf, COM_BLANK) != پوچ
			   && !VIM_ISWHITE(سطر[i + j]) && سطر[i + j] != NUL)
		ادامه;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    اگر (vim_strchr(part_buf, COM_میانه) != پوچ)
	    {
		اگر (میانه_match_len == 0)
		{
		    میانه_match_len = j;
		    saved_flags = prev_list;
		}
		ادامه;
	    }
	    اگر (میانه_match_len != 0 && j > میانه_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		میانه_match_len = 0;

	    اگر (میانه_match_len == 0)
		i += j;
	    found_one = TRUE;
	    بشکن;
	}

	اگر (میانه_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    اگر (!got_com && flags != پوچ)
		*flags = saved_flags;
	    i += میانه_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	اگر (!found_one)
	    بشکن;

	result = i;

	// Include any trailing white space.
	وقتی (VIM_ISWHITE(سطر[i]))
	    ++i;

	اگر (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	اگر (vim_strchr(part_buf, COM_NEST) == پوچ)
	    بشکن;
    }
    بازگشت result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    صحیح
get_last_leader_offset(char_u *سطر, char_u **flags)
{
    صحیح		result = -1;
    صحیح		i, j;
    صحیح		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    صحیح		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (صحیح)STRLEN(سطر);
    وقتی (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	برای (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (تهی)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    اگر (string == پوچ)	// If everything is fine, this cannot actually
				// happen.
		ادامه;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    اگر (VIM_ISWHITE(string[0]))
	    {
		اگر (i == 0 || !VIM_ISWHITE(سطر[i - 1]))
		    ادامه;
		وقتی (VIM_ISWHITE(*string))
		    ++string;
	    }
	    برای (j = 0; string[j] != NUL && string[j] == سطر[i + j]; ++j)
		/* do nothing */;
	    اگر (string[j] != NUL)
		ادامه;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    اگر (vim_strchr(part_buf, COM_BLANK) != پوچ
		    && !VIM_ISWHITE(سطر[i + j]) && سطر[i + j] != NUL)
		ادامه;

	    اگر (vim_strchr(part_buf, COM_میانه) != پوچ)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		برای (j = 0; VIM_ISWHITE(سطر[j]) && j <= i; j++)
		    ;
		اگر (j < i)
		    ادامه;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    اگر (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    بشکن;
	}

	اگر (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    صحیح     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    اگر (vim_strchr(part_buf, COM_NEST) != پوچ)
		ادامه;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    وقتی (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (صحیح)STRLEN(com_leader);

	    برای (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(تهی)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		اگر (flags_save == com_flags)
		    ادامه;
		string = vim_strchr(part_buf2, ':');
		++string;
		وقتی (VIM_ISWHITE(*string))
		    ++string;
		len2 = (صحیح)STRLEN(string);
		اگر (len2 == 0)
		    ادامه;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		برای (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    اگر (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			اگر (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    بازگشت result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    صحیح
plines(linenr_T lnum)
{
    بازگشت plines_win(curwin, lnum, TRUE);
}

    صحیح
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		limit_winheight)	// when TRUE limit to window height
{
#اگر defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    بازگشت plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    صحیح
plines_nofill(linenr_T lnum)
{
    بازگشت plines_win_nofill(curwin, lnum, TRUE);
}

    صحیح
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    صحیح		limit_winheight)	// when TRUE limit to window height
{
#endif
    صحیح		سطرها;

    اگر (wp->w_width == 0)
	بازگشت 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    اگر (سطر_Folded(wp, lnum) == TRUE)
	بازگشت 1;
#endif

    اگر (!wp->w_p_wrap)
	سطرها = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_شمار_above_below(wp->w_buffer, lnum)
#endif
	;
    وگرنه
	سطرها = plines_win_nofold(wp, lnum);

    اگر (limit_winheight && سطرها > wp->w_height)
	بازگشت wp->w_height;
    بازگشت سطرها;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    صحیح
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    صحیح		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    اگر (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	بازگشت 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (صحیح)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    اگر (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    اگر (width <= 0)
	بازگشت 32000;
    اگر (col <= width)
	بازگشت 1;
    col -= width;
    width += win_col_off2(wp);
    بازگشت (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    صحیح
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    صحیح		سطرها = 0;
    صحیح		width;
    char_u	*سطر;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    سطرها = diff_check_fill(wp, lnum);
#endif

    اگر (!wp->w_p_wrap)
	بازگشت سطرها + 1;

    اگر (wp->w_width == 0)
	بازگشت سطرها + 1;

    سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    // "column" is a byte index, advance the pointer until it is reached.
    وقتی (*cts.cts_ptr != NUL && cts.cts_ptr < سطر + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, پوچ, پوچ);
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
    اگر (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, پوچ, پوچ) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    اگر (width <= 0)
	بازگشت 9999;

    سطرها += 1;
    اگر (col > width)
	سطرها += (col - width) / (width + win_col_off2(wp)) + 1;
    بازگشت سطرها;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    صحیح
plines_m_win(win_T *wp, linenr_T first, linenr_T last, صحیح max)
{
    صحیح		شمار = 0;

    وقتی (first <= last && شمار < max)
    {
#ifdef FEAT_FOLDING
	صحیح	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_شمار(wp, first, پوچ);
	اگر (x > 0)
	{
	    ++شمار;	    // count 1 for "+-- folded" line
	    first += x;
	}
	وگرنه
#endif
	{
#ifdef FEAT_DIFF
	    اگر (first == wp->w_topline)
		شمار += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    وگرنه
#endif
		شمار += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    اگر (first == wp->w_buffer->b_ml.ml_سطر_شمار + 1)
	شمار += diff_check_fill(wp, first);
#endif
    بازگشت MIN(max, شمار);
}

    صحیح
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    صحیح		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    اگر (pos->col == MAXCOL)
	بازگشت NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    اگر (pos->col > ptrlen)
	بازگشت NUL;
    اگر (has_mbyte)
	بازگشت (*mb_ptr2char)(ptr);
    بازگشت (صحیح)*ptr;
}

    صحیح
gchar_cursor(تهی)
{
    اگر (has_mbyte)
	بازگشت (*mb_ptr2char)(ml_get_cursor());
    بازگشت (صحیح)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    صحیح
char_before_cursor(تهی)
{
    اگر (curwin->w_cursor.col == 0)
	بازگشت -1;

    char_u *سطر = ml_get_curline();

    اگر (has_mbyte)
    {
	char_u *p = سطر + curwin->w_cursor.col;
	صحیح prev_len = (*mb_head_off)(سطر, p - 1) + 1;
	بازگشت mb_ptr2char(p - prev_len);
    }
    بازگشت سطر[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    تهی
pchar_cursor(صحیح c)
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
    اگر (*p == ',')
	++p;
    وقتی (*p == ' ')
	++p;
    بازگشت p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    تهی
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_buffer == buf && wp->w_status_height)
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
    صحیح
ask_yesno(char_u *str, صحیح direct)
{
    صحیح	    r = ' ';
    صحیح	    save_State = State;

    اگر (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_کلیدها;		// no mapping here, but recognize keys

    وقتی (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	اگر (direct)
	    r = get_keystroke();
	وگرنه
	    r = plain_vgetc();
	اگر (r == Ctrl_C || r == ESC)
	    r = 'n';
	پیام_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_کلیدها;

    بازگشت r;
}

#اگر defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    تهی
get_mode(char_u *buf)
{
    صحیح		i = 0;

    اگر (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    وگرنه اگر (term_use_loop())
    {
	اگر (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    وگرنه اگر (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	اگر (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	وگرنه اگر (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    وگرنه اگر (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    وگرنه اگر (State & MODE_درج)
    {
	اگر (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	وگرنه
	{
	    اگر (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    وگرنه
		buf[i++] = 'i';
	}

	اگر (ins_compl_active())
	    buf[i++] = 'c';
	وگرنه اگر (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    وگرنه اگر ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	اگر (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	وگرنه اگر (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	اگر ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    وگرنه اگر (VIsual_active)
    {
	اگر (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	وگرنه
	{
	    buf[i++] = VIsual_mode;
	    اگر (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    وگرنه
    {
	buf[i++] = 'n';
	اگر (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	وگرنه اگر (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	وگرنه اگر (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    تهی
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    اگر (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	بازگشت;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    اگر (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ایستا تهی
may_add_state_char(garray_T *gap, char_u *include, صحیح c)
{
    اگر (include == پوچ || vim_strchr(include, c) != پوچ)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    تهی
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = پوچ;
    صحیح		i;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    ga_init2(&ga, 1, 20);
    اگر (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    اگر (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == پوچ))
	may_add_state_char(&ga, include, 'm');
    اگر (op_pending())
	may_add_state_char(&ga, include, 'o');
    اگر (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    اگر (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    اگر (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    اگر (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    برای (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    اگر (پیام_scrolled > 0)
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
    صحیح
get_keystroke(تهی)
{
    char_u	*buf = پوچ;
    صحیح		buflen = 150;
    صحیح		maxlen;
    صحیح		len = 0;
    صحیح		n;
    صحیح		save_mapped_ctrl_c = mapped_ctrl_c;
    صحیح		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    برای (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	اگر (buf == پوچ)
	    buf = alloc(buflen);
	وگرنه اگر (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    اگر (buf == پوچ)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	اگر (buf == پوچ)
	{
	    do_outofmem_پیام((long_u)buflen);
	    بازگشت ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	اگر (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_درون_داد_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	وگرنه اگر (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	اگر ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ادامه;

	اگر (n == KEYLEN_REMOVED)  // key code removed
	{
	    اگر (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ادامه;
	}
	اگر (n > 0)		// found a termcode: adjust length
	    len = n;
	اگر (len == 0)		// nothing typed yet
	    ادامه;

	// Handle modifier and/or special key code.
	n = buf[0];
	اگر (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    اگر (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_کلید(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		اگر (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		اگر (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ادامه;
	    }
	    بشکن;
	}
	اگر (has_mbyte)
	{
	    اگر (MB_BYTE2LEN(n) > len)
		ادامه;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	اگر (n == intr_char)
	    n = ESC;
#endif
	بشکن;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    بازگشت merge_modify_Other_کلیدها(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    ایستا صحیح
vim_append_digit_int(صحیح *مقدار, صحیح digit)
{
    صحیح x = *مقدار;
    اگر (x > ((INT_MAX - digit) / 10))
	بازگشت FAIL;
    *مقدار = x * 10 + digit;
    بازگشت OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    صحیح
get_عدد(
    صحیح	    colon,			// allow colon to abort
    صحیح	    *mouse_used)
{
    صحیح	n = 0;
    صحیح	c;
    صحیح typed = 0;

    اگر (mouse_used != پوچ)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    اگر (پیام_silent != 0)
	بازگشت 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_کلیدها;		// no mapping here, but recognize keys
    برای (;;)
    {
	windgoto(پیام_row, cmdline_col_off + پیام_col);
	c = safe_vgetc();
	اگر (VIM_ISDIGIT(c))
	{
	    اگر (vim_append_digit_int(&n, c - '0') == FAIL)
		بازگشت 0;
	    پیام_putchar(c);
	    ++typed;
	}
	وگرنه اگر (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    اگر (typed > 0)
	    {
		پیام_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	وگرنه اگر (mouse_used != پوچ && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    بشکن;
	}
	وگرنه اگر (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    اگر (!exmode_active)
		cmdline_row = پیام_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    بشکن;
	}
	وگرنه اگر (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    بشکن;
	}
	وگرنه اگر (c == CAR || c == NL )
	    بشکن;
    }
    --no_mapping;
    --allow_کلیدها;
    بازگشت n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    صحیح
prompt_for_عدد(صحیح *mouse_used)
{
    صحیح		i;
    صحیح		save_cmdline_row;
    صحیح		save_State;

    // When using ":silent" assume that <CR> was entered.
    اگر (mouse_used != پوچ)
	پیام_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    وگرنه
	پیام_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_عدد(TRUE, mouse_used);
    اگر (کلید_Typed)
    {
	// don't call wait_return() now
	اگر (پیام_row > 0)
	    cmdline_row = پیام_row - 1;
	need_wait_return = FALSE;
	پیام_didany = FALSE;
	پیام_didout = FALSE;
    }
    وگرنه
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    بازگشت i;
}

    تهی
msgmore(long n)
{
    long pn;

    اگر (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	بازگشت;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    اگر (keep_پیام != پوچ && !keep_پیام_more)
	بازگشت;

    اگر (n > 0)
	pn = n;
    وگرنه
	pn = -n;

    اگر (pn > p_report)
    {
	اگر (n > 0)
	    vim_snprintf(پیام_buf, پیام_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	وگرنه
	    vim_snprintf(پیام_buf, پیام_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	اگر (got_int)
	    vim_strcat((char_u *)پیام_buf, (char_u *)_(" (Interrupted)"),
								  پیام_BUF_LEN);
	اگر (پیام(پیام_buf))
	{
	    set_keep_پیام((char_u *)پیام_buf, 0);
	    keep_پیام_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    تهی
beep_flush(تهی)
{
    اگر (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    تهی
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    اگر (emsg_silent != 0 || in_assert_fails)
	بازگشت;

    اگر (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	ایستا صحیح		did_init = FALSE;
	ایستا elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	اگر (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    اگر (p_vb
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
		اگر (has_vtp_working() != 0
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
	    وگرنه
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    اگر (vim_strchr(p_debug, 'e') != پوچ)
    {
	پیام_source(HL_ATTR(HLF_W));
	پیام_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    تهی
init_homedir(تهی)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#وگرنه
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    اگر (var == پوچ || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	اگر (homepath == پوچ || *homepath == NUL)
	    homepath = (char_u *)"\\";
	اگر (homedrive != پوچ
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((نویسه *)NameBuff, "%s%s", homedrive, homepath);
	    اگر (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    اگر (var == پوچ)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    اگر (var != پوچ && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	اگر (p != پوچ)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    اگر (exp != پوچ && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((نویسه *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    اگر (var != پوچ && *var == NUL)	// empty is same as not set
	var = پوچ;

    اگر (enc_utf8 && var != پوچ)
    {
	صحیح	len;
	char_u  *pp = پوچ;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (صحیح)STRLEN(var), &pp, &len);
	اگر (pp != پوچ)
	{
	    homedir = pp;
	    بازگشت;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    اگر (var == پوچ)
	var = (char_u *)"C:/";
#endif

    اگر (var != پوچ)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	اگر (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((نویسه *)NameBuff) == 0)
	{
	    اگر (!mch_chdir((نویسه *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    اگر (mch_chdir((نویسه *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#اگر defined(EXITFREE)
    تهی
free_homedir(تهی)
{
    vim_free(homedir);
}

    تهی
free_users(تهی)
{
    ga_clear_strings(&ga_users);
}
#endif

#اگر defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    تهی
init_vimdir(تهی)
{
    صحیح	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    اگر (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    اگر (mustfree)
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
    بازگشت expand_env_save_opt(src, FALSE, پوچ);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, صحیح one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    اگر (p != پوچ)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, پوچ);
    بازگشت p;
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
    صحیح		dstlen)		// maximum length of the result
{
    بازگشت expand_env_esc(src, dst, dstlen, پوچ, FALSE, پوچ);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    صحیح		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    صحیح		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    صحیح		c;
    char_u	*var;
    صحیح		copy_char;
    صحیح		mustfree;	// var was allocated, need to free it later
    صحیح		at_start = TRUE; // at start of a name
    صحیح		startstr_len = 0;
    char_u	*dst_start = dst;

    اگر (startstr != پوچ)
	startstr_len = (صحیح)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    وقتی (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	اگر (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (تهی)skip_عبارت(&src, پوچ);
	    اگر (*src == '`')
		++src;
	    len = src - var;
	    اگر (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (صحیح)len;
	    ادامه;
	}
#endif
	copy_char = TRUE;
	اگر ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#اگر defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    اگر (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		اگر (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    وقتی (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		وگرنه
#endif
		{
		    وقتی (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#اگر defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#اگر defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		اگر (src[1] == '{' && *tail != '}')
# وگرنه
		اگر (*src == '%' && *tail != '%')
# endif
		    var = پوچ;
		وگرنه
		{
# ifdef UNIX
		    اگر (src[1] == '{')
# وگرنه
		    اگر (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#اگر defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    وگرنه اگر (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != پوچ)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    وگرنه					// user directory
	    {
#اگر defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		وقتی (	   c-- > 0
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
#  اگر defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ساختار passwd *pw = (*dst == NUL)
					? پوچ : getpwnam((نویسه *)dst + 1);

		    var = (pw == پوچ) ? پوچ : (char_u *)pw->pw_dir;
		}
		اگر (var == پوچ)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, پوچ,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# وگرنه	// !UNIX, thus VMS
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
		    وقتی (*next_path)
		    {
			برای (path = next_path; *next_path && *next_path != ',';
				next_path++);
			اگر (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (نویسه *)test,
			    اندازه(test),
			    "%s/%s",
			    path,
			    dst + 1);
			اگر (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    اگر (var != پوچ)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    بشکن;
			}
		    }
		}
# endif // UNIX
#وگرنه
		// cannot expand user's home directory, so don't try
		var = پوچ;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    اگر (p_ssl && var != پوچ && vim_strchr(var, '\\') != پوچ)
	    {
		char_u	*p = vim_strsave(var);

		اگر (p != پوچ)
		{
		    اگر (mustfree)
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
	    اگر (esc_chars != پوچ && var != پوچ
		    && vim_strpbrk(var, esc_chars) != پوچ)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		اگر (p != پوچ)
		{
		    اگر (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    اگر (var != پوچ && *var != NUL)
	    {
		c = (صحیح)STRLEN(var);

		اگر (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    اگر (after_pathsep(dst, dst + c)
#اگر defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    اگر (mustfree)
		vim_free(var);
	}

	اگر (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    اگر (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    وگرنه اگر ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    اگر (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		اگر (startstr != پوچ && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    بازگشت (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ایستا char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    صحیح		len = (صحیح)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    اگر (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	بازگشت newend;
    بازگشت pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ایستا char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    اگر (vimdir == پوچ || *vimdir == NUL)
	بازگشت پوچ;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    اگر (p.string != پوچ && mch_isdir(p.string))
	بازگشت p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    اگر (p.string != پوچ && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	اگر (fname.string != پوچ)
	{
	    صحیح exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    اگر (exists)
		بازگشت p.string;
	}
    }
    vim_free(p.string);
    بازگشت پوچ;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, صحیح *mustfree)
{
    char_u	*p = پوچ;
    char_u	*pend;
    صحیح		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    اگر (STRCMP(name, "HOME") == 0)
	بازگشت homedir;

    // Use Wide function
    wn = enc_to_utf16(name, پوچ);
    اگر (wn == پوچ)
	بازگشت پوچ;

    wp = _wgetenv(wn);
    vim_free(wn);

    اگر (wp != پوچ && *wp == NUL)   // empty is the same as not set
	wp = پوچ;

    اگر (wp != پوچ)
    {
	p = utf16_to_enc(wp, پوچ);
	اگر (p == پوچ)
	    بازگشت پوچ;

	*mustfree = TRUE;
	بازگشت p;
    }
#وگرنه
    p = mch_getenv(name);
    اگر (p != پوچ && *p == NUL)	    // empty is the same as not set
	p = پوچ;

    اگر (p != پوچ)
	بازگشت p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    اگر (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ایستا نویسه userSettingsPath[MAXPATHL];

	اگر (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    بازگشت (char_u *)userSettingsPath;
	وگرنه
	    بازگشت پوچ;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    اگر (!vimruntime && STRCMP(name, "VIM") != 0)
	بازگشت پوچ;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    اگر (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	اگر (wp != پوچ && *wp == NUL)	    // empty is the same as not set
	    wp = پوچ;
	اگر (wp != پوچ)
	{
	    char_u *q = utf16_to_enc(wp, پوچ);
	    اگر (q != پوچ)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		اگر (p == پوچ)
		    p = q;
	    }
	}
#وگرنه
	p = mch_getenv((char_u *)"VIM");
	اگر (p != پوچ && *p == NUL)	    // empty is the same as not set
	    p = پوچ;
	اگر (p != پوچ)
	{
	    p = vim_version_dir(p);
	    اگر (p != پوچ)
		*mustfree = TRUE;
	    وگرنه
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    اگر (p == پوچ)
    {
	اگر (p_hf != پوچ && vim_strchr(p_hf, '$') == پوچ)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	وگرنه
	    p = exe_name;
#endif
	اگر (p != پوچ)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    اگر (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    اگر (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		اگر (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    اگر (pnew != پوچ)
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
	    اگر (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    اگر (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    اگر (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    اگر (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    اگر (p != پوچ && !mch_isdir(p))
		VIM_CLEAR(p);
	    وگرنه
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		اگر (vimruntime && (pend = vim_version_dir(p)) != پوچ)
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
    اگر (p == پوچ)
    {
	// Only use default_vimruntime_dir when it is not empty
	اگر (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	وگرنه اگر (*default_vim_dir != NUL)
	{
	    اگر (vimruntime && (p = vim_version_dir(default_vim_dir)) != پوچ)
		*mustfree = TRUE;
	    وگرنه
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
    اگر (p != پوچ)
    {
	اگر (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	وگرنه
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    بازگشت p;
}

    تهی
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((نویسه *)var);
#وگرنه
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    تهی
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    اگر (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    وگرنه اگر (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#اگر defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    تهی
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    اگر (STRICMP(name, "HOME") == 0)
	init_homedir();
    وگرنه اگر (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    وگرنه اگر (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    تهی
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((نویسه *)name, (نویسه *)val, 1);
#وگرنه
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    اگر (envbuf != پوچ)
    {
	sprintf((نویسه *)envbuf, "%s=%s", name, val);
	putenv((نویسه *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    اگر (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	اگر (buf != پوچ)
	{
	    bindtextdomain(VIMPACKAGE, (نویسه *)buf);
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
    صحیح		idx)
{
#اگر defined(AMIGA)
    // No environ[] on the Amiga.
    بازگشت پوچ;
#وگرنه
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    بیرونی نویسه		**environ;
# endif
    char_u		*str;
    صحیح			n;

    str = (char_u *)environ[idx];
    اگر (str == پوچ)
	بازگشت پوچ;

    برای (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	اگر (str[n] == '=' || str[n] == NUL)
	    بشکن;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    بازگشت xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ایستا تهی
add_user(char_u *user, صحیح need_copy)
{
    char_u	*user_copy = (user != پوچ && need_copy)
						    ? vim_strsave(user) : user;

    اگر (user_copy == پوچ || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	اگر (need_copy)
	    vim_free(user_copy);
	بازگشت;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ایستا تهی
init_users(تهی)
{
    ایستا صحیح	lazy_init_done = FALSE;

    اگر (lazy_init_done)
	بازگشت;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, اندازه(char_u *), 20);

#اگر defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ساختار passwd*	pw;

	setpwent();
	وقتی ((pw = getpwent()) != پوچ)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#وگرنهاگر defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	اگر (NetUserEnum(پوچ, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, پوچ) == NERR_Success)
	{
	    برای (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, پوچ), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#اگر defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	اگر (user_env != پوچ && *user_env != NUL)
	{
	    صحیح	i;

	    برای (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		اگر (STRCMP(local_user, user_env) == 0)
		    بشکن;
	    }

	    اگر (i == ga_users.ga_len)
	    {
		ساختار passwd	*pw = getpwnam((نویسه *)user_env);

		اگر (pw != پوچ)
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
get_users(expand_T *xp UNUSED, صحیح idx)
{
    init_users();
    اگر (idx < ga_users.ga_len)
	بازگشت ((char_u **)ga_users.ga_data)[idx];
    بازگشت پوچ;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    صحیح
match_user(char_u *name)
{
    صحیح i;
    صحیح n = (صحیح)STRLEN(name);
    صحیح result = 0;

    init_users();
    برای (i = 0; i < ga_users.ga_len; i++)
    {
	اگر (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    بازگشت 2; // full match
	اگر (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    بازگشت result;
}

    ایستا تهی
prepare_to_exit(تهی)
{
#اگر defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    اگر (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    وگرنه
#endif
    {
	windgoto((صحیح)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	اگر (!full_screen)
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
    تهی
preserve_exit(تهی)
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
	اگر (buf->b_ml.ml_mfp != پوچ && buf->b_ml.ml_mfp->mf_fname != پوچ)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    بشکن;
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

ایستا صحیح	breakcheck_شمار = 0;

    تهی
سطر_breakcheck(تهی)
{
    اگر (++breakcheck_شمار >= BREAKCHECK_SKIP)
    {
	breakcheck_شمار = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    تهی
fast_breakcheck(تهی)
{
    اگر (++breakcheck_شمار >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_شمار = 0;
	ui_breakcheck();
    }
}

#اگر defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    تهی
veryfast_breakcheck(تهی)
{
    اگر (++breakcheck_شمار >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_شمار = 0;
	ui_breakcheck();
    }
}
#endif

#اگر defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    صحیح		flags,		// can be SHELL_SILENT
    صحیح		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = پوچ;
    صحیح		len;
    صحیح		i = 0;
    FILE	*fd;

    اگر (check_restricted() || check_secure())
	بازگشت پوچ;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    اگر ((tempname = vim_tempname('o', TRUE)) == پوچ)
    {
	emsg(_(e_cant_get_temp_file_name));
	بازگشت پوچ;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    اگر (command == پوچ)
	برو done;

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
    fd = mch_fopen((نویسه *)tempname, "r");
# وگرنه
    fd = mch_fopen((نویسه *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    اگر (fd == پوچ
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	اگر (fd != پوچ)
	    fclose(fd);
	برو done;
    }

    buffer = alloc(len + 1);
    اگر (buffer != پوچ)
	i = (صحیح)fread((نویسه *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    اگر (buffer == پوچ)
	برو done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    اگر (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    وگرنه اگر (ret_len == پوچ)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	برای (i = 0; i < len; ++i)
	    اگر (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    وگرنه
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    بازگشت buffer;
}

# اگر defined(FEAT_EVAL)

    ایستا تهی
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		retlist)
{
    char_u	*res = پوچ;
    char_u	*p;
    char_u	*infile = پوچ;
    صحیح		err = FALSE;
    FILE	*fd;
    list_T	*list = پوچ;
    صحیح		flags = SHELL_SILENT;
    صحیح		use_argv = FALSE;
    نویسه	**argv = پوچ;
    صحیح		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    اگر (check_restricted() || check_secure())
	برو errret;

    اگر (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_عدد_or_list_arg(argvars, 1)
								      == FAIL))
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	اگر ((infile = vim_tempname('i', TRUE)) == پوچ)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    برو errret;
	}

	fd = mch_fopen((نویسه *)infile, WRITEBIN);
	اگر (fd == پوچ)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    برو errret;
	}
	اگر (argvars[1].v_type == VAR_عدد)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_عدد);
	    اگر (buf == پوچ)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_عدد);
		fclose(fd);
		برو errret;
	    }

	    برای (lnum = 1; lnum <= buf->b_ml.ml_سطر_شمار; lnum++)
	    {
		برای (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    اگر (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			بشکن;
		    }
		اگر (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    بشکن;
		}
	    }
	}
	وگرنه اگر (argvars[1].v_type == VAR_LIST)
	{
	    اگر (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	وگرنه
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    اگر (p == پوچ)
	    {
		fclose(fd);
		برو errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    اگر (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	اگر (fclose(fd) != 0)
	    err = TRUE;
	اگر (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    برو errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    اگر (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	اگر (l == پوچ || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    برو errret;
	}
	اگر (build_argv_from_list(l, &argv, &argc) == FAIL)
	    برو errret;
	اگر (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    برو errret;
	}
	use_argv = TRUE;

	اگر (p_verbose > 3)
	{
	    صحیح		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    برای (i = 0; i < argc; ++i)
	    {
		اگر (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (نویسه *)ga.ga_data);
	    پیام_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    اگر (!پیام_silent)
	flags += SHELL_COOKED;

    اگر (retlist)
    {
	صحیح		len;
	listitem_T	*li;
	char_u		*s = پوچ;
	char_u		*start;
	char_u		*end;
	صحیح		i;

	اگر (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	وگرنه
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	اگر (res == پوچ)
	    برو errret;

	list = list_alloc();
	اگر (list == پوچ)
	    برو errret;

	برای (i = 0; i < len; ++i)
	{
	    start = res + i;
	    وقتی (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    اگر (s == پوچ)
		برو errret;

	    برای (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    اگر (li == پوچ)
	    {
		vim_free(s);
		برو errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = پوچ;
    }
    وگرنه
    {
	اگر (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, پوچ);
	وگرنه
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, پوچ);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	اگر (res != پوچ)
	{
	    char_u	*s, *d;

	    d = res;
	    برای (s = res; *s; ++s)
	    {
		اگر (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = پوچ;
    }

errret:
    اگر (argv != پوچ)
    {
	صحیح i;
	برای (i = 0; argv[i] != پوچ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    اگر (infile != پوچ)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    اگر (res != پوچ)
	vim_free(res);
    اگر (list != پوچ)
	list_free(list);
}

/*
 * "system()" function
 */
    تهی
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    تهی
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
    صحیح
goto_im(تهی)
{
    بازگشت (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(تهی)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#وگرنه
    p = skiptowhite(p_sh);
    اگر (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    وگرنه
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	برای (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    اگر (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    بازگشت p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    صحیح
path_is_url(char_u *p)
{
    اگر (STRNCMP(p, "://", (size_t)3) == 0)
	بازگشت URL_SLASH;
    وگرنه اگر (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	بازگشت URL_BACKSLASH;
    بازگشت 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    صحیح
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    اگر (!ASCII_ISALPHA(*fname))
	بازگشت 0;

    // check body: alpha or dash
    برای (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    اگر (p[-1] == '-')
	بازگشت 0;

    // "://" or ":\\" must follow
    بازگشت path_is_url(p);
}

#اگر defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    اگر (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    وگرنه
	sve->sve_did_save = FALSE;
    بازگشت v_event;
}

    تهی
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    اگر (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    وگرنه
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    تهی
may_trigger_modechanged(تهی)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    اگر (!has_modechanged() || got_int)
	بازگشت;

    get_mode(curr_mode);
    اگر (STRCMP(curr_mode, last_mode) == 0)
	بازگشت;

    v_event = get_v_event(&save_v_event);
    (تهی)dict_add_string(v_event, "new_mode", curr_mode);
    (تهی)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_اقلام_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((نویسه *)pattern_buf, اندازه(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, پوچ, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    صحیح
vim_append_digit_long(long *مقدار, صحیح digit)
{
    long x = *مقدار;
    اگر (x > ((LONG_MAX - (long)digit) / 10))
	بازگشت FAIL;
    *مقدار = x * 10 + (long)digit;
    بازگشت OK;
}

// Return something that fits into an int.
    صحیح
trim_to_int(vimlong_T x)
{
    بازگشت x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
