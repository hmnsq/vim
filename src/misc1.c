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

#אם defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#אם defined(MSWIN)
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
    char_u	*line,
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
    صحيح		middle_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = عدم;

    result = i = 0;
    כלעוד (VIM_ISWHITE(line[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    כלעוד (line[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	עבור (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    אם (!got_com && flags != عدم)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (فراغ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    אם (string == عدم)	    // missing ':', ignore this part
		استمر;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    אם (middle_match_len != 0
		    && vim_strchr(part_buf, COM_MIDDLE) == عدم
		    && vim_strchr(part_buf, COM_END) == عدم)
		اكسر;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    אם (got_com && vim_strchr(part_buf, COM_NEST) == عدم)
		استمر;

	    // When 'O' flag present and using "O" command skip this one.
	    אם (backward && vim_strchr(part_buf, COM_NOBACK) != عدم)
		استمر;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    אם (VIM_ISWHITE(string[0]))
	    {
		אם (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    استمر;  // missing white space
		כלעוד (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    עבור (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		;
	    אם (string[j] != NUL)
		استمر;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    אם (vim_strchr(part_buf, COM_BLANK) != عدم
			   && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		استمر;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    אם (vim_strchr(part_buf, COM_MIDDLE) != عدم)
	    {
		אם (middle_match_len == 0)
		{
		    middle_match_len = j;
		    saved_flags = prev_list;
		}
		استمر;
	    }
	    אם (middle_match_len != 0 && j > middle_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		middle_match_len = 0;

	    אם (middle_match_len == 0)
		i += j;
	    found_one = TRUE;
	    اكسر;
	}

	אם (middle_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    אם (!got_com && flags != عدم)
		*flags = saved_flags;
	    i += middle_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	אם (!found_one)
	    اكسر;

	result = i;

	// Include any trailing white space.
	כלעוד (VIM_ISWHITE(line[i]))
	    ++i;

	אם (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	אם (vim_strchr(part_buf, COM_NEST) == عدم)
	    اكسر;
    }
    החזר result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    صحيح
get_last_leader_offset(char_u *line, char_u **flags)
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
    i = (صحيح)STRLEN(line);
    כלעוד (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	עבור (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (فراغ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    אם (string == عدم)	// If everything is fine, this cannot actually
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
	    אם (VIM_ISWHITE(string[0]))
	    {
		אם (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    استمر;
		כלעוד (VIM_ISWHITE(*string))
		    ++string;
	    }
	    עבור (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		/* do nothing */;
	    אם (string[j] != NUL)
		استمر;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    אם (vim_strchr(part_buf, COM_BLANK) != عدم
		    && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		استمر;

	    אם (vim_strchr(part_buf, COM_MIDDLE) != عدم)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		עבור (j = 0; VIM_ISWHITE(line[j]) && j <= i; j++)
		    ;
		אם (j < i)
		    استمر;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    אם (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    اكسر;
	}

	אם (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    صحيح     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    אם (vim_strchr(part_buf, COM_NEST) != عدم)
		استمر;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    כלעוד (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (صحيح)STRLEN(com_leader);

	    עבור (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(فراغ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		אם (flags_save == com_flags)
		    استمر;
		string = vim_strchr(part_buf2, ':');
		++string;
		כלעוד (VIM_ISWHITE(*string))
		    ++string;
		len2 = (صحيح)STRLEN(string);
		אם (len2 == 0)
		    استمر;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		עבור (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    אם (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			אם (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    החזר result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    صحيح
plines(linenr_T lnum)
{
    החזר plines_win(curwin, lnum, TRUE);
}

    صحيح
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		limit_winheight)	// when TRUE limit to window height
{
#אם defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    החזר plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    صحيح
plines_nofill(linenr_T lnum)
{
    החזר plines_win_nofill(curwin, lnum, TRUE);
}

    صحيح
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    صحيح		limit_winheight)	// when TRUE limit to window height
{
#endif
    صحيح		lines;

    אם (wp->w_width == 0)
	החזר 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    אם (lineFolded(wp, lnum) == TRUE)
	החזר 1;
#endif

    אם (!wp->w_p_wrap)
	lines = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_count_above_below(wp->w_buffer, lnum)
#endif
	;
    אחרת
	lines = plines_win_nofold(wp, lnum);

    אם (limit_winheight && lines > wp->w_height)
	החזר wp->w_height;
    החזר lines;
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
    אם (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	החזר 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (صحيح)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    אם (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    אם (width <= 0)
	החזר 32000;
    אם (col <= width)
	החזר 1;
    col -= width;
    width += win_col_off2(wp);
    החזר (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    صحيح
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    صحيح		lines = 0;
    صحيح		width;
    char_u	*line;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    lines = diff_check_fill(wp, lnum);
#endif

    אם (!wp->w_p_wrap)
	החזר lines + 1;

    אם (wp->w_width == 0)
	החזר lines + 1;

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    // "column" is a byte index, advance the pointer until it is reached.
    כלעוד (*cts.cts_ptr != NUL && cts.cts_ptr < line + column)
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
    אם (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, عدم, عدم) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    אם (width <= 0)
	החזר 9999;

    lines += 1;
    אם (col > width)
	lines += (col - width) / (width + win_col_off2(wp)) + 1;
    החזר lines;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    صحيح
plines_m_win(win_T *wp, linenr_T first, linenr_T last, صحيح max)
{
    صحيح		count = 0;

    כלעוד (first <= last && count < max)
    {
#ifdef FEAT_FOLDING
	صحيح	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = foldedCount(wp, first, عدم);
	אם (x > 0)
	{
	    ++count;	    // count 1 for "+-- folded" line
	    first += x;
	}
	אחרת
#endif
	{
#ifdef FEAT_DIFF
	    אם (first == wp->w_topline)
		count += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    אחרת
#endif
		count += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    אם (first == wp->w_buffer->b_ml.ml_line_count + 1)
	count += diff_check_fill(wp, first);
#endif
    החזר MIN(max, count);
}

    صحيح
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    صحيح		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    אם (pos->col == MAXCOL)
	החזר NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    אם (pos->col > ptrlen)
	החזר NUL;
    אם (has_mbyte)
	החזר (*mb_ptr2char)(ptr);
    החזר (صحيح)*ptr;
}

    صحيح
gchar_cursor(فراغ)
{
    אם (has_mbyte)
	החזר (*mb_ptr2char)(ml_get_cursor());
    החזר (صحيح)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    صحيح
char_before_cursor(فراغ)
{
    אם (curwin->w_cursor.col == 0)
	החזר -1;

    char_u *line = ml_get_curline();

    אם (has_mbyte)
    {
	char_u *p = line + curwin->w_cursor.col;
	صحيح prev_len = (*mb_head_off)(line, p - 1) + 1;
	החזר mb_ptr2char(p - prev_len);
    }
    החזר line[curwin->w_cursor.col - 1];
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
    אם (*p == ',')
	++p;
    כלעוד (*p == ' ')
	++p;
    החזר p;
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
	אם (wp->w_buffer == buf && wp->w_status_height)
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

    אם (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys

    כלעוד (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	אם (direct)
	    r = get_keystroke();
	אחרת
	    r = plain_vgetc();
	אם (r == Ctrl_C || r == ESC)
	    r = 'n';
	msg_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_keys;

    החזר r;
}

#אם defined(FEAT_EVAL)

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

    אם (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    אחרת אם (term_use_loop())
    {
	אם (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    אחרת אם (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	אם (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	אחרת אם (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    אחרת אם (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    אחרת אם (State & MODE_INSERT)
    {
	אם (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	אחרת
	{
	    אם (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    אחרת
		buf[i++] = 'i';
	}

	אם (ins_compl_active())
	    buf[i++] = 'c';
	אחרת אם (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    אחרת אם ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	אם (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	אחרת אם (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	אם ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    אחרת אם (VIsual_active)
    {
	אם (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	אחרת
	{
	    buf[i++] = VIsual_mode;
	    אם (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    אחרת
    {
	buf[i++] = 'n';
	אם (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	אחרת אם (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	אחרת אם (term_in_normal_mode(curbuf))
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

    אם (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	החזר;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    אם (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ساكن فراغ
may_add_state_char(garray_T *gap, char_u *include, صحيح c)
{
    אם (include == عدم || vim_strchr(include, c) != عدم)
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

    אם (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	החזר;

    ga_init2(&ga, 1, 20);
    אם (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    אם (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == عدم))
	may_add_state_char(&ga, include, 'm');
    אם (op_pending())
	may_add_state_char(&ga, include, 'o');
    אם (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    אם (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    אם (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    אם (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    עבור (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    אם (msg_scrolled > 0)
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
    עבור (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	אם (buf == عدم)
	    buf = alloc(buflen);
	אחרת אם (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    אם (buf == عدم)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	אם (buf == عدم)
	{
	    do_outofmem_msg((long_u)buflen);
	    החזר ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	אם (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_input_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	אחרת אם (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	אם ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    استمر;

	אם (n == KEYLEN_REMOVED)  // key code removed
	{
	    אם (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    استمر;
	}
	אם (n > 0)		// found a termcode: adjust length
	    len = n;
	אם (len == 0)		// nothing typed yet
	    استمر;

	// Handle modifier and/or special key code.
	n = buf[0];
	אם (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    אם (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_key(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		אם (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		אם (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		استمر;
	    }
	    اكسر;
	}
	אם (has_mbyte)
	{
	    אם (MB_BYTE2LEN(n) > len)
		استمر;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	אם (n == intr_char)
	    n = ESC;
#endif
	اكسر;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    החזר merge_modifyOtherKeys(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    ساكن صحيح
vim_append_digit_int(صحيح *value, صحيح digit)
{
    صحيح x = *value;
    אם (x > ((INT_MAX - digit) / 10))
	החזר FAIL;
    *value = x * 10 + digit;
    החזר OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    صحيح
get_number(
    صحيح	    colon,			// allow colon to abort
    صحيح	    *mouse_used)
{
    صحيح	n = 0;
    صحيح	c;
    صحيح typed = 0;

    אם (mouse_used != عدم)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    אם (msg_silent != 0)
	החזר 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys
    עבור (;;)
    {
	windgoto(msg_row, cmdline_col_off + msg_col);
	c = safe_vgetc();
	אם (VIM_ISDIGIT(c))
	{
	    אם (vim_append_digit_int(&n, c - '0') == FAIL)
		החזר 0;
	    msg_putchar(c);
	    ++typed;
	}
	אחרת אם (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    אם (typed > 0)
	    {
		msg_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	אחרת אם (mouse_used != عدم && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    اكسر;
	}
	אחרת אם (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    אם (!exmode_active)
		cmdline_row = msg_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    اكسر;
	}
	אחרת אם (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    اكسر;
	}
	אחרת אם (c == CAR || c == NL )
	    اكسر;
    }
    --no_mapping;
    --allow_keys;
    החזר n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    صحيح
prompt_for_number(صحيح *mouse_used)
{
    صحيح		i;
    صحيح		save_cmdline_row;
    صحيح		save_State;

    // When using ":silent" assume that <CR> was entered.
    אם (mouse_used != عدم)
	msg_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    אחרת
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
    אם (KeyTyped)
    {
	// don't call wait_return() now
	אם (msg_row > 0)
	    cmdline_row = msg_row - 1;
	need_wait_return = FALSE;
	msg_didany = FALSE;
	msg_didout = FALSE;
    }
    אחרת
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    החזר i;
}

    فراغ
msgmore(long n)
{
    long pn;

    אם (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	החזר;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    אם (keep_msg != عدم && !keep_msg_more)
	החזר;

    אם (n > 0)
	pn = n;
    אחרת
	pn = -n;

    אם (pn > p_report)
    {
	אם (n > 0)
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	אחרת
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	אם (got_int)
	    vim_strcat((char_u *)msg_buf, (char_u *)_(" (Interrupted)"),
								  MSG_BUF_LEN);
	אם (msg(msg_buf))
	{
	    set_keep_msg((char_u *)msg_buf, 0);
	    keep_msg_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    فراغ
beep_flush(فراغ)
{
    אם (emsg_silent == 0)
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

    אם (emsg_silent != 0 || in_assert_fails)
	החזר;

    אם (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	ساكن صحيح		did_init = FALSE;
	ساكن elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	אם (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    אם (p_vb
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
		אם (has_vtp_working() != 0
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
	    אחרת
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    אם (vim_strchr(p_debug, 'e') != عدم)
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
    فراغ
init_homedir(فراغ)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#אחרת
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    אם (var == عدم || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	אם (homepath == عدم || *homepath == NUL)
	    homepath = (char_u *)"\\";
	אם (homedrive != عدم
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((حرف *)NameBuff, "%s%s", homedrive, homepath);
	    אם (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    אם (var == عدم)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    אם (var != عدم && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	אם (p != عدم)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    אם (exp != عدم && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((حرف *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    אם (var != عدم && *var == NUL)	// empty is same as not set
	var = عدم;

    אם (enc_utf8 && var != عدم)
    {
	صحيح	len;
	char_u  *pp = عدم;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (صحيح)STRLEN(var), &pp, &len);
	אם (pp != عدم)
	{
	    homedir = pp;
	    החזר;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    אם (var == عدم)
	var = (char_u *)"C:/";
#endif

    אם (var != عدم)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	אם (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((حرف *)NameBuff) == 0)
	{
	    אם (!mch_chdir((حرف *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    אם (mch_chdir((حرف *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#אם defined(EXITFREE)
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

#אם defined(MSWIN)
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
    אם (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    אם (mustfree)
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
    החזר expand_env_save_opt(src, FALSE, عدم);
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
    אם (p != عدم)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, عدم);
    החזר p;
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
    החזר expand_env_esc(src, dst, dstlen, عدم, FALSE, عدم);
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

    אם (startstr != عدم)
	startstr_len = (صحيح)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    כלעוד (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	אם (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (فراغ)skip_expr(&src, عدم);
	    אם (*src == '`')
		++src;
	    len = src - var;
	    אם (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (صحيح)len;
	    استمر;
	}
#endif
	copy_char = TRUE;
	אם ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#אם defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    אם (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		אם (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    כלעוד (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		אחרת
#endif
		{
		    כלעוד (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#אם defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#אם defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		אם (src[1] == '{' && *tail != '}')
# אחרת
		אם (*src == '%' && *tail != '%')
# endif
		    var = عدم;
		אחרת
		{
# ifdef UNIX
		    אם (src[1] == '{')
# אחרת
		    אם (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#אם defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    אחרת אם (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != عدم)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    אחרת					// user directory
	    {
#אם defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		כלעוד (	   c-- > 0
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
#  אם defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    بنية passwd *pw = (*dst == NUL)
					? عدم : getpwnam((حرف *)dst + 1);

		    var = (pw == عدم) ? عدم : (char_u *)pw->pw_dir;
		}
		אם (var == عدم)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, عدم,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# אחרת	// !UNIX, thus VMS
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
		    כלעוד (*next_path)
		    {
			עבור (path = next_path; *next_path && *next_path != ',';
				next_path++);
			אם (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (حرف *)test,
			    حجم(test),
			    "%s/%s",
			    path,
			    dst + 1);
			אם (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    אם (var != عدم)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    اكسر;
			}
		    }
		}
# endif // UNIX
#אחרת
		// cannot expand user's home directory, so don't try
		var = عدم;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    אם (p_ssl && var != عدم && vim_strchr(var, '\\') != عدم)
	    {
		char_u	*p = vim_strsave(var);

		אם (p != عدم)
		{
		    אם (mustfree)
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
	    אם (esc_chars != عدم && var != عدم
		    && vim_strpbrk(var, esc_chars) != عدم)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		אם (p != عدم)
		{
		    אם (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    אם (var != عدم && *var != NUL)
	    {
		c = (صحيح)STRLEN(var);

		אם (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    אם (after_pathsep(dst, dst + c)
#אם defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    אם (mustfree)
		vim_free(var);
	}

	אם (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    אם (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    אחרת אם ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    אם (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		אם (startstr != عدم && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    החזר (size_t)(dst - dst_start);
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

    אם (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	החזר newend;
    החזר pend;
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

    אם (vimdir == عدم || *vimdir == NUL)
	החזר عدم;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    אם (p.string != عدم && mch_isdir(p.string))
	החזר p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    אם (p.string != عدم && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	אם (fname.string != عدم)
	{
	    صحيح exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    אם (exists)
		החזר p.string;
	}
    }
    vim_free(p.string);
    החזר عدم;
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
    אם (STRCMP(name, "HOME") == 0)
	החזר homedir;

    // Use Wide function
    wn = enc_to_utf16(name, عدم);
    אם (wn == عدم)
	החזר عدم;

    wp = _wgetenv(wn);
    vim_free(wn);

    אם (wp != عدم && *wp == NUL)   // empty is the same as not set
	wp = عدم;

    אם (wp != عدم)
    {
	p = utf16_to_enc(wp, عدم);
	אם (p == عدم)
	    החזר عدم;

	*mustfree = TRUE;
	החזר p;
    }
#אחרת
    p = mch_getenv(name);
    אם (p != عدم && *p == NUL)	    // empty is the same as not set
	p = عدم;

    אם (p != عدم)
	החזר p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    אם (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ساكن حرف userSettingsPath[MAXPATHL];

	אם (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    החזר (char_u *)userSettingsPath;
	אחרת
	    החזר عدم;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    אם (!vimruntime && STRCMP(name, "VIM") != 0)
	החזר عدم;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    אם (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	אם (wp != عدم && *wp == NUL)	    // empty is the same as not set
	    wp = عدم;
	אם (wp != عدم)
	{
	    char_u *q = utf16_to_enc(wp, عدم);
	    אם (q != عدم)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		אם (p == عدم)
		    p = q;
	    }
	}
#אחרת
	p = mch_getenv((char_u *)"VIM");
	אם (p != عدم && *p == NUL)	    // empty is the same as not set
	    p = عدم;
	אם (p != عدم)
	{
	    p = vim_version_dir(p);
	    אם (p != عدم)
		*mustfree = TRUE;
	    אחרת
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    אם (p == عدم)
    {
	אם (p_hf != عدم && vim_strchr(p_hf, '$') == عدم)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	אחרת
	    p = exe_name;
#endif
	אם (p != عدم)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    אם (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    אם (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		אם (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    אם (pnew != عدم)
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
	    אם (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    אם (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    אם (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    אם (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    אם (p != عدم && !mch_isdir(p))
		VIM_CLEAR(p);
	    אחרת
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		אם (vimruntime && (pend = vim_version_dir(p)) != عدم)
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
    אם (p == عدم)
    {
	// Only use default_vimruntime_dir when it is not empty
	אם (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	אחרת אם (*default_vim_dir != NUL)
	{
	    אם (vimruntime && (p = vim_version_dir(default_vim_dir)) != عدم)
		*mustfree = TRUE;
	    אחרת
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
    אם (p != عدم)
    {
	אם (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	אחרת
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    החזר p;
}

    فراغ
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((حرف *)var);
#אחרת
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
    אם (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    אחרת אם (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#אם defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    فراغ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    אם (STRICMP(name, "HOME") == 0)
	init_homedir();
    אחרת אם (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    אחרת אם (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
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
#אחרת
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    אם (envbuf != عدم)
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
    אם (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	אם (buf != عدم)
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
#אם defined(AMIGA)
    // No environ[] on the Amiga.
    החזר عدم;
#אחרת
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    خارجي حرف		**environ;
# endif
    char_u		*str;
    صحيح			n;

    str = (char_u *)environ[idx];
    אם (str == عدم)
	החזר عدم;

    עבור (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	אם (str[n] == '=' || str[n] == NUL)
	    اكسر;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    החזר xp->xp_buf;
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

    אם (user_copy == عدم || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	אם (need_copy)
	    vim_free(user_copy);
	החזר;
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

    אם (lazy_init_done)
	החזר;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, حجم(char_u *), 20);

#אם defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	بنية passwd*	pw;

	setpwent();
	כלעוד ((pw = getpwent()) != عدم)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#وإلاإذا defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	אם (NetUserEnum(عدم, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, عدم) == NERR_Success)
	{
	    עבור (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, عدم), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#אם defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	אם (user_env != عدم && *user_env != NUL)
	{
	    صحيح	i;

	    עבור (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		אם (STRCMP(local_user, user_env) == 0)
		    اكسر;
	    }

	    אם (i == ga_users.ga_len)
	    {
		بنية passwd	*pw = getpwnam((حرف *)user_env);

		אם (pw != عدم)
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
    אם (idx < ga_users.ga_len)
	החזר ((char_u **)ga_users.ga_data)[idx];
    החזר عدم;
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
    עבור (i = 0; i < ga_users.ga_len; i++)
    {
	אם (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    החזר 2; // full match
	אם (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    החזר result;
}

    ساكن فراغ
prepare_to_exit(فراغ)
{
#אם defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    אם (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    אחרת
#endif
    {
	windgoto((صحيح)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	אם (!full_screen)
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
	אם (buf->b_ml.ml_mfp != عدم && buf->b_ml.ml_mfp->mf_fname != عدم)
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

ساكن صحيح	breakcheck_count = 0;

    فراغ
line_breakcheck(فراغ)
{
    אם (++breakcheck_count >= BREAKCHECK_SKIP)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    فراغ
fast_breakcheck(فراغ)
{
    אם (++breakcheck_count >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}

#אם defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    فراغ
veryfast_breakcheck(فراغ)
{
    אם (++breakcheck_count >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}
#endif

#אם defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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

    אם (check_restricted() || check_secure())
	החזר عدم;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    אם ((tempname = vim_tempname('o', TRUE)) == عدم)
    {
	emsg(_(e_cant_get_temp_file_name));
	החזר عدم;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    אם (command == عدم)
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
# אחרת
    fd = mch_fopen((حرف *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    אם (fd == عدم
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	אם (fd != عدم)
	    fclose(fd);
	اذهب done;
    }

    buffer = alloc(len + 1);
    אם (buffer != عدم)
	i = (صحيح)fread((حرف *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    אם (buffer == عدم)
	اذهب done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    אם (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    אחרת אם (ret_len == عدم)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	עבור (i = 0; i < len; ++i)
	    אם (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    אחרת
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    החזר buffer;
}

# אם defined(FEAT_EVAL)

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
    אם (check_restricted() || check_secure())
	اذهب errret;

    אם (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_or_list_arg(argvars, 1)
								      == FAIL))
	החזר;

    אם (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	אם ((infile = vim_tempname('i', TRUE)) == عدم)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    اذهب errret;
	}

	fd = mch_fopen((حرف *)infile, WRITEBIN);
	אם (fd == عدم)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    اذهب errret;
	}
	אם (argvars[1].v_type == VAR_NUMBER)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_number);
	    אם (buf == عدم)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_number);
		fclose(fd);
		اذهب errret;
	    }

	    עבור (lnum = 1; lnum <= buf->b_ml.ml_line_count; lnum++)
	    {
		עבור (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    אם (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			اكسر;
		    }
		אם (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    اكسر;
		}
	    }
	}
	אחרת אם (argvars[1].v_type == VAR_LIST)
	{
	    אם (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	אחרת
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    אם (p == عدم)
	    {
		fclose(fd);
		اذهب errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    אם (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	אם (fclose(fd) != 0)
	    err = TRUE;
	אם (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    اذهب errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    אם (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	אם (l == عدم || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    اذهب errret;
	}
	אם (build_argv_from_list(l, &argv, &argc) == FAIL)
	    اذهب errret;
	אם (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    اذهب errret;
	}
	use_argv = TRUE;

	אם (p_verbose > 3)
	{
	    صحيح		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    עבור (i = 0; i < argc; ++i)
	    {
		אם (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (حرف *)ga.ga_data);
	    msg_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    אם (!msg_silent)
	flags += SHELL_COOKED;

    אם (retlist)
    {
	صحيح		len;
	listitem_T	*li;
	char_u		*s = عدم;
	char_u		*start;
	char_u		*end;
	صحيح		i;

	אם (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	אחרת
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	אם (res == عدم)
	    اذهب errret;

	list = list_alloc();
	אם (list == عدم)
	    اذهب errret;

	עבור (i = 0; i < len; ++i)
	{
	    start = res + i;
	    כלעוד (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    אם (s == عدم)
		اذهب errret;

	    עבור (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    אם (li == عدم)
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
    אחרת
    {
	אם (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, عدم);
	אחרת
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, عدم);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	אם (res != عدم)
	{
	    char_u	*s, *d;

	    d = res;
	    עבור (s = res; *s; ++s)
	    {
		אם (s[0] == CAR && s[1] == NL)
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
    אם (argv != عدم)
    {
	صحيح i;
	עבור (i = 0; argv[i] != عدم; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    אם (infile != عدم)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    אם (res != عدم)
	vim_free(res);
    אם (list != عدم)
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
    החזר (p_im && stuff_empty() && typebuf_typed());
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
#אחרת
    p = skiptowhite(p_sh);
    אם (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    אחרת
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	עבור (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    אם (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    החזר p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    صحيح
path_is_url(char_u *p)
{
    אם (STRNCMP(p, "://", (size_t)3) == 0)
	החזר URL_SLASH;
    אחרת אם (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	החזר URL_BACKSLASH;
    החזר 0;
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
    אם (!ASCII_ISALPHA(*fname))
	החזר 0;

    // check body: alpha or dash
    עבור (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    אם (p[-1] == '-')
	החזר 0;

    // "://" or ":\\" must follow
    החזר path_is_url(p);
}

#אם defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    אם (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    אחרת
	sve->sve_did_save = FALSE;
    החזר v_event;
}

    فراغ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    אם (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    אחרת
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
    אם (!has_modechanged() || got_int)
	החזר;

    get_mode(curr_mode);
    אם (STRCMP(curr_mode, last_mode) == 0)
	החזר;

    v_event = get_v_event(&save_v_event);
    (فراغ)dict_add_string(v_event, "new_mode", curr_mode);
    (فراغ)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_items_ro(v_event);

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
vim_append_digit_long(long *value, صحيح digit)
{
    long x = *value;
    אם (x > ((LONG_MAX - (long)digit) / 10))
	החזר FAIL;
    *value = x * 10 + (long)digit;
    החזר OK;
}

// Return something that fits into an int.
    صحيح
trim_to_int(vimlong_T x)
{
    החזר x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
