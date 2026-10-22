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

#如果 defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#如果 defined(MSWIN)
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
    整数
get_leader_len(
    char_u	*line,
    char_u	**flags,
    整数		backward,
    整数		include_space)
{
    整数		i, j;
    整数		result;
    整数		got_com = FALSE;
    整数		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    整数		middle_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = NULL;

    result = i = 0;
    当 (VIM_ISWHITE(line[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    当 (line[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	为 (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    如果 (!got_com && flags != NULL)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (void)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    如果 (string == NULL)	    // missing ':', ignore this part
		continue;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    如果 (middle_match_len != 0
		    && vim_strchr(part_buf, COM_MIDDLE) == NULL
		    && vim_strchr(part_buf, COM_END) == NULL)
		break;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    如果 (got_com && vim_strchr(part_buf, COM_NEST) == NULL)
		continue;

	    // When 'O' flag present and using "O" command skip this one.
	    如果 (backward && vim_strchr(part_buf, COM_NOBACK) != NULL)
		continue;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    如果 (VIM_ISWHITE(string[0]))
	    {
		如果 (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    continue;  // missing white space
		当 (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    为 (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		;
	    如果 (string[j] != NUL)
		continue;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    如果 (vim_strchr(part_buf, COM_BLANK) != NULL
			   && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		continue;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    如果 (vim_strchr(part_buf, COM_MIDDLE) != NULL)
	    {
		如果 (middle_match_len == 0)
		{
		    middle_match_len = j;
		    saved_flags = prev_list;
		}
		continue;
	    }
	    如果 (middle_match_len != 0 && j > middle_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		middle_match_len = 0;

	    如果 (middle_match_len == 0)
		i += j;
	    found_one = TRUE;
	    break;
	}

	如果 (middle_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    如果 (!got_com && flags != NULL)
		*flags = saved_flags;
	    i += middle_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	如果 (!found_one)
	    break;

	result = i;

	// Include any trailing white space.
	当 (VIM_ISWHITE(line[i]))
	    ++i;

	如果 (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	如果 (vim_strchr(part_buf, COM_NEST) == NULL)
	    break;
    }
    返回 result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    整数
get_last_leader_offset(char_u *line, char_u **flags)
{
    整数		result = -1;
    整数		i, j;
    整数		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    整数		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (整数)STRLEN(line);
    当 (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	为 (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (void)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    如果 (string == NULL)	// If everything is fine, this cannot actually
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
	    如果 (VIM_ISWHITE(string[0]))
	    {
		如果 (i == 0 || !VIM_ISWHITE(line[i - 1]))
		    continue;
		当 (VIM_ISWHITE(*string))
		    ++string;
	    }
	    为 (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		/* do nothing */;
	    如果 (string[j] != NUL)
		continue;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    如果 (vim_strchr(part_buf, COM_BLANK) != NULL
		    && !VIM_ISWHITE(line[i + j]) && line[i + j] != NUL)
		continue;

	    如果 (vim_strchr(part_buf, COM_MIDDLE) != NULL)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		为 (j = 0; VIM_ISWHITE(line[j]) && j <= i; j++)
		    ;
		如果 (j < i)
		    continue;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    如果 (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    break;
	}

	如果 (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    整数     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    如果 (vim_strchr(part_buf, COM_NEST) != NULL)
		continue;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    当 (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (整数)STRLEN(com_leader);

	    为 (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(void)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		如果 (flags_save == com_flags)
		    continue;
		string = vim_strchr(part_buf2, ':');
		++string;
		当 (VIM_ISWHITE(*string))
		    ++string;
		len2 = (整数)STRLEN(string);
		如果 (len2 == 0)
		    continue;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		为 (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    如果 (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			如果 (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    返回 result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    整数
plines(linenr_T lnum)
{
    返回 plines_win(curwin, lnum, TRUE);
}

    整数
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    整数		limit_winheight)	// when TRUE limit to window height
{
#如果 defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    返回 plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    整数
plines_nofill(linenr_T lnum)
{
    返回 plines_win_nofill(curwin, lnum, TRUE);
}

    整数
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    整数		limit_winheight)	// when TRUE limit to window height
{
#endif
    整数		lines;

    如果 (wp->w_width == 0)
	返回 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    如果 (lineFolded(wp, lnum) == TRUE)
	返回 1;
#endif

    如果 (!wp->w_p_wrap)
	lines = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_count_above_below(wp->w_buffer, lnum)
#endif
	;
    否则
	lines = plines_win_nofold(wp, lnum);

    如果 (limit_winheight && lines > wp->w_height)
	返回 wp->w_height;
    返回 lines;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    整数
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    整数		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    如果 (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	返回 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (整数)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    如果 (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    如果 (width <= 0)
	返回 32000;
    如果 (col <= width)
	返回 1;
    col -= width;
    width += win_col_off2(wp);
    返回 (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    整数
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    整数		lines = 0;
    整数		width;
    char_u	*line;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    lines = diff_check_fill(wp, lnum);
#endif

    如果 (!wp->w_p_wrap)
	返回 lines + 1;

    如果 (wp->w_width == 0)
	返回 lines + 1;

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    // "column" is a byte index, advance the pointer until it is reached.
    当 (*cts.cts_ptr != NUL && cts.cts_ptr < line + column)
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
    如果 (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, NULL, NULL) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    如果 (width <= 0)
	返回 9999;

    lines += 1;
    如果 (col > width)
	lines += (col - width) / (width + win_col_off2(wp)) + 1;
    返回 lines;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    整数
plines_m_win(win_T *wp, linenr_T first, linenr_T last, 整数 max)
{
    整数		count = 0;

    当 (first <= last && count < max)
    {
#ifdef FEAT_FOLDING
	整数	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = foldedCount(wp, first, NULL);
	如果 (x > 0)
	{
	    ++count;	    // count 1 for "+-- folded" line
	    first += x;
	}
	否则
#endif
	{
#ifdef FEAT_DIFF
	    如果 (first == wp->w_topline)
		count += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    否则
#endif
		count += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    如果 (first == wp->w_buffer->b_ml.ml_line_count + 1)
	count += diff_check_fill(wp, first);
#endif
    返回 MIN(max, count);
}

    整数
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    整数		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    如果 (pos->col == MAXCOL)
	返回 NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    如果 (pos->col > ptrlen)
	返回 NUL;
    如果 (has_mbyte)
	返回 (*mb_ptr2char)(ptr);
    返回 (整数)*ptr;
}

    整数
gchar_cursor(void)
{
    如果 (has_mbyte)
	返回 (*mb_ptr2char)(ml_get_cursor());
    返回 (整数)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    整数
char_before_cursor(void)
{
    如果 (curwin->w_cursor.col == 0)
	返回 -1;

    char_u *line = ml_get_curline();

    如果 (has_mbyte)
    {
	char_u *p = line + curwin->w_cursor.col;
	整数 prev_len = (*mb_head_off)(line, p - 1) + 1;
	返回 mb_ptr2char(p - prev_len);
    }
    返回 line[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    void
pchar_cursor(整数 c)
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
    如果 (*p == ',')
	++p;
    当 (*p == ' ')
	++p;
    返回 p;
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
	如果 (wp->w_buffer == buf && wp->w_status_height)
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
    整数
ask_yesno(char_u *str, 整数 direct)
{
    整数	    r = ' ';
    整数	    save_State = State;

    如果 (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys

    当 (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	如果 (direct)
	    r = get_keystroke();
	否则
	    r = plain_vgetc();
	如果 (r == Ctrl_C || r == ESC)
	    r = 'n';
	msg_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_keys;

    返回 r;
}

#如果 defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    void
get_mode(char_u *buf)
{
    整数		i = 0;

    如果 (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    否则 如果 (term_use_loop())
    {
	如果 (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    否则 如果 (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	如果 (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	否则 如果 (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    否则 如果 (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    否则 如果 (State & MODE_INSERT)
    {
	如果 (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	否则
	{
	    如果 (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    否则
		buf[i++] = 'i';
	}

	如果 (ins_compl_active())
	    buf[i++] = 'c';
	否则 如果 (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    否则 如果 ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	如果 (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	否则 如果 (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	如果 ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    否则 如果 (VIsual_active)
    {
	如果 (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	否则
	{
	    buf[i++] = VIsual_mode;
	    如果 (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    否则
    {
	buf[i++] = 'n';
	如果 (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	否则 如果 (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	否则 如果 (term_in_normal_mode(curbuf))
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

    如果 (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	返回;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    如果 (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    static void
may_add_state_char(garray_T *gap, char_u *include, 整数 c)
{
    如果 (include == NULL || vim_strchr(include, c) != NULL)
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
    整数		i;

    如果 (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	返回;

    ga_init2(&ga, 1, 20);
    如果 (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    如果 (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == NULL))
	may_add_state_char(&ga, include, 'm');
    如果 (op_pending())
	may_add_state_char(&ga, include, 'o');
    如果 (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    如果 (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    如果 (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    如果 (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    为 (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    如果 (msg_scrolled > 0)
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
    整数
get_keystroke(void)
{
    char_u	*buf = NULL;
    整数		buflen = 150;
    整数		maxlen;
    整数		len = 0;
    整数		n;
    整数		save_mapped_ctrl_c = mapped_ctrl_c;
    整数		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    为 (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	如果 (buf == NULL)
	    buf = alloc(buflen);
	否则 如果 (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    如果 (buf == NULL)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	如果 (buf == NULL)
	{
	    do_outofmem_msg((long_u)buflen);
	    返回 ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	如果 (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_input_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	否则 如果 (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	如果 ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    continue;

	如果 (n == KEYLEN_REMOVED)  // key code removed
	{
	    如果 (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    continue;
	}
	如果 (n > 0)		// found a termcode: adjust length
	    len = n;
	如果 (len == 0)		// nothing typed yet
	    continue;

	// Handle modifier and/or special key code.
	n = buf[0];
	如果 (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    如果 (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_key(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		如果 (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		如果 (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		continue;
	    }
	    break;
	}
	如果 (has_mbyte)
	{
	    如果 (MB_BYTE2LEN(n) > len)
		continue;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	如果 (n == intr_char)
	    n = ESC;
#endif
	break;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    返回 merge_modifyOtherKeys(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    static 整数
vim_append_digit_int(整数 *value, 整数 digit)
{
    整数 x = *value;
    如果 (x > ((INT_MAX - digit) / 10))
	返回 FAIL;
    *value = x * 10 + digit;
    返回 OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    整数
get_number(
    整数	    colon,			// allow colon to abort
    整数	    *mouse_used)
{
    整数	n = 0;
    整数	c;
    整数 typed = 0;

    如果 (mouse_used != NULL)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    如果 (msg_silent != 0)
	返回 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_keys;		// no mapping here, but recognize keys
    为 (;;)
    {
	windgoto(msg_row, cmdline_col_off + msg_col);
	c = safe_vgetc();
	如果 (VIM_ISDIGIT(c))
	{
	    如果 (vim_append_digit_int(&n, c - '0') == FAIL)
		返回 0;
	    msg_putchar(c);
	    ++typed;
	}
	否则 如果 (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    如果 (typed > 0)
	    {
		msg_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	否则 如果 (mouse_used != NULL && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    break;
	}
	否则 如果 (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    如果 (!exmode_active)
		cmdline_row = msg_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    break;
	}
	否则 如果 (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    break;
	}
	否则 如果 (c == CAR || c == NL )
	    break;
    }
    --no_mapping;
    --allow_keys;
    返回 n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    整数
prompt_for_number(整数 *mouse_used)
{
    整数		i;
    整数		save_cmdline_row;
    整数		save_State;

    // When using ":silent" assume that <CR> was entered.
    如果 (mouse_used != NULL)
	msg_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    否则
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
    如果 (KeyTyped)
    {
	// don't call wait_return() now
	如果 (msg_row > 0)
	    cmdline_row = msg_row - 1;
	need_wait_return = FALSE;
	msg_didany = FALSE;
	msg_didout = FALSE;
    }
    否则
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    返回 i;
}

    void
msgmore(long n)
{
    long pn;

    如果 (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	返回;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    如果 (keep_msg != NULL && !keep_msg_more)
	返回;

    如果 (n > 0)
	pn = n;
    否则
	pn = -n;

    如果 (pn > p_report)
    {
	如果 (n > 0)
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	否则
	    vim_snprintf(msg_buf, MSG_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	如果 (got_int)
	    vim_strcat((char_u *)msg_buf, (char_u *)_(" (Interrupted)"),
								  MSG_BUF_LEN);
	如果 (msg(msg_buf))
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
    如果 (emsg_silent == 0)
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

    如果 (emsg_silent != 0 || in_assert_fails)
	返回;

    如果 (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	static 整数		did_init = FALSE;
	static elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	如果 (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    如果 (p_vb
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
		如果 (has_vtp_working() != 0
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
	    否则
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    如果 (vim_strchr(p_debug, 'e') != NULL)
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
#否则
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    如果 (var == NULL || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	如果 (homepath == NULL || *homepath == NUL)
	    homepath = (char_u *)"\\";
	如果 (homedrive != NULL
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((char *)NameBuff, "%s%s", homedrive, homepath);
	    如果 (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    如果 (var == NULL)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    如果 (var != NULL && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	如果 (p != NULL)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    如果 (exp != NULL && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((char *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    如果 (var != NULL && *var == NUL)	// empty is same as not set
	var = NULL;

    如果 (enc_utf8 && var != NULL)
    {
	整数	len;
	char_u  *pp = NULL;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (整数)STRLEN(var), &pp, &len);
	如果 (pp != NULL)
	{
	    homedir = pp;
	    返回;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    如果 (var == NULL)
	var = (char_u *)"C:/";
#endif

    如果 (var != NULL)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	如果 (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((char *)NameBuff) == 0)
	{
	    如果 (!mch_chdir((char *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    如果 (mch_chdir((char *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#如果 defined(EXITFREE)
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

#如果 defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    void
init_vimdir(void)
{
    整数	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    如果 (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    如果 (mustfree)
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
    返回 expand_env_save_opt(src, FALSE, NULL);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, 整数 one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    如果 (p != NULL)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, NULL);
    返回 p;
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
    整数		dstlen)		// maximum length of the result
{
    返回 expand_env_esc(src, dst, dstlen, NULL, FALSE, NULL);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    整数		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    整数		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    整数		c;
    char_u	*var;
    整数		copy_char;
    整数		mustfree;	// var was allocated, need to free it later
    整数		at_start = TRUE; // at start of a name
    整数		startstr_len = 0;
    char_u	*dst_start = dst;

    如果 (startstr != NULL)
	startstr_len = (整数)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    当 (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	如果 (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (void)skip_expr(&src, NULL);
	    如果 (*src == '`')
		++src;
	    len = src - var;
	    如果 (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (整数)len;
	    continue;
	}
#endif
	copy_char = TRUE;
	如果 ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#如果 defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    如果 (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		如果 (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    当 (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		否则
#endif
		{
		    当 (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#如果 defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#如果 defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		如果 (src[1] == '{' && *tail != '}')
# 否则
		如果 (*src == '%' && *tail != '%')
# endif
		    var = NULL;
		否则
		{
# ifdef UNIX
		    如果 (src[1] == '{')
# 否则
		    如果 (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#如果 defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    否则 如果 (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != NULL)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    否则					// user directory
	    {
#如果 defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		当 (	   c-- > 0
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
#  如果 defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    struct passwd *pw = (*dst == NUL)
					? NULL : getpwnam((char *)dst + 1);

		    var = (pw == NULL) ? NULL : (char_u *)pw->pw_dir;
		}
		如果 (var == NULL)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, NULL,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# 否则	// !UNIX, thus VMS
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
		    当 (*next_path)
		    {
			为 (path = next_path; *next_path && *next_path != ',';
				next_path++);
			如果 (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (char *)test,
			    sizeof(test),
			    "%s/%s",
			    path,
			    dst + 1);
			如果 (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    如果 (var != NULL)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    break;
			}
		    }
		}
# endif // UNIX
#否则
		// cannot expand user's home directory, so don't try
		var = NULL;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    如果 (p_ssl && var != NULL && vim_strchr(var, '\\') != NULL)
	    {
		char_u	*p = vim_strsave(var);

		如果 (p != NULL)
		{
		    如果 (mustfree)
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
	    如果 (esc_chars != NULL && var != NULL
		    && vim_strpbrk(var, esc_chars) != NULL)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		如果 (p != NULL)
		{
		    如果 (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    如果 (var != NULL && *var != NUL)
	    {
		c = (整数)STRLEN(var);

		如果 (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    如果 (after_pathsep(dst, dst + c)
#如果 defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    如果 (mustfree)
		vim_free(var);
	}

	如果 (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    如果 (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    否则 如果 ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    如果 (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		如果 (startstr != NULL && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    返回 (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    static char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    整数		len = (整数)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    如果 (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	返回 newend;
    返回 pend;
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

    如果 (vimdir == NULL || *vimdir == NUL)
	返回 NULL;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    如果 (p.string != NULL && mch_isdir(p.string))
	返回 p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    如果 (p.string != NULL && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	如果 (fname.string != NULL)
	{
	    整数 exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    如果 (exists)
		返回 p.string;
	}
    }
    vim_free(p.string);
    返回 NULL;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, 整数 *mustfree)
{
    char_u	*p = NULL;
    char_u	*pend;
    整数		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    如果 (STRCMP(name, "HOME") == 0)
	返回 homedir;

    // Use Wide function
    wn = enc_to_utf16(name, NULL);
    如果 (wn == NULL)
	返回 NULL;

    wp = _wgetenv(wn);
    vim_free(wn);

    如果 (wp != NULL && *wp == NUL)   // empty is the same as not set
	wp = NULL;

    如果 (wp != NULL)
    {
	p = utf16_to_enc(wp, NULL);
	如果 (p == NULL)
	    返回 NULL;

	*mustfree = TRUE;
	返回 p;
    }
#否则
    p = mch_getenv(name);
    如果 (p != NULL && *p == NUL)	    // empty is the same as not set
	p = NULL;

    如果 (p != NULL)
	返回 p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    如果 (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	static char userSettingsPath[MAXPATHL];

	如果 (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    返回 (char_u *)userSettingsPath;
	否则
	    返回 NULL;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    如果 (!vimruntime && STRCMP(name, "VIM") != 0)
	返回 NULL;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    如果 (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	如果 (wp != NULL && *wp == NUL)	    // empty is the same as not set
	    wp = NULL;
	如果 (wp != NULL)
	{
	    char_u *q = utf16_to_enc(wp, NULL);
	    如果 (q != NULL)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		如果 (p == NULL)
		    p = q;
	    }
	}
#否则
	p = mch_getenv((char_u *)"VIM");
	如果 (p != NULL && *p == NUL)	    // empty is the same as not set
	    p = NULL;
	如果 (p != NULL)
	{
	    p = vim_version_dir(p);
	    如果 (p != NULL)
		*mustfree = TRUE;
	    否则
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    如果 (p == NULL)
    {
	如果 (p_hf != NULL && vim_strchr(p_hf, '$') == NULL)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	否则
	    p = exe_name;
#endif
	如果 (p != NULL)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    如果 (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    如果 (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		如果 (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    如果 (pnew != NULL)
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
	    如果 (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    如果 (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    如果 (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    如果 (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    如果 (p != NULL && !mch_isdir(p))
		VIM_CLEAR(p);
	    否则
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		如果 (vimruntime && (pend = vim_version_dir(p)) != NULL)
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
    如果 (p == NULL)
    {
	// Only use default_vimruntime_dir when it is not empty
	如果 (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	否则 如果 (*default_vim_dir != NUL)
	{
	    如果 (vimruntime && (p = vim_version_dir(default_vim_dir)) != NULL)
		*mustfree = TRUE;
	    否则
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
    如果 (p != NULL)
    {
	如果 (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	否则
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    返回 p;
}

    void
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((char *)var);
#否则
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
    如果 (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    否则 如果 (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#如果 defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    void
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    如果 (STRICMP(name, "HOME") == 0)
	init_homedir();
    否则 如果 (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    否则 如果 (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
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
    mch_setenv((char *)name, (char *)val, 1);
#否则
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    如果 (envbuf != NULL)
    {
	sprintf((char *)envbuf, "%s=%s", name, val);
	putenv((char *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    如果 (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	如果 (buf != NULL)
	{
	    bindtextdomain(VIMPACKAGE, (char *)buf);
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
    整数		idx)
{
#如果 defined(AMIGA)
    // No environ[] on the Amiga.
    返回 NULL;
#否则
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    extern char		**environ;
# endif
    char_u		*str;
    整数			n;

    str = (char_u *)environ[idx];
    如果 (str == NULL)
	返回 NULL;

    为 (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	如果 (str[n] == '=' || str[n] == NUL)
	    break;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    返回 xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    static void
add_user(char_u *user, 整数 need_copy)
{
    char_u	*user_copy = (user != NULL && need_copy)
						    ? vim_strsave(user) : user;

    如果 (user_copy == NULL || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	如果 (need_copy)
	    vim_free(user_copy);
	返回;
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
    static 整数	lazy_init_done = FALSE;

    如果 (lazy_init_done)
	返回;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, sizeof(char_u *), 20);

#如果 defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	struct passwd*	pw;

	setpwent();
	当 ((pw = getpwent()) != NULL)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#elif defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	如果 (NetUserEnum(NULL, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, NULL) == NERR_Success)
	{
	    为 (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, NULL), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#如果 defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	如果 (user_env != NULL && *user_env != NUL)
	{
	    整数	i;

	    为 (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		如果 (STRCMP(local_user, user_env) == 0)
		    break;
	    }

	    如果 (i == ga_users.ga_len)
	    {
		struct passwd	*pw = getpwnam((char *)user_env);

		如果 (pw != NULL)
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
get_users(expand_T *xp UNUSED, 整数 idx)
{
    init_users();
    如果 (idx < ga_users.ga_len)
	返回 ((char_u **)ga_users.ga_data)[idx];
    返回 NULL;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    整数
match_user(char_u *name)
{
    整数 i;
    整数 n = (整数)STRLEN(name);
    整数 result = 0;

    init_users();
    为 (i = 0; i < ga_users.ga_len; i++)
    {
	如果 (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    返回 2; // full match
	如果 (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    返回 result;
}

    static void
prepare_to_exit(void)
{
#如果 defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    如果 (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    否则
#endif
    {
	windgoto((整数)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	如果 (!full_screen)
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
	如果 (buf->b_ml.ml_mfp != NULL && buf->b_ml.ml_mfp->mf_fname != NULL)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    break;
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

static 整数	breakcheck_count = 0;

    void
line_breakcheck(void)
{
    如果 (++breakcheck_count >= BREAKCHECK_SKIP)
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
    如果 (++breakcheck_count >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}

#如果 defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    void
veryfast_breakcheck(void)
{
    如果 (++breakcheck_count >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_count = 0;
	ui_breakcheck();
    }
}
#endif

#如果 defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    整数		flags,		// can be SHELL_SILENT
    整数		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = NULL;
    整数		len;
    整数		i = 0;
    FILE	*fd;

    如果 (check_restricted() || check_secure())
	返回 NULL;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    如果 ((tempname = vim_tempname('o', TRUE)) == NULL)
    {
	emsg(_(e_cant_get_temp_file_name));
	返回 NULL;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    如果 (command == NULL)
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
    fd = mch_fopen((char *)tempname, "r");
# 否则
    fd = mch_fopen((char *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    如果 (fd == NULL
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	如果 (fd != NULL)
	    fclose(fd);
	goto done;
    }

    buffer = alloc(len + 1);
    如果 (buffer != NULL)
	i = (整数)fread((char *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    如果 (buffer == NULL)
	goto done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    如果 (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    否则 如果 (ret_len == NULL)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	为 (i = 0; i < len; ++i)
	    如果 (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    否则
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    返回 buffer;
}

# 如果 defined(FEAT_EVAL)

    static void
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    整数		retlist)
{
    char_u	*res = NULL;
    char_u	*p;
    char_u	*infile = NULL;
    整数		err = FALSE;
    FILE	*fd;
    list_T	*list = NULL;
    整数		flags = SHELL_SILENT;
    整数		use_argv = FALSE;
    char	**argv = NULL;
    整数		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    如果 (check_restricted() || check_secure())
	goto errret;

    如果 (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_number_or_list_arg(argvars, 1)
								      == FAIL))
	返回;

    如果 (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	如果 ((infile = vim_tempname('i', TRUE)) == NULL)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    goto errret;
	}

	fd = mch_fopen((char *)infile, WRITEBIN);
	如果 (fd == NULL)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    goto errret;
	}
	如果 (argvars[1].v_type == VAR_NUMBER)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_number);
	    如果 (buf == NULL)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_number);
		fclose(fd);
		goto errret;
	    }

	    为 (lnum = 1; lnum <= buf->b_ml.ml_line_count; lnum++)
	    {
		为 (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    如果 (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			break;
		    }
		如果 (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    break;
		}
	    }
	}
	否则 如果 (argvars[1].v_type == VAR_LIST)
	{
	    如果 (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	否则
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    如果 (p == NULL)
	    {
		fclose(fd);
		goto errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    如果 (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	如果 (fclose(fd) != 0)
	    err = TRUE;
	如果 (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    goto errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    如果 (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	如果 (l == NULL || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    goto errret;
	}
	如果 (build_argv_from_list(l, &argv, &argc) == FAIL)
	    goto errret;
	如果 (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    goto errret;
	}
	use_argv = TRUE;

	如果 (p_verbose > 3)
	{
	    整数		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    为 (i = 0; i < argc; ++i)
	    {
		如果 (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (char *)ga.ga_data);
	    msg_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    如果 (!msg_silent)
	flags += SHELL_COOKED;

    如果 (retlist)
    {
	整数		len;
	listitem_T	*li;
	char_u		*s = NULL;
	char_u		*start;
	char_u		*end;
	整数		i;

	如果 (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	否则
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	如果 (res == NULL)
	    goto errret;

	list = list_alloc();
	如果 (list == NULL)
	    goto errret;

	为 (i = 0; i < len; ++i)
	{
	    start = res + i;
	    当 (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    如果 (s == NULL)
		goto errret;

	    为 (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    如果 (li == NULL)
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
    否则
    {
	如果 (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, NULL);
	否则
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, NULL);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	如果 (res != NULL)
	{
	    char_u	*s, *d;

	    d = res;
	    为 (s = res; *s; ++s)
	    {
		如果 (s[0] == CAR && s[1] == NL)
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
    如果 (argv != NULL)
    {
	整数 i;
	为 (i = 0; argv[i] != NULL; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    如果 (infile != NULL)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    如果 (res != NULL)
	vim_free(res);
    如果 (list != NULL)
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
    整数
goto_im(void)
{
    返回 (p_im && stuff_empty() && typebuf_typed());
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
#否则
    p = skiptowhite(p_sh);
    如果 (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    否则
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	为 (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    如果 (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    返回 p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    整数
path_is_url(char_u *p)
{
    如果 (STRNCMP(p, "://", (size_t)3) == 0)
	返回 URL_SLASH;
    否则 如果 (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	返回 URL_BACKSLASH;
    返回 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    整数
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    如果 (!ASCII_ISALPHA(*fname))
	返回 0;

    // check body: alpha or dash
    为 (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    如果 (p[-1] == '-')
	返回 0;

    // "://" or ":\\" must follow
    返回 path_is_url(p);
}

#如果 defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    如果 (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    否则
	sve->sve_did_save = FALSE;
    返回 v_event;
}

    void
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    如果 (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    否则
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
    如果 (!has_modechanged() || got_int)
	返回;

    get_mode(curr_mode);
    如果 (STRCMP(curr_mode, last_mode) == 0)
	返回;

    v_event = get_v_event(&save_v_event);
    (void)dict_add_string(v_event, "new_mode", curr_mode);
    (void)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_items_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((char *)pattern_buf, sizeof(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, NULL, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    整数
vim_append_digit_long(long *value, 整数 digit)
{
    long x = *value;
    如果 (x > ((LONG_MAX - (long)digit) / 10))
	返回 FAIL;
    *value = x * 10 + (long)digit;
    返回 OK;
}

// Return something that fits into an int.
    整数
trim_to_int(vimlong_T x)
{
    返回 x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
