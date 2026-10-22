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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#जर defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#जर defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
स्थिर garray_T	ga_users;

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
    पूर्णांक
get_leader_len(
    char_u	*ओळ,
    char_u	**flags,
    पूर्णांक		backward,
    पूर्णांक		include_space)
{
    पूर्णांक		i, j;
    पूर्णांक		result;
    पूर्णांक		got_com = FALSE;
    पूर्णांक		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    पूर्णांक		मधला_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = रिक्त;

    result = i = 0;
    पर्यंत (VIM_ISWHITE(ओळ[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    पर्यंत (ओळ[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	साठी (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    जर (!got_com && flags != रिक्त)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (शून्य)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    जर (string == रिक्त)	    // missing ':', ignore this part
		सुरूठेवा;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    जर (मधला_match_len != 0
		    && vim_strchr(part_buf, COM_मधला) == रिक्त
		    && vim_strchr(part_buf, COM_END) == रिक्त)
		थांबा;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    जर (got_com && vim_strchr(part_buf, COM_NEST) == रिक्त)
		सुरूठेवा;

	    // When 'O' flag present and using "O" command skip this one.
	    जर (backward && vim_strchr(part_buf, COM_NOBACK) != रिक्त)
		सुरूठेवा;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    जर (VIM_ISWHITE(string[0]))
	    {
		जर (i == 0 || !VIM_ISWHITE(ओळ[i - 1]))
		    सुरूठेवा;  // missing white space
		पर्यंत (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    साठी (j = 0; string[j] != NUL && string[j] == ओळ[i + j]; ++j)
		;
	    जर (string[j] != NUL)
		सुरूठेवा;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    जर (vim_strchr(part_buf, COM_BLANK) != रिक्त
			   && !VIM_ISWHITE(ओळ[i + j]) && ओळ[i + j] != NUL)
		सुरूठेवा;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    जर (vim_strchr(part_buf, COM_मधला) != रिक्त)
	    {
		जर (मधला_match_len == 0)
		{
		    मधला_match_len = j;
		    saved_flags = prev_list;
		}
		सुरूठेवा;
	    }
	    जर (मधला_match_len != 0 && j > मधला_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		मधला_match_len = 0;

	    जर (मधला_match_len == 0)
		i += j;
	    found_one = TRUE;
	    थांबा;
	}

	जर (मधला_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    जर (!got_com && flags != रिक्त)
		*flags = saved_flags;
	    i += मधला_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	जर (!found_one)
	    थांबा;

	result = i;

	// Include any trailing white space.
	पर्यंत (VIM_ISWHITE(ओळ[i]))
	    ++i;

	जर (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	जर (vim_strchr(part_buf, COM_NEST) == रिक्त)
	    थांबा;
    }
    परत result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    पूर्णांक
get_last_leader_offset(char_u *ओळ, char_u **flags)
{
    पूर्णांक		result = -1;
    पूर्णांक		i, j;
    पूर्णांक		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    पूर्णांक		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (पूर्णांक)STRLEN(ओळ);
    पर्यंत (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	साठी (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (शून्य)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    जर (string == रिक्त)	// If everything is fine, this cannot actually
				// happen.
		सुरूठेवा;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    जर (VIM_ISWHITE(string[0]))
	    {
		जर (i == 0 || !VIM_ISWHITE(ओळ[i - 1]))
		    सुरूठेवा;
		पर्यंत (VIM_ISWHITE(*string))
		    ++string;
	    }
	    साठी (j = 0; string[j] != NUL && string[j] == ओळ[i + j]; ++j)
		/* do nothing */;
	    जर (string[j] != NUL)
		सुरूठेवा;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    जर (vim_strchr(part_buf, COM_BLANK) != रिक्त
		    && !VIM_ISWHITE(ओळ[i + j]) && ओळ[i + j] != NUL)
		सुरूठेवा;

	    जर (vim_strchr(part_buf, COM_मधला) != रिक्त)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		साठी (j = 0; VIM_ISWHITE(ओळ[j]) && j <= i; j++)
		    ;
		जर (j < i)
		    सुरूठेवा;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    जर (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    थांबा;
	}

	जर (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    पूर्णांक     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    जर (vim_strchr(part_buf, COM_NEST) != रिक्त)
		सुरूठेवा;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    पर्यंत (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (पूर्णांक)STRLEN(com_leader);

	    साठी (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(शून्य)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		जर (flags_save == com_flags)
		    सुरूठेवा;
		string = vim_strchr(part_buf2, ':');
		++string;
		पर्यंत (VIM_ISWHITE(*string))
		    ++string;
		len2 = (पूर्णांक)STRLEN(string);
		जर (len2 == 0)
		    सुरूठेवा;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		साठी (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    जर (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			जर (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    परत result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    पूर्णांक
plines(linenr_T lnum)
{
    परत plines_win(curwin, lnum, TRUE);
}

    पूर्णांक
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    पूर्णांक		limit_winheight)	// when TRUE limit to window height
{
#जर defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    परत plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    पूर्णांक
plines_nofill(linenr_T lnum)
{
    परत plines_win_nofill(curwin, lnum, TRUE);
}

    पूर्णांक
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    पूर्णांक		limit_winheight)	// when TRUE limit to window height
{
#endif
    पूर्णांक		ओळी;

    जर (wp->w_width == 0)
	परत 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    जर (ओळ_Folded(wp, lnum) == TRUE)
	परत 1;
#endif

    जर (!wp->w_p_wrap)
	ओळी = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_मोजणी_above_below(wp->w_buffer, lnum)
#endif
	;
    अन्यथा
	ओळी = plines_win_nofold(wp, lnum);

    जर (limit_winheight && ओळी > wp->w_height)
	परत wp->w_height;
    परत ओळी;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    पूर्णांक
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    दीर्घ	col;
    पूर्णांक		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    जर (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	परत 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (पूर्णांक)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    जर (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    जर (width <= 0)
	परत 32000;
    जर (col <= width)
	परत 1;
    col -= width;
    width += win_col_off2(wp);
    परत (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    पूर्णांक
plines_win_col(win_T *wp, linenr_T lnum, दीर्घ column)
{
    दीर्घ	col;
    पूर्णांक		ओळी = 0;
    पूर्णांक		width;
    char_u	*ओळ;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ओळी = diff_check_fill(wp, lnum);
#endif

    जर (!wp->w_p_wrap)
	परत ओळी + 1;

    जर (wp->w_width == 0)
	परत ओळी + 1;

    ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ओळ, ओळ);
    // "column" is a byte index, advance the pointer until it is reached.
    पर्यंत (*cts.cts_ptr != NUL && cts.cts_ptr < ओळ + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, रिक्त, रिक्त);
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
    जर (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, रिक्त, रिक्त) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    जर (width <= 0)
	परत 9999;

    ओळी += 1;
    जर (col > width)
	ओळी += (col - width) / (width + win_col_off2(wp)) + 1;
    परत ओळी;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    पूर्णांक
plines_m_win(win_T *wp, linenr_T first, linenr_T last, पूर्णांक max)
{
    पूर्णांक		मोजणी = 0;

    पर्यंत (first <= last && मोजणी < max)
    {
#ifdef FEAT_FOLDING
	पूर्णांक	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_मोजणी(wp, first, रिक्त);
	जर (x > 0)
	{
	    ++मोजणी;	    // count 1 for "+-- folded" line
	    first += x;
	}
	अन्यथा
#endif
	{
#ifdef FEAT_DIFF
	    जर (first == wp->w_topline)
		मोजणी += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    अन्यथा
#endif
		मोजणी += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    जर (first == wp->w_buffer->b_ml.ml_ओळ_मोजणी + 1)
	मोजणी += diff_check_fill(wp, first);
#endif
    परत MIN(max, मोजणी);
}

    पूर्णांक
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    पूर्णांक		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    जर (pos->col == MAXCOL)
	परत NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    जर (pos->col > ptrlen)
	परत NUL;
    जर (has_mbyte)
	परत (*mb_ptr2char)(ptr);
    परत (पूर्णांक)*ptr;
}

    पूर्णांक
gchar_cursor(शून्य)
{
    जर (has_mbyte)
	परत (*mb_ptr2char)(ml_get_cursor());
    परत (पूर्णांक)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    पूर्णांक
char_before_cursor(शून्य)
{
    जर (curwin->w_cursor.col == 0)
	परत -1;

    char_u *ओळ = ml_get_curline();

    जर (has_mbyte)
    {
	char_u *p = ओळ + curwin->w_cursor.col;
	पूर्णांक prev_len = (*mb_head_off)(ओळ, p - 1) + 1;
	परत mb_ptr2char(p - prev_len);
    }
    परत ओळ[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    शून्य
pchar_cursor(पूर्णांक c)
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
    जर (*p == ',')
	++p;
    पर्यंत (*p == ' ')
	++p;
    परत p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    शून्य
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	जर (wp->w_buffer == buf && wp->w_status_height)
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
    पूर्णांक
ask_yesno(char_u *str, पूर्णांक direct)
{
    पूर्णांक	    r = ' ';
    पूर्णांक	    save_State = State;

    जर (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_किल्ल्या;		// no mapping here, but recognize keys

    पर्यंत (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	जर (direct)
	    r = get_keystroke();
	अन्यथा
	    r = plain_vgetc();
	जर (r == Ctrl_C || r == ESC)
	    r = 'n';
	संदेश_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_किल्ल्या;

    परत r;
}

#जर defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    शून्य
get_mode(char_u *buf)
{
    पूर्णांक		i = 0;

    जर (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    अन्यथा जर (term_use_loop())
    {
	जर (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    अन्यथा जर (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	जर (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	अन्यथा जर (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    अन्यथा जर (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    अन्यथा जर (State & MODE_घाला)
    {
	जर (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	अन्यथा
	{
	    जर (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    अन्यथा
		buf[i++] = 'i';
	}

	जर (ins_compl_active())
	    buf[i++] = 'c';
	अन्यथा जर (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    अन्यथा जर ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	जर (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	अन्यथा जर (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	जर ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    अन्यथा जर (VIsual_active)
    {
	जर (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	अन्यथा
	{
	    buf[i++] = VIsual_mode;
	    जर (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    अन्यथा
    {
	buf[i++] = 'n';
	जर (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	अन्यथा जर (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	अन्यथा जर (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    शून्य
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    जर (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	परत;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    जर (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    स्थिर शून्य
may_add_state_char(garray_T *gap, char_u *include, पूर्णांक c)
{
    जर (include == रिक्त || vim_strchr(include, c) != रिक्त)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    शून्य
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = रिक्त;
    पूर्णांक		i;

    जर (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	परत;

    ga_init2(&ga, 1, 20);
    जर (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    जर (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == रिक्त))
	may_add_state_char(&ga, include, 'm');
    जर (op_pending())
	may_add_state_char(&ga, include, 'o');
    जर (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    जर (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    जर (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    जर (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    साठी (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    जर (संदेश_scrolled > 0)
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
    पूर्णांक
get_keystroke(शून्य)
{
    char_u	*buf = रिक्त;
    पूर्णांक		buflen = 150;
    पूर्णांक		maxlen;
    पूर्णांक		len = 0;
    पूर्णांक		n;
    पूर्णांक		save_mapped_ctrl_c = mapped_ctrl_c;
    पूर्णांक		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    साठी (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	जर (buf == रिक्त)
	    buf = alloc(buflen);
	अन्यथा जर (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    जर (buf == रिक्त)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	जर (buf == रिक्त)
	{
	    do_outofmem_संदेश((long_u)buflen);
	    परत ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	जर (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_आदान_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	अन्यथा जर (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	जर ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    सुरूठेवा;

	जर (n == KEYLEN_REMOVED)  // key code removed
	{
	    जर (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    सुरूठेवा;
	}
	जर (n > 0)		// found a termcode: adjust length
	    len = n;
	जर (len == 0)		// nothing typed yet
	    सुरूठेवा;

	// Handle modifier and/or special key code.
	n = buf[0];
	जर (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    जर (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_किल्ली(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		जर (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		जर (len > 0)
		    mch_memmove(buf, buf + 3, (माप_प्रकार)len);
		सुरूठेवा;
	    }
	    थांबा;
	}
	जर (has_mbyte)
	{
	    जर (MB_BYTE2LEN(n) > len)
		सुरूठेवा;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	जर (n == intr_char)
	    n = ESC;
#endif
	थांबा;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    परत merge_modify_Other_किल्ल्या(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    स्थिर पूर्णांक
vim_append_digit_int(पूर्णांक *मूल्य, पूर्णांक digit)
{
    पूर्णांक x = *मूल्य;
    जर (x > ((पूर्णांक_उच्च - digit) / 10))
	परत FAIL;
    *मूल्य = x * 10 + digit;
    परत OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    पूर्णांक
get_संख्या(
    पूर्णांक	    colon,			// allow colon to abort
    पूर्णांक	    *mouse_used)
{
    पूर्णांक	n = 0;
    पूर्णांक	c;
    पूर्णांक typed = 0;

    जर (mouse_used != रिक्त)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    जर (संदेश_silent != 0)
	परत 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_किल्ल्या;		// no mapping here, but recognize keys
    साठी (;;)
    {
	windgoto(संदेश_row, cmdline_col_off + संदेश_col);
	c = safe_vgetc();
	जर (VIM_ISDIGIT(c))
	{
	    जर (vim_append_digit_int(&n, c - '0') == FAIL)
		परत 0;
	    संदेश_putchar(c);
	    ++typed;
	}
	अन्यथा जर (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    जर (typed > 0)
	    {
		संदेश_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	अन्यथा जर (mouse_used != रिक्त && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    थांबा;
	}
	अन्यथा जर (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    जर (!exmode_active)
		cmdline_row = संदेश_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    थांबा;
	}
	अन्यथा जर (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    थांबा;
	}
	अन्यथा जर (c == CAR || c == NL )
	    थांबा;
    }
    --no_mapping;
    --allow_किल्ल्या;
    परत n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    पूर्णांक
prompt_for_संख्या(पूर्णांक *mouse_used)
{
    पूर्णांक		i;
    पूर्णांक		save_cmdline_row;
    पूर्णांक		save_State;

    // When using ":silent" assume that <CR> was entered.
    जर (mouse_used != रिक्त)
	संदेश_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    अन्यथा
	संदेश_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_संख्या(TRUE, mouse_used);
    जर (किल्ली_Typed)
    {
	// don't call wait_return() now
	जर (संदेश_row > 0)
	    cmdline_row = संदेश_row - 1;
	need_wait_return = FALSE;
	संदेश_didany = FALSE;
	संदेश_didout = FALSE;
    }
    अन्यथा
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    परत i;
}

    शून्य
msgmore(दीर्घ n)
{
    दीर्घ pn;

    जर (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	परत;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    जर (keep_संदेश != रिक्त && !keep_संदेश_more)
	परत;

    जर (n > 0)
	pn = n;
    अन्यथा
	pn = -n;

    जर (pn > p_report)
    {
	जर (n > 0)
	    vim_snprintf(संदेश_buf, संदेश_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	अन्यथा
	    vim_snprintf(संदेश_buf, संदेश_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	जर (got_int)
	    vim_strcat((char_u *)संदेश_buf, (char_u *)_(" (Interrupted)"),
								  संदेश_BUF_LEN);
	जर (संदेश(संदेश_buf))
	{
	    set_keep_संदेश((char_u *)संदेश_buf, 0);
	    keep_संदेश_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    शून्य
beep_flush(शून्य)
{
    जर (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    शून्य
vim_beep(अचिन्हित val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    जर (emsg_silent != 0 || in_assert_fails)
	परत;

    जर (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	स्थिर पूर्णांक		did_init = FALSE;
	स्थिर elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	जर (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    जर (p_vb
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
		जर (has_vtp_working() != 0
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
	    अन्यथा
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    जर (vim_strchr(p_debug, 'e') != रिक्त)
    {
	संदेश_source(HL_ATTR(HLF_W));
	संदेश_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    शून्य
init_homedir(शून्य)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#अन्यथा
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    जर (var == रिक्त || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	जर (homepath == रिक्त || *homepath == NUL)
	    homepath = (char_u *)"\\";
	जर (homedrive != रिक्त
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    प्र_लिखो((अक्षर *)NameBuff, "%s%s", homedrive, homepath);
	    जर (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    जर (var == रिक्त)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    जर (var != रिक्त && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	जर (p != रिक्त)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    जर (exp != रिक्त && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((अक्षर *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    जर (var != रिक्त && *var == NUL)	// empty is same as not set
	var = रिक्त;

    जर (enc_utf8 && var != रिक्त)
    {
	पूर्णांक	len;
	char_u  *pp = रिक्त;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (पूर्णांक)STRLEN(var), &pp, &len);
	जर (pp != रिक्त)
	{
	    homedir = pp;
	    परत;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    जर (var == रिक्त)
	var = (char_u *)"C:/";
#endif

    जर (var != रिक्त)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	जर (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((अक्षर *)NameBuff) == 0)
	{
	    जर (!mch_chdir((अक्षर *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    जर (mch_chdir((अक्षर *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#जर defined(EXITFREE)
    शून्य
free_homedir(शून्य)
{
    vim_free(homedir);
}

    शून्य
free_users(शून्य)
{
    ga_clear_strings(&ga_users);
}
#endif

#जर defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    शून्य
init_vimdir(शून्य)
{
    पूर्णांक	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    जर (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    जर (mustfree)
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
    परत expand_env_save_opt(src, FALSE, रिक्त);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, पूर्णांक one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    जर (p != रिक्त)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, रिक्त);
    परत p;
}

/*
 * Expand environment variable with path name.
 * "~/" is also expanded, using $HOME.	For Unix "~user/" is expanded.
 * Skips over "\ ", "\~" and "\$" (not for Win32 though).
 * If anything fails no expansion is done and dst equals src.
 */
    माप_प्रकार
expand_env(
    char_u	*src,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    पूर्णांक		dstlen)		// maximum length of the result
{
    परत expand_env_esc(src, dst, dstlen, रिक्त, FALSE, रिक्त);
}

    माप_प्रकार
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    पूर्णांक		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    पूर्णांक		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    पूर्णांक		c;
    char_u	*var;
    पूर्णांक		copy_char;
    पूर्णांक		mustfree;	// var was allocated, need to free it later
    पूर्णांक		at_start = TRUE; // at start of a name
    पूर्णांक		startstr_len = 0;
    char_u	*dst_start = dst;

    जर (startstr != रिक्त)
	startstr_len = (पूर्णांक)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    पर्यंत (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	जर (src[0] == '`' && src[1] == '=')
	{
	    माप_प्रकार len;

	    var = src;
	    src += 2;
	    (शून्य)skip_अभिव्यक्ती(&src, रिक्त);
	    जर (*src == '`')
		++src;
	    len = src - var;
	    जर (len > (माप_प्रकार)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (पूर्णांक)len;
	    सुरूठेवा;
	}
#endif
	copy_char = TRUE;
	जर ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#जर defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    जर (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		जर (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    पर्यंत (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		अन्यथा
#endif
		{
		    पर्यंत (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#जर defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#जर defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		जर (src[1] == '{' && *tail != '}')
# अन्यथा
		जर (*src == '%' && *tail != '%')
# endif
		    var = रिक्त;
		अन्यथा
		{
# ifdef UNIX
		    जर (src[1] == '{')
# अन्यथा
		    जर (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#जर defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    अन्यथा जर (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != रिक्त)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    अन्यथा					// user directory
	    {
#जर defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		पर्यंत (	   c-- > 0
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
#  जर defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    संरचना passwd *pw = (*dst == NUL)
					? रिक्त : getpwnam((अक्षर *)dst + 1);

		    var = (pw == रिक्त) ? रिक्त : (char_u *)pw->pw_dir;
		}
		जर (var == रिक्त)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, रिक्त,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# अन्यथा	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    माप_प्रकार	testlen;
		    char_u	*path, *next_path, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    next_path = paths;
		    पर्यंत (*next_path)
		    {
			साठी (path = next_path; *next_path && *next_path != ',';
				next_path++);
			जर (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (अक्षर *)test,
			    आकार(test),
			    "%s/%s",
			    path,
			    dst + 1);
			जर (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    जर (var != रिक्त)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    थांबा;
			}
		    }
		}
# endif // UNIX
#अन्यथा
		// cannot expand user's home directory, so don't try
		var = रिक्त;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    जर (p_ssl && var != रिक्त && vim_strchr(var, '\\') != रिक्त)
	    {
		char_u	*p = vim_strsave(var);

		जर (p != रिक्त)
		{
		    जर (mustfree)
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
	    जर (esc_chars != रिक्त && var != रिक्त
		    && vim_strpbrk(var, esc_chars) != रिक्त)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		जर (p != रिक्त)
		{
		    जर (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    जर (var != रिक्त && *var != NUL)
	    {
		c = (पूर्णांक)STRLEN(var);

		जर (c + STRLEN(tail) + 1 < (अचिन्हित)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    जर (after_pathsep(dst, dst + c)
#जर defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    जर (mustfree)
		vim_free(var);
	}

	जर (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    जर (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    अन्यथा जर ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    जर (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		जर (startstr != रिक्त && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    परत (माप_प्रकार)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    स्थिर char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    पूर्णांक		len = (पूर्णांक)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    जर (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	परत newend;
    परत pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    स्थिर char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    माप_प्रकार	vimdir_len;

    जर (vimdir == रिक्त || *vimdir == NUL)
	परत रिक्त;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    जर (p.string != रिक्त && mch_isdir(p.string))
	परत p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    जर (p.string != रिक्त && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	जर (fname.string != रिक्त)
	{
	    पूर्णांक exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    जर (exists)
		परत p.string;
	}
    }
    vim_free(p.string);
    परत रिक्त;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, पूर्णांक *mustfree)
{
    char_u	*p = रिक्त;
    char_u	*pend;
    पूर्णांक		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    जर (STRCMP(name, "HOME") == 0)
	परत homedir;

    // Use Wide function
    wn = enc_to_utf16(name, रिक्त);
    जर (wn == रिक्त)
	परत रिक्त;

    wp = _wgetenv(wn);
    vim_free(wn);

    जर (wp != रिक्त && *wp == NUL)   // empty is the same as not set
	wp = रिक्त;

    जर (wp != रिक्त)
    {
	p = utf16_to_enc(wp, रिक्त);
	जर (p == रिक्त)
	    परत रिक्त;

	*mustfree = TRUE;
	परत p;
    }
#अन्यथा
    p = mch_getenv(name);
    जर (p != रिक्त && *p == NUL)	    // empty is the same as not set
	p = रिक्त;

    जर (p != रिक्त)
	परत p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    जर (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	स्थिर अक्षर userSettingsPath[MAXPATHL];

	जर (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    परत (char_u *)userSettingsPath;
	अन्यथा
	    परत रिक्त;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    जर (!vimruntime && STRCMP(name, "VIM") != 0)
	परत रिक्त;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    जर (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	जर (wp != रिक्त && *wp == NUL)	    // empty is the same as not set
	    wp = रिक्त;
	जर (wp != रिक्त)
	{
	    char_u *q = utf16_to_enc(wp, रिक्त);
	    जर (q != रिक्त)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		जर (p == रिक्त)
		    p = q;
	    }
	}
#अन्यथा
	p = mch_getenv((char_u *)"VIM");
	जर (p != रिक्त && *p == NUL)	    // empty is the same as not set
	    p = रिक्त;
	जर (p != रिक्त)
	{
	    p = vim_version_dir(p);
	    जर (p != रिक्त)
		*mustfree = TRUE;
	    अन्यथा
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    जर (p == रिक्त)
    {
	जर (p_hf != रिक्त && vim_strchr(p_hf, '$') == रिक्त)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	अन्यथा
	    p = exe_name;
#endif
	जर (p != रिक्त)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    जर (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    जर (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		जर (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    जर (pnew != रिक्त)
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
	    जर (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    जर (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    जर (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    जर (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    जर (p != रिक्त && !mch_isdir(p))
		VIM_CLEAR(p);
	    अन्यथा
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		जर (vimruntime && (pend = vim_version_dir(p)) != रिक्त)
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
    जर (p == रिक्त)
    {
	// Only use default_vimruntime_dir when it is not empty
	जर (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	अन्यथा जर (*default_vim_dir != NUL)
	{
	    जर (vimruntime && (p = vim_version_dir(default_vim_dir)) != रिक्त)
		*mustfree = TRUE;
	    अन्यथा
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
    जर (p != रिक्त)
    {
	जर (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	अन्यथा
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    परत p;
}

    शून्य
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((अक्षर *)var);
#अन्यथा
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    शून्य
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    जर (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    अन्यथा जर (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#जर defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    शून्य
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    जर (STRICMP(name, "HOME") == 0)
	init_homedir();
    अन्यथा जर (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    अन्यथा जर (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    शून्य
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((अक्षर *)name, (अक्षर *)val, 1);
#अन्यथा
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    जर (envbuf != रिक्त)
    {
	प्र_लिखो((अक्षर *)envbuf, "%s=%s", name, val);
	putenv((अक्षर *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    जर (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	जर (buf != रिक्त)
	{
	    bindtextdomain(VIMPACKAGE, (अक्षर *)buf);
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
    पूर्णांक		idx)
{
#जर defined(AMIGA)
    // No environ[] on the Amiga.
    परत रिक्त;
#अन्यथा
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    बाह्य अक्षर		**environ;
# endif
    char_u		*str;
    पूर्णांक			n;

    str = (char_u *)environ[idx];
    जर (str == रिक्त)
	परत रिक्त;

    साठी (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	जर (str[n] == '=' || str[n] == NUL)
	    थांबा;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    परत xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    स्थिर शून्य
add_user(char_u *user, पूर्णांक need_copy)
{
    char_u	*user_copy = (user != रिक्त && need_copy)
						    ? vim_strsave(user) : user;

    जर (user_copy == रिक्त || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	जर (need_copy)
	    vim_free(user_copy);
	परत;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    स्थिर शून्य
init_users(शून्य)
{
    स्थिर पूर्णांक	lazy_init_done = FALSE;

    जर (lazy_init_done)
	परत;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, आकार(char_u *), 20);

#जर defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	संरचना passwd*	pw;

	setpwent();
	पर्यंत ((pw = getpwent()) != रिक्त)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#किंवाजर defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	जर (NetUserEnum(रिक्त, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, रिक्त) == NERR_Success)
	{
	    साठी (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, रिक्त), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#जर defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	जर (user_env != रिक्त && *user_env != NUL)
	{
	    पूर्णांक	i;

	    साठी (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		जर (STRCMP(local_user, user_env) == 0)
		    थांबा;
	    }

	    जर (i == ga_users.ga_len)
	    {
		संरचना passwd	*pw = getpwnam((अक्षर *)user_env);

		जर (pw != रिक्त)
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
get_users(expand_T *xp UNUSED, पूर्णांक idx)
{
    init_users();
    जर (idx < ga_users.ga_len)
	परत ((char_u **)ga_users.ga_data)[idx];
    परत रिक्त;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    पूर्णांक
match_user(char_u *name)
{
    पूर्णांक i;
    पूर्णांक n = (पूर्णांक)STRLEN(name);
    पूर्णांक result = 0;

    init_users();
    साठी (i = 0; i < ga_users.ga_len; i++)
    {
	जर (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    परत 2; // full match
	जर (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    परत result;
}

    स्थिर शून्य
prepare_to_exit(शून्य)
{
#जर defined(SIGHUP) && defined(संक_छोड़ो)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, संक_छोड़ो);
#endif

#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    अन्यथा
#endif
    {
	windgoto((पूर्णांक)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	जर (!full_screen)
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
    शून्य
preserve_exit(शून्य)
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
	जर (buf->b_ml.ml_mfp != रिक्त && buf->b_ml.ml_mfp->mf_fname != रिक्त)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    थांबा;
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

स्थिर पूर्णांक	breakcheck_मोजणी = 0;

    शून्य
ओळ_breakcheck(शून्य)
{
    जर (++breakcheck_मोजणी >= BREAKCHECK_SKIP)
    {
	breakcheck_मोजणी = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    शून्य
fast_breakcheck(शून्य)
{
    जर (++breakcheck_मोजणी >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_मोजणी = 0;
	ui_breakcheck();
    }
}

#जर defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    शून्य
veryfast_breakcheck(शून्य)
{
    जर (++breakcheck_मोजणी >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_मोजणी = 0;
	ui_breakcheck();
    }
}
#endif

#जर defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

# ifndef शुरू_से
#  define शुरू_से 0
# endif
# ifndef अंत_से
#  define अंत_से 2
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
    पूर्णांक		flags,		// can be SHELL_SILENT
    पूर्णांक		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = रिक्त;
    पूर्णांक		len;
    पूर्णांक		i = 0;
    खाता	*fd;

    जर (check_restricted() || check_secure())
	परत रिक्त;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    जर ((tempname = vim_tempname('o', TRUE)) == रिक्त)
    {
	emsg(_(e_cant_get_temp_file_name));
	परत रिक्त;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    जर (command == रिक्त)
	जा done;

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
    fd = mch_fopen((अक्षर *)tempname, "r");
# अन्यथा
    fd = mch_fopen((अक्षर *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    जर (fd == रिक्त
	    || ख_जाओ(fd, 0L, अंत_से) == -1
	    || (len = ख_बताओ(fd)) == -1		// get size of temp file
	    || ख_जाओ(fd, 0L, शुरू_से) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	जर (fd != रिक्त)
	    ख_बंद(fd);
	जा done;
    }

    buffer = alloc(len + 1);
    जर (buffer != रिक्त)
	i = (पूर्णांक)ख_पढ़ो((अक्षर *)buffer, (माप_प्रकार)1, (माप_प्रकार)len, fd);
    ख_बंद(fd);
    जर (buffer == रिक्त)
	जा done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    जर (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    अन्यथा जर (ret_len == रिक्त)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	साठी (i = 0; i < len; ++i)
	    जर (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    अन्यथा
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    परत buffer;
}

# जर defined(FEAT_EVAL)

    स्थिर शून्य
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    पूर्णांक		retlist)
{
    char_u	*res = रिक्त;
    char_u	*p;
    char_u	*infile = रिक्त;
    पूर्णांक		err = FALSE;
    खाता	*fd;
    list_T	*list = रिक्त;
    पूर्णांक		flags = SHELL_SILENT;
    पूर्णांक		use_argv = FALSE;
    अक्षर	**argv = रिक्त;
    पूर्णांक		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    जर (check_restricted() || check_secure())
	जा errret;

    जर (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_संख्या_or_list_arg(argvars, 1)
								      == FAIL))
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	जर ((infile = vim_tempname('i', TRUE)) == रिक्त)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    जा errret;
	}

	fd = mch_fopen((अक्षर *)infile, WRITEBIN);
	जर (fd == रिक्त)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    जा errret;
	}
	जर (argvars[1].v_type == VAR_संख्या)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_संख्या);
	    जर (buf == रिक्त)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_संख्या);
		ख_बंद(fd);
		जा errret;
	    }

	    साठी (lnum = 1; lnum <= buf->b_ml.ml_ओळ_मोजणी; lnum++)
	    {
		साठी (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    जर (अ_दो(*p == '\n' ? NUL : *p, fd) == खातापूर्ण)
		    {
			err = TRUE;
			थांबा;
		    }
		जर (अ_दो(NL, fd) == खातापूर्ण)
		{
		    err = TRUE;
		    थांबा;
		}
	    }
	}
	अन्यथा जर (argvars[1].v_type == VAR_LIST)
	{
	    जर (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	अन्यथा
	{
	    माप_प्रकार	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    जर (p == रिक्त)
	    {
		ख_बंद(fd);
		जा errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    जर (len > 0 && ख_डालो(p, len, 1, fd) != 1)
		err = TRUE;
	}
	जर (ख_बंद(fd) != 0)
	    err = TRUE;
	जर (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    जा errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    जर (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	जर (l == रिक्त || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    जा errret;
	}
	जर (build_argv_from_list(l, &argv, &argc) == FAIL)
	    जा errret;
	जर (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    जा errret;
	}
	use_argv = TRUE;

	जर (p_verbose > 3)
	{
	    पूर्णांक		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    साठी (i = 0; i < argc; ++i)
	    {
		जर (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (अक्षर *)ga.ga_data);
	    संदेश_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    जर (!संदेश_silent)
	flags += SHELL_COOKED;

    जर (retlist)
    {
	पूर्णांक		len;
	listitem_T	*li;
	char_u		*s = रिक्त;
	char_u		*start;
	char_u		*end;
	पूर्णांक		i;

	जर (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	अन्यथा
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	जर (res == रिक्त)
	    जा errret;

	list = list_alloc();
	जर (list == रिक्त)
	    जा errret;

	साठी (i = 0; i < len; ++i)
	{
	    start = res + i;
	    पर्यंत (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    जर (s == रिक्त)
		जा errret;

	    साठी (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    जर (li == रिक्त)
	    {
		vim_free(s);
		जा errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = रिक्त;
    }
    अन्यथा
    {
	जर (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, रिक्त);
	अन्यथा
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, रिक्त);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	जर (res != रिक्त)
	{
	    char_u	*s, *d;

	    d = res;
	    साठी (s = res; *s; ++s)
	    {
		जर (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = रिक्त;
    }

errret:
    जर (argv != रिक्त)
    {
	पूर्णांक i;
	साठी (i = 0; argv[i] != रिक्त; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    जर (infile != रिक्त)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    जर (res != रिक्त)
	vim_free(res);
    जर (list != रिक्त)
	list_free(list);
}

/*
 * "system()" function
 */
    शून्य
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    शून्य
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
    पूर्णांक
goto_im(शून्य)
{
    परत (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(शून्य)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#अन्यथा
    p = skiptowhite(p_sh);
    जर (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    अन्यथा
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	साठी (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    जर (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    परत p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    पूर्णांक
path_is_url(char_u *p)
{
    जर (STRNCMP(p, "://", (माप_प्रकार)3) == 0)
	परत URL_SLASH;
    अन्यथा जर (STRNCMP(p, ":\\\\", (माप_प्रकार)3) == 0)
	परत URL_BACKSLASH;
    परत 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    पूर्णांक
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    जर (!ASCII_ISALPHA(*fname))
	परत 0;

    // check body: alpha or dash
    साठी (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    जर (p[-1] == '-')
	परत 0;

    // "://" or ":\\" must follow
    परत path_is_url(p);
}

#जर defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    जर (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    अन्यथा
	sve->sve_did_save = FALSE;
    परत v_event;
}

    शून्य
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    जर (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    अन्यथा
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    शून्य
may_trigger_modechanged(शून्य)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    जर (!has_modechanged() || got_int)
	परत;

    get_mode(curr_mode);
    जर (STRCMP(curr_mode, last_mode) == 0)
	परत;

    v_event = get_v_event(&save_v_event);
    (शून्य)dict_add_string(v_event, "new_mode", curr_mode);
    (शून्य)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_वस्तुसंच_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((अक्षर *)pattern_buf, आकार(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, रिक्त, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    पूर्णांक
vim_append_digit_long(दीर्घ *मूल्य, पूर्णांक digit)
{
    दीर्घ x = *मूल्य;
    जर (x > ((दीर्घ_उच्च - (दीर्घ)digit) / 10))
	परत FAIL;
    *मूल्य = x * 10 + (दीर्घ)digit;
    परत OK;
}

// Return something that fits into an int.
    पूर्णांक
trim_to_int(vimlong_T x)
{
    परत x > पूर्णांक_उच्च ? पूर्णांक_उच्च : x < पूर्णांक_न्यून ? पूर्णांक_न्यून : x;
}
