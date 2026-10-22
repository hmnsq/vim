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

#ضمن "vim.h"
#ضمن "version.h"

#إذا defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#إذا defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ساكن garray_T	ga_users;

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
    صحيح
get_leader_len(
    char_u	*قۇر,
    char_u	**flags,
    صحيح		backward,
    صحيح		include_space)
{
    صحيح		i, j;
    صحيح		result;
    صحيح		got_com = FALSE;
    صحيح		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    صحيح		ئوتتۇرا_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = عدم;

    result = i = 0;
    طالما (VIM_ISWHITE(قۇر[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    طالما (قۇر[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	لكل (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    إذا (!got_com && flags != عدم)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (فراغ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    إذا (string == عدم)	    // missing ':', ignore this part
		استمر;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    إذا (ئوتتۇرا_match_len != 0
		    && vim_strchr(part_buf, COM_ئوتتۇرا) == عدم
		    && vim_strchr(part_buf, COM_END) == عدم)
		اكسر;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    إذا (got_com && vim_strchr(part_buf, COM_NEST) == عدم)
		استمر;

	    // When 'O' flag present and using "O" command skip this one.
	    إذا (backward && vim_strchr(part_buf, COM_NOBACK) != عدم)
		استمر;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    إذا (VIM_ISWHITE(string[0]))
	    {
		إذا (i == 0 || !VIM_ISWHITE(قۇر[i - 1]))
		    استمر;  // missing white space
		طالما (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    لكل (j = 0; string[j] != NUL && string[j] == قۇر[i + j]; ++j)
		;
	    إذا (string[j] != NUL)
		استمر;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    إذا (vim_strchr(part_buf, COM_BLANK) != عدم
			   && !VIM_ISWHITE(قۇر[i + j]) && قۇر[i + j] != NUL)
		استمر;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    إذا (vim_strchr(part_buf, COM_ئوتتۇرا) != عدم)
	    {
		إذا (ئوتتۇرا_match_len == 0)
		{
		    ئوتتۇرا_match_len = j;
		    saved_flags = prev_list;
		}
		استمر;
	    }
	    إذا (ئوتتۇرا_match_len != 0 && j > ئوتتۇرا_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		ئوتتۇرا_match_len = 0;

	    إذا (ئوتتۇرا_match_len == 0)
		i += j;
	    found_one = TRUE;
	    اكسر;
	}

	إذا (ئوتتۇرا_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    إذا (!got_com && flags != عدم)
		*flags = saved_flags;
	    i += ئوتتۇرا_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	إذا (!found_one)
	    اكسر;

	result = i;

	// Include any trailing white space.
	طالما (VIM_ISWHITE(قۇر[i]))
	    ++i;

	إذا (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	إذا (vim_strchr(part_buf, COM_NEST) == عدم)
	    اكسر;
    }
    أرجع result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    صحيح
get_last_leader_offset(char_u *قۇر, char_u **flags)
{
    صحيح		result = -1;
    صحيح		i, j;
    صحيح		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    صحيح		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (صحيح)STRLEN(قۇر);
    طالما (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	لكل (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (فراغ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    إذا (string == عدم)	// If everything is fine, this cannot actually
				// happen.
		استمر;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    إذا (VIM_ISWHITE(string[0]))
	    {
		إذا (i == 0 || !VIM_ISWHITE(قۇر[i - 1]))
		    استمر;
		طالما (VIM_ISWHITE(*string))
		    ++string;
	    }
	    لكل (j = 0; string[j] != NUL && string[j] == قۇر[i + j]; ++j)
		/* do nothing */;
	    إذا (string[j] != NUL)
		استمر;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    إذا (vim_strchr(part_buf, COM_BLANK) != عدم
		    && !VIM_ISWHITE(قۇر[i + j]) && قۇر[i + j] != NUL)
		استمر;

	    إذا (vim_strchr(part_buf, COM_ئوتتۇرا) != عدم)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		لكل (j = 0; VIM_ISWHITE(قۇر[j]) && j <= i; j++)
		    ;
		إذا (j < i)
		    استمر;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    إذا (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    اكسر;
	}

	إذا (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    صحيح     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    إذا (vim_strchr(part_buf, COM_NEST) != عدم)
		استمر;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    طالما (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (صحيح)STRLEN(com_leader);

	    لكل (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(فراغ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		إذا (flags_save == com_flags)
		    استمر;
		string = vim_strchr(part_buf2, ':');
		++string;
		طالما (VIM_ISWHITE(*string))
		    ++string;
		len2 = (صحيح)STRLEN(string);
		إذا (len2 == 0)
		    استمر;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		لكل (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    إذا (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			إذا (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    أرجع result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    صحيح
plines(linenr_T lnum)
{
    أرجع plines_win(curwin, lnum, TRUE);
}

    صحيح
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		limit_winheight)	// when TRUE limit to window height
{
#إذا defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    أرجع plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    صحيح
plines_nofill(linenr_T lnum)
{
    أرجع plines_win_nofill(curwin, lnum, TRUE);
}

    صحيح
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		limit_winheight)	// when TRUE limit to window height
{
#endif
    صحيح		قۇرلار;

    إذا (wp->w_width == 0)
	أرجع 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    إذا (قۇر_Folded(wp, lnum) == TRUE)
	أرجع 1;
#endif

    إذا (!wp->w_p_wrap)
	قۇرلار = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_سانى_above_below(wp->w_buffer, lnum)
#endif
	;
    وإلا
	قۇرلار = plines_win_nofold(wp, lnum);

    إذا (limit_winheight && قۇرلار > wp->w_height)
	أرجع wp->w_height;
    أرجع قۇرلار;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    صحيح
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    صحيح		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    إذا (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	أرجع 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (صحيح)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    إذا (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    إذا (width <= 0)
	أرجع 32000;
    إذا (col <= width)
	أرجع 1;
    col -= width;
    width += win_col_off2(wp);
    أرجع (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    صحيح
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    صحيح		قۇرلار = 0;
    صحيح		width;
    char_u	*قۇر;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    قۇرلار = diff_check_fill(wp, lnum);
#endif

    إذا (!wp->w_p_wrap)
	أرجع قۇرلار + 1;

    إذا (wp->w_width == 0)
	أرجع قۇرلار + 1;

    قۇر = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, قۇر, قۇر);
    // "column" is a byte index, advance the pointer until it is reached.
    طالما (*cts.cts_ptr != NUL && cts.cts_ptr < قۇر + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, عدم, عدم);
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
    إذا (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, عدم, عدم) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    إذا (width <= 0)
	أرجع 9999;

    قۇرلار += 1;
    إذا (col > width)
	قۇرلار += (col - width) / (width + win_col_off2(wp)) + 1;
    أرجع قۇرلار;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    صحيح
plines_m_win(win_T *wp, linenr_T first, linenr_T last, صحيح max)
{
    صحيح		سانى = 0;

    طالما (first <= last && سانى < max)
    {
#ifdef FEAT_FOLDING
	صحيح	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_سانى(wp, first, عدم);
	إذا (x > 0)
	{
	    ++سانى;	    // count 1 for "+-- folded" line
	    first += x;
	}
	وإلا
#endif
	{
#ifdef FEAT_DIFF
	    إذا (first == wp->w_topline)
		سانى += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    وإلا
#endif
		سانى += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    إذا (first == wp->w_buffer->b_ml.ml_قۇر_سانى + 1)
	سانى += diff_check_fill(wp, first);
#endif
    أرجع MIN(max, سانى);
}

    صحيح
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    صحيح		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    إذا (pos->col == MAXCOL)
	أرجع NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    إذا (pos->col > ptrlen)
	أرجع NUL;
    إذا (has_mbyte)
	أرجع (*mb_ptr2char)(ptr);
    أرجع (صحيح)*ptr;
}

    صحيح
gchar_cursor(فراغ)
{
    إذا (has_mbyte)
	أرجع (*mb_ptr2char)(ml_get_cursor());
    أرجع (صحيح)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    صحيح
char_before_cursor(فراغ)
{
    إذا (curwin->w_cursor.col == 0)
	أرجع -1;

    char_u *قۇر = ml_get_curline();

    إذا (has_mbyte)
    {
	char_u *p = قۇر + curwin->w_cursor.col;
	صحيح prev_len = (*mb_head_off)(قۇر, p - 1) + 1;
	أرجع mb_ptr2char(p - prev_len);
    }
    أرجع قۇر[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    فراغ
pchar_cursor(صحيح c)
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
    إذا (*p == ',')
	++p;
    طالما (*p == ' ')
	++p;
    أرجع p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    فراغ
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_buffer == buf && wp->w_status_height)
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
    صحيح
ask_yesno(char_u *str, صحيح direct)
{
    صحيح	    r = ' ';
    صحيح	    save_State = State;

    إذا (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ئاچقۇچلار;		// no mapping here, but recognize keys

    طالما (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	إذا (direct)
	    r = get_keystroke();
	وإلا
	    r = plain_vgetc();
	إذا (r == Ctrl_C || r == ESC)
	    r = 'n';
	ئۇچۇر_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ئاچقۇچلار;

    أرجع r;
}

#إذا defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    فراغ
get_mode(char_u *buf)
{
    صحيح		i = 0;

    إذا (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    وإلا إذا (term_use_loop())
    {
	إذا (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    وإلا إذا (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	إذا (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	وإلا إذا (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    وإلا إذا (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    وإلا إذا (State & MODE_قىستۇرۇش)
    {
	إذا (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	وإلا
	{
	    إذا (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    وإلا
		buf[i++] = 'i';
	}

	إذا (ins_compl_active())
	    buf[i++] = 'c';
	وإلا إذا (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    وإلا إذا ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	إذا (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	وإلا إذا (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	إذا ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    وإلا إذا (VIsual_active)
    {
	إذا (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	وإلا
	{
	    buf[i++] = VIsual_mode;
	    إذا (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    وإلا
    {
	buf[i++] = 'n';
	إذا (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	وإلا إذا (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	وإلا إذا (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    فراغ
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    إذا (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	أرجع;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    إذا (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ساكن فراغ
may_add_state_char(garray_T *gap, char_u *include, صحيح c)
{
    إذا (include == عدم || vim_strchr(include, c) != عدم)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    فراغ
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = عدم;
    صحيح		i;

    إذا (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    ga_init2(&ga, 1, 20);
    إذا (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    إذا (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == عدم))
	may_add_state_char(&ga, include, 'm');
    إذا (op_pending())
	may_add_state_char(&ga, include, 'o');
    إذا (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    إذا (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    إذا (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    إذا (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    لكل (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    إذا (ئۇچۇر_scrolled > 0)
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
    صحيح
get_keystroke(فراغ)
{
    char_u	*buf = عدم;
    صحيح		buflen = 150;
    صحيح		maxlen;
    صحيح		len = 0;
    صحيح		n;
    صحيح		save_mapped_ctrl_c = mapped_ctrl_c;
    صحيح		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    لكل (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	إذا (buf == عدم)
	    buf = alloc(buflen);
	وإلا إذا (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    إذا (buf == عدم)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	إذا (buf == عدم)
	{
	    do_outofmem_ئۇچۇر((long_u)buflen);
	    أرجع ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	إذا (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_كىرگۈزۈش_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	وإلا إذا (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	إذا ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    استمر;

	إذا (n == KEYLEN_REMOVED)  // key code removed
	{
	    إذا (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    استمر;
	}
	إذا (n > 0)		// found a termcode: adjust length
	    len = n;
	إذا (len == 0)		// nothing typed yet
	    استمر;

	// Handle modifier and/or special key code.
	n = buf[0];
	إذا (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    إذا (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ئاچقۇچ(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		إذا (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		إذا (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		استمر;
	    }
	    اكسر;
	}
	إذا (has_mbyte)
	{
	    إذا (MB_BYTE2LEN(n) > len)
		استمر;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	إذا (n == intr_char)
	    n = ESC;
#endif
	اكسر;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    أرجع merge_modify_Other_ئاچقۇچلار(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    ساكن صحيح
vim_append_digit_int(صحيح *قىممەت, صحيح digit)
{
    صحيح x = *قىممەت;
    إذا (x > ((INT_MAX - digit) / 10))
	أرجع FAIL;
    *قىممەت = x * 10 + digit;
    أرجع OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    صحيح
get_سان(
    صحيح	    colon,			// allow colon to abort
    صحيح	    *mouse_used)
{
    صحيح	n = 0;
    صحيح	c;
    صحيح typed = 0;

    إذا (mouse_used != عدم)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    إذا (ئۇچۇر_silent != 0)
	أرجع 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_ئاچقۇچلار;		// no mapping here, but recognize keys
    لكل (;;)
    {
	windgoto(ئۇچۇر_row, cmdline_col_off + ئۇچۇر_col);
	c = safe_vgetc();
	إذا (VIM_ISDIGIT(c))
	{
	    إذا (vim_append_digit_int(&n, c - '0') == FAIL)
		أرجع 0;
	    ئۇچۇر_putchar(c);
	    ++typed;
	}
	وإلا إذا (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    إذا (typed > 0)
	    {
		ئۇچۇر_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	وإلا إذا (mouse_used != عدم && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    اكسر;
	}
	وإلا إذا (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    إذا (!exmode_active)
		cmdline_row = ئۇچۇر_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    اكسر;
	}
	وإلا إذا (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    اكسر;
	}
	وإلا إذا (c == CAR || c == NL )
	    اكسر;
    }
    --no_mapping;
    --allow_ئاچقۇچلار;
    أرجع n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    صحيح
prompt_for_سان(صحيح *mouse_used)
{
    صحيح		i;
    صحيح		save_cmdline_row;
    صحيح		save_State;

    // When using ":silent" assume that <CR> was entered.
    إذا (mouse_used != عدم)
	ئۇچۇر_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    وإلا
	ئۇچۇر_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_سان(TRUE, mouse_used);
    إذا (ئاچقۇچ_Typed)
    {
	// don't call wait_return() now
	إذا (ئۇچۇر_row > 0)
	    cmdline_row = ئۇچۇر_row - 1;
	need_wait_return = FALSE;
	ئۇچۇر_didany = FALSE;
	ئۇچۇر_didout = FALSE;
    }
    وإلا
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    أرجع i;
}

    فراغ
msgmore(long n)
{
    long pn;

    إذا (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	أرجع;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    إذا (keep_ئۇچۇر != عدم && !keep_ئۇچۇر_more)
	أرجع;

    إذا (n > 0)
	pn = n;
    وإلا
	pn = -n;

    إذا (pn > p_report)
    {
	إذا (n > 0)
	    vim_snprintf(ئۇچۇر_buf, ئۇچۇر_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	وإلا
	    vim_snprintf(ئۇچۇر_buf, ئۇچۇر_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	إذا (got_int)
	    vim_strcat((char_u *)ئۇچۇر_buf, (char_u *)_(" (Interrupted)"),
								  ئۇچۇر_BUF_LEN);
	إذا (ئۇچۇر(ئۇچۇر_buf))
	{
	    set_keep_ئۇچۇر((char_u *)ئۇچۇر_buf, 0);
	    keep_ئۇچۇر_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    فراغ
beep_flush(فراغ)
{
    إذا (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    فراغ
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    إذا (emsg_silent != 0 || in_assert_fails)
	أرجع;

    إذا (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	ساكن صحيح		did_init = FALSE;
	ساكن elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	إذا (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    إذا (p_vb
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
		إذا (has_vtp_working() != 0
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
	    وإلا
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    إذا (vim_strchr(p_debug, 'e') != عدم)
    {
	ئۇچۇر_source(HL_ATTR(HLF_W));
	ئۇچۇر_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    فراغ
init_homedir(فراغ)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#وإلا
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    إذا (var == عدم || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	إذا (homepath == عدم || *homepath == NUL)
	    homepath = (char_u *)"\\";
	إذا (homedrive != عدم
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((حرف *)NameBuff, "%s%s", homedrive, homepath);
	    إذا (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    إذا (var == عدم)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    إذا (var != عدم && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	إذا (p != عدم)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    إذا (exp != عدم && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    إذا (var != عدم && *var == NUL)	// empty is same as not set
	var = عدم;

    إذا (enc_utf8 && var != عدم)
    {
	صحيح	len;
	char_u  *pp = عدم;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (صحيح)STRLEN(var), &pp, &len);
	إذا (pp != عدم)
	{
	    homedir = pp;
	    أرجع;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    إذا (var == عدم)
	var = (char_u *)"C:/";
#endif

    إذا (var != عدم)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	إذا (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((حرف *)NameBuff) == 0)
	{
	    إذا (!mch_chdir((حرف *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    إذا (mch_chdir((حرف *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#إذا defined(EXITFREE)
    فراغ
free_homedir(فراغ)
{
    vim_free(homedir);
}

    فراغ
free_users(فراغ)
{
    ga_clear_strings(&ga_users);
}
#endif

#إذا defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    فراغ
init_vimdir(فراغ)
{
    صحيح	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    إذا (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    إذا (mustfree)
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
    أرجع expand_env_save_opt(src, FALSE, عدم);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, صحيح one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    إذا (p != عدم)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, عدم);
    أرجع p;
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
    صحيح		dstlen)		// maximum length of the result
{
    أرجع expand_env_esc(src, dst, dstlen, عدم, FALSE, عدم);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    صحيح		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    صحيح		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    صحيح		c;
    char_u	*var;
    صحيح		copy_char;
    صحيح		mustfree;	// var was allocated, need to free it later
    صحيح		at_start = TRUE; // at start of a name
    صحيح		startstr_len = 0;
    char_u	*dst_start = dst;

    إذا (startstr != عدم)
	startstr_len = (صحيح)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    طالما (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	إذا (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (فراغ)skip_ئىپادە(&src, عدم);
	    إذا (*src == '`')
		++src;
	    len = src - var;
	    إذا (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (صحيح)len;
	    استمر;
	}
#endif
	copy_char = TRUE;
	إذا ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#إذا defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    إذا (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		إذا (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    طالما (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		وإلا
#endif
		{
		    طالما (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#إذا defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#إذا defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		إذا (src[1] == '{' && *tail != '}')
# وإلا
		إذا (*src == '%' && *tail != '%')
# endif
		    var = عدم;
		وإلا
		{
# ifdef UNIX
		    إذا (src[1] == '{')
# وإلا
		    إذا (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#إذا defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    وإلا إذا (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != عدم)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    وإلا					// user directory
	    {
#إذا defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		طالما (	   c-- > 0
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
#  إذا defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    بنية passwd *pw = (*dst == NUL)
					? عدم : getpwnam((حرف *)dst + 1);

		    var = (pw == عدم) ? عدم : (char_u *)pw->pw_dir;
		}
		إذا (var == عدم)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, عدم,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# وإلا	// !UNIX, thus VMS
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
		    طالما (*next_path)
		    {
			لكل (path = next_path; *next_path && *next_path != ',';
				next_path++);
			إذا (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (حرف *)test,
			    حجم(test),
			    "%s/%s",
			    path,
			    dst + 1);
			إذا (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    إذا (var != عدم)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    اكسر;
			}
		    }
		}
# endif // UNIX
#وإلا
		// cannot expand user's home directory, so don't try
		var = عدم;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    إذا (p_ssl && var != عدم && vim_strchr(var, '\\') != عدم)
	    {
		char_u	*p = vim_strsave(var);

		إذا (p != عدم)
		{
		    إذا (mustfree)
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
	    إذا (esc_chars != عدم && var != عدم
		    && vim_strpbrk(var, esc_chars) != عدم)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		إذا (p != عدم)
		{
		    إذا (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    إذا (var != عدم && *var != NUL)
	    {
		c = (صحيح)STRLEN(var);

		إذا (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    إذا (after_pathsep(dst, dst + c)
#إذا defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    إذا (mustfree)
		vim_free(var);
	}

	إذا (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    إذا (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    وإلا إذا ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    إذا (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		إذا (startstr != عدم && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    أرجع (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ساكن char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    صحيح		len = (صحيح)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    إذا (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	أرجع newend;
    أرجع pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ساكن char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    إذا (vimdir == عدم || *vimdir == NUL)
	أرجع عدم;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    إذا (p.string != عدم && mch_isdir(p.string))
	أرجع p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    إذا (p.string != عدم && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	إذا (fname.string != عدم)
	{
	    صحيح exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    إذا (exists)
		أرجع p.string;
	}
    }
    vim_free(p.string);
    أرجع عدم;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, صحيح *mustfree)
{
    char_u	*p = عدم;
    char_u	*pend;
    صحيح		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    إذا (STRCMP(name, "HOME") == 0)
	أرجع homedir;

    // Use Wide function
    wn = enc_to_utf16(name, عدم);
    إذا (wn == عدم)
	أرجع عدم;

    wp = _wgetenv(wn);
    vim_free(wn);

    إذا (wp != عدم && *wp == NUL)   // empty is the same as not set
	wp = عدم;

    إذا (wp != عدم)
    {
	p = utf16_to_enc(wp, عدم);
	إذا (p == عدم)
	    أرجع عدم;

	*mustfree = TRUE;
	أرجع p;
    }
#وإلا
    p = mch_getenv(name);
    إذا (p != عدم && *p == NUL)	    // empty is the same as not set
	p = عدم;

    إذا (p != عدم)
	أرجع p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    إذا (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ساكن حرف userSettingsPath[MAXPATHL];

	إذا (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    أرجع (char_u *)userSettingsPath;
	وإلا
	    أرجع عدم;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    إذا (!vimruntime && STRCMP(name, "VIM") != 0)
	أرجع عدم;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    إذا (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	إذا (wp != عدم && *wp == NUL)	    // empty is the same as not set
	    wp = عدم;
	إذا (wp != عدم)
	{
	    char_u *q = utf16_to_enc(wp, عدم);
	    إذا (q != عدم)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		إذا (p == عدم)
		    p = q;
	    }
	}
#وإلا
	p = mch_getenv((char_u *)"VIM");
	إذا (p != عدم && *p == NUL)	    // empty is the same as not set
	    p = عدم;
	إذا (p != عدم)
	{
	    p = vim_version_dir(p);
	    إذا (p != عدم)
		*mustfree = TRUE;
	    وإلا
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    إذا (p == عدم)
    {
	إذا (p_hf != عدم && vim_strchr(p_hf, '$') == عدم)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	وإلا
	    p = exe_name;
#endif
	إذا (p != عدم)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    إذا (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    إذا (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		إذا (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    إذا (pnew != عدم)
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
	    إذا (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    إذا (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    إذا (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    إذا (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    إذا (p != عدم && !mch_isdir(p))
		VIM_CLEAR(p);
	    وإلا
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		إذا (vimruntime && (pend = vim_version_dir(p)) != عدم)
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
    إذا (p == عدم)
    {
	// Only use default_vimruntime_dir when it is not empty
	إذا (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	وإلا إذا (*default_vim_dir != NUL)
	{
	    إذا (vimruntime && (p = vim_version_dir(default_vim_dir)) != عدم)
		*mustfree = TRUE;
	    وإلا
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
    إذا (p != عدم)
    {
	إذا (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	وإلا
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    أرجع p;
}

    فراغ
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((حرف *)var);
#وإلا
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    فراغ
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    إذا (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    وإلا إذا (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#إذا defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    فراغ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    إذا (STRICMP(name, "HOME") == 0)
	init_homedir();
    وإلا إذا (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    وإلا إذا (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    فراغ
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((حرف *)name, (حرف *)val, 1);
#وإلا
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    إذا (envbuf != عدم)
    {
	sprintf((حرف *)envbuf, "%s=%s", name, val);
	putenv((حرف *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    إذا (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	إذا (buf != عدم)
	{
	    bindtextdomain(VIMPACKAGE, (حرف *)buf);
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
    صحيح		idx)
{
#إذا defined(AMIGA)
    // No environ[] on the Amiga.
    أرجع عدم;
#وإلا
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    خارجي حرف		**environ;
# endif
    char_u		*str;
    صحيح			n;

    str = (char_u *)environ[idx];
    إذا (str == عدم)
	أرجع عدم;

    لكل (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	إذا (str[n] == '=' || str[n] == NUL)
	    اكسر;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    أرجع xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ساكن فراغ
add_user(char_u *user, صحيح need_copy)
{
    char_u	*user_copy = (user != عدم && need_copy)
						    ? vim_strsave(user) : user;

    إذا (user_copy == عدم || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	إذا (need_copy)
	    vim_free(user_copy);
	أرجع;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ساكن فراغ
init_users(فراغ)
{
    ساكن صحيح	lazy_init_done = FALSE;

    إذا (lazy_init_done)
	أرجع;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, حجم(char_u *), 20);

#إذا defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	بنية passwd*	pw;

	setpwent();
	طالما ((pw = getpwent()) != عدم)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#وإلاإذا defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	إذا (NetUserEnum(عدم, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, عدم) == NERR_Success)
	{
	    لكل (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, عدم), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#إذا defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	إذا (user_env != عدم && *user_env != NUL)
	{
	    صحيح	i;

	    لكل (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		إذا (STRCMP(local_user, user_env) == 0)
		    اكسر;
	    }

	    إذا (i == ga_users.ga_len)
	    {
		بنية passwd	*pw = getpwnam((حرف *)user_env);

		إذا (pw != عدم)
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
get_users(expand_T *xp UNUSED, صحيح idx)
{
    init_users();
    إذا (idx < ga_users.ga_len)
	أرجع ((char_u **)ga_users.ga_data)[idx];
    أرجع عدم;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    صحيح
match_user(char_u *name)
{
    صحيح i;
    صحيح n = (صحيح)STRLEN(name);
    صحيح result = 0;

    init_users();
    لكل (i = 0; i < ga_users.ga_len; i++)
    {
	إذا (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    أرجع 2; // full match
	إذا (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    أرجع result;
}

    ساكن فراغ
prepare_to_exit(فراغ)
{
#إذا defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    إذا (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    وإلا
#endif
    {
	windgoto((صحيح)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	إذا (!full_screen)
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
    فراغ
preserve_exit(فراغ)
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
	إذا (buf->b_ml.ml_mfp != عدم && buf->b_ml.ml_mfp->mf_fname != عدم)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    اكسر;
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

ساكن صحيح	breakcheck_سانى = 0;

    فراغ
قۇر_breakcheck(فراغ)
{
    إذا (++breakcheck_سانى >= BREAKCHECK_SKIP)
    {
	breakcheck_سانى = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    فراغ
fast_breakcheck(فراغ)
{
    إذا (++breakcheck_سانى >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_سانى = 0;
	ui_breakcheck();
    }
}

#إذا defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    فراغ
veryfast_breakcheck(فراغ)
{
    إذا (++breakcheck_سانى >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_سانى = 0;
	ui_breakcheck();
    }
}
#endif

#إذا defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    صحيح		flags,		// can be SHELL_SILENT
    صحيح		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = عدم;
    صحيح		len;
    صحيح		i = 0;
    FILE	*fd;

    إذا (check_restricted() || check_secure())
	أرجع عدم;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    إذا ((tempname = vim_tempname('o', TRUE)) == عدم)
    {
	emsg(_(e_cant_get_temp_file_name));
	أرجع عدم;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    إذا (command == عدم)
	اذهب done;

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
    fd = mch_fopen((حرف *)tempname, "r");
# وإلا
    fd = mch_fopen((حرف *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    إذا (fd == عدم
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	إذا (fd != عدم)
	    fclose(fd);
	اذهب done;
    }

    buffer = alloc(len + 1);
    إذا (buffer != عدم)
	i = (صحيح)fread((حرف *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    إذا (buffer == عدم)
	اذهب done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    إذا (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    وإلا إذا (ret_len == عدم)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	لكل (i = 0; i < len; ++i)
	    إذا (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    وإلا
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    أرجع buffer;
}

# إذا defined(FEAT_EVAL)

    ساكن فراغ
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    صحيح		retlist)
{
    char_u	*res = عدم;
    char_u	*p;
    char_u	*infile = عدم;
    صحيح		err = FALSE;
    FILE	*fd;
    list_T	*list = عدم;
    صحيح		flags = SHELL_SILENT;
    صحيح		use_argv = FALSE;
    حرف	**argv = عدم;
    صحيح		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    إذا (check_restricted() || check_secure())
	اذهب errret;

    إذا (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_سان_or_list_arg(argvars, 1)
								      == FAIL))
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	إذا ((infile = vim_tempname('i', TRUE)) == عدم)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    اذهب errret;
	}

	fd = mch_fopen((حرف *)infile, WRITEBIN);
	إذا (fd == عدم)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    اذهب errret;
	}
	إذا (argvars[1].v_type == VAR_سان)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_سان);
	    إذا (buf == عدم)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_سان);
		fclose(fd);
		اذهب errret;
	    }

	    لكل (lnum = 1; lnum <= buf->b_ml.ml_قۇر_سانى; lnum++)
	    {
		لكل (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    إذا (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			اكسر;
		    }
		إذا (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    اكسر;
		}
	    }
	}
	وإلا إذا (argvars[1].v_type == VAR_LIST)
	{
	    إذا (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	وإلا
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    إذا (p == عدم)
	    {
		fclose(fd);
		اذهب errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    إذا (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	إذا (fclose(fd) != 0)
	    err = TRUE;
	إذا (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    اذهب errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    إذا (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	إذا (l == عدم || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    اذهب errret;
	}
	إذا (build_argv_from_list(l, &argv, &argc) == FAIL)
	    اذهب errret;
	إذا (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    اذهب errret;
	}
	use_argv = TRUE;

	إذا (p_verbose > 3)
	{
	    صحيح		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    لكل (i = 0; i < argc; ++i)
	    {
		إذا (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (حرف *)ga.ga_data);
	    ئۇچۇر_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    إذا (!ئۇچۇر_silent)
	flags += SHELL_COOKED;

    إذا (retlist)
    {
	صحيح		len;
	listitem_T	*li;
	char_u		*s = عدم;
	char_u		*start;
	char_u		*end;
	صحيح		i;

	إذا (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	وإلا
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	إذا (res == عدم)
	    اذهب errret;

	list = list_alloc();
	إذا (list == عدم)
	    اذهب errret;

	لكل (i = 0; i < len; ++i)
	{
	    start = res + i;
	    طالما (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    إذا (s == عدم)
		اذهب errret;

	    لكل (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    إذا (li == عدم)
	    {
		vim_free(s);
		اذهب errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = عدم;
    }
    وإلا
    {
	إذا (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, عدم);
	وإلا
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, عدم);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	إذا (res != عدم)
	{
	    char_u	*s, *d;

	    d = res;
	    لكل (s = res; *s; ++s)
	    {
		إذا (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = عدم;
    }

errret:
    إذا (argv != عدم)
    {
	صحيح i;
	لكل (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    إذا (infile != عدم)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    إذا (res != عدم)
	vim_free(res);
    إذا (list != عدم)
	list_free(list);
}

/*
 * "system()" function
 */
    فراغ
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    فراغ
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
    صحيح
goto_im(فراغ)
{
    أرجع (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(فراغ)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#وإلا
    p = skiptowhite(p_sh);
    إذا (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    وإلا
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	لكل (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    إذا (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    أرجع p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    صحيح
path_is_url(char_u *p)
{
    إذا (STRNCMP(p, "://", (size_t)3) == 0)
	أرجع URL_SLASH;
    وإلا إذا (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	أرجع URL_BACKSLASH;
    أرجع 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    صحيح
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    إذا (!ASCII_ISALPHA(*fname))
	أرجع 0;

    // check body: alpha or dash
    لكل (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    إذا (p[-1] == '-')
	أرجع 0;

    // "://" or ":\\" must follow
    أرجع path_is_url(p);
}

#إذا defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    إذا (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    وإلا
	sve->sve_did_save = FALSE;
    أرجع v_event;
}

    فراغ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    إذا (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    وإلا
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    فراغ
may_trigger_modechanged(فراغ)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    إذا (!has_modechanged() || got_int)
	أرجع;

    get_mode(curr_mode);
    إذا (STRCMP(curr_mode, last_mode) == 0)
	أرجع;

    v_event = get_v_event(&save_v_event);
    (فراغ)dict_add_string(v_event, "new_mode", curr_mode);
    (فراغ)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_بۇيۇملار_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((حرف *)pattern_buf, حجم(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, عدم, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    صحيح
vim_append_digit_long(long *قىممەت, صحيح digit)
{
    long x = *قىممەت;
    إذا (x > ((LONG_MAX - (long)digit) / 10))
	أرجع FAIL;
    *قىممەت = x * 10 + (long)digit;
    أرجع OK;
}

// Return something that fits into an int.
    صحيح
trim_to_int(vimlong_T x)
{
    أرجع x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
