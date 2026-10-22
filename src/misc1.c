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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#જો defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#જો defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
સ્થિર garray_T	ga_users;

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
    પૂર્ણાંક
get_leader_len(
    char_u	*લીટી,
    char_u	**flags,
    પૂર્ણાંક		backward,
    પૂર્ણાંક		include_space)
{
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		result;
    પૂર્ણાંક		got_com = FALSE;
    પૂર્ણાંક		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    પૂર્ણાંક		મધ્ય_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ખાલી;

    result = i = 0;
    જ્યારે (VIM_ISWHITE(લીટી[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    જ્યારે (લીટી[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	માટે (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    જો (!got_com && flags != ખાલી)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (શૂન્ય)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    જો (string == ખાલી)	    // missing ':', ignore this part
		ચાલુ;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    જો (મધ્ય_match_len != 0
		    && vim_strchr(part_buf, COM_મધ્ય) == ખાલી
		    && vim_strchr(part_buf, COM_END) == ખાલી)
		તોડો;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    જો (got_com && vim_strchr(part_buf, COM_NEST) == ખાલી)
		ચાલુ;

	    // When 'O' flag present and using "O" command skip this one.
	    જો (backward && vim_strchr(part_buf, COM_NOBACK) != ખાલી)
		ચાલુ;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    જો (VIM_ISWHITE(string[0]))
	    {
		જો (i == 0 || !VIM_ISWHITE(લીટી[i - 1]))
		    ચાલુ;  // missing white space
		જ્યારે (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    માટે (j = 0; string[j] != NUL && string[j] == લીટી[i + j]; ++j)
		;
	    જો (string[j] != NUL)
		ચાલુ;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    જો (vim_strchr(part_buf, COM_BLANK) != ખાલી
			   && !VIM_ISWHITE(લીટી[i + j]) && લીટી[i + j] != NUL)
		ચાલુ;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    જો (vim_strchr(part_buf, COM_મધ્ય) != ખાલી)
	    {
		જો (મધ્ય_match_len == 0)
		{
		    મધ્ય_match_len = j;
		    saved_flags = prev_list;
		}
		ચાલુ;
	    }
	    જો (મધ્ય_match_len != 0 && j > મધ્ય_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		મધ્ય_match_len = 0;

	    જો (મધ્ય_match_len == 0)
		i += j;
	    found_one = TRUE;
	    તોડો;
	}

	જો (મધ્ય_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    જો (!got_com && flags != ખાલી)
		*flags = saved_flags;
	    i += મધ્ય_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	જો (!found_one)
	    તોડો;

	result = i;

	// Include any trailing white space.
	જ્યારે (VIM_ISWHITE(લીટી[i]))
	    ++i;

	જો (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	જો (vim_strchr(part_buf, COM_NEST) == ખાલી)
	    તોડો;
    }
    પાછા result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    પૂર્ણાંક
get_last_leader_offset(char_u *લીટી, char_u **flags)
{
    પૂર્ણાંક		result = -1;
    પૂર્ણાંક		i, j;
    પૂર્ણાંક		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    પૂર્ણાંક		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (પૂર્ણાંક)STRLEN(લીટી);
    જ્યારે (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	માટે (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (શૂન્ય)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    જો (string == ખાલી)	// If everything is fine, this cannot actually
				// happen.
		ચાલુ;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    જો (VIM_ISWHITE(string[0]))
	    {
		જો (i == 0 || !VIM_ISWHITE(લીટી[i - 1]))
		    ચાલુ;
		જ્યારે (VIM_ISWHITE(*string))
		    ++string;
	    }
	    માટે (j = 0; string[j] != NUL && string[j] == લીટી[i + j]; ++j)
		/* do nothing */;
	    જો (string[j] != NUL)
		ચાલુ;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    જો (vim_strchr(part_buf, COM_BLANK) != ખાલી
		    && !VIM_ISWHITE(લીટી[i + j]) && લીટી[i + j] != NUL)
		ચાલુ;

	    જો (vim_strchr(part_buf, COM_મધ્ય) != ખાલી)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		માટે (j = 0; VIM_ISWHITE(લીટી[j]) && j <= i; j++)
		    ;
		જો (j < i)
		    ચાલુ;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    જો (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    તોડો;
	}

	જો (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    પૂર્ણાંક     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    જો (vim_strchr(part_buf, COM_NEST) != ખાલી)
		ચાલુ;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    જ્યારે (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (પૂર્ણાંક)STRLEN(com_leader);

	    માટે (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(શૂન્ય)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		જો (flags_save == com_flags)
		    ચાલુ;
		string = vim_strchr(part_buf2, ':');
		++string;
		જ્યારે (VIM_ISWHITE(*string))
		    ++string;
		len2 = (પૂર્ણાંક)STRLEN(string);
		જો (len2 == 0)
		    ચાલુ;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		માટે (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    જો (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			જો (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    પાછા result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    પૂર્ણાંક
plines(linenr_T lnum)
{
    પાછા plines_win(curwin, lnum, TRUE);
}

    પૂર્ણાંક
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    પૂર્ણાંક		limit_winheight)	// when TRUE limit to window height
{
#જો defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    પાછા plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    પૂર્ણાંક
plines_nofill(linenr_T lnum)
{
    પાછા plines_win_nofill(curwin, lnum, TRUE);
}

    પૂર્ણાંક
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    પૂર્ણાંક		limit_winheight)	// when TRUE limit to window height
{
#endif
    પૂર્ણાંક		લીટીઓ;

    જો (wp->w_width == 0)
	પાછા 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    જો (લીટી_Folded(wp, lnum) == TRUE)
	પાછા 1;
#endif

    જો (!wp->w_p_wrap)
	લીટીઓ = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_ગણતરી_above_below(wp->w_buffer, lnum)
#endif
	;
    નહીંતર
	લીટીઓ = plines_win_nofold(wp, lnum);

    જો (limit_winheight && લીટીઓ > wp->w_height)
	પાછા wp->w_height;
    પાછા લીટીઓ;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    પૂર્ણાંક
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    પૂર્ણાંક		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    જો (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	પાછા 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (પૂર્ણાંક)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    જો (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    જો (width <= 0)
	પાછા 32000;
    જો (col <= width)
	પાછા 1;
    col -= width;
    width += win_col_off2(wp);
    પાછા (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    પૂર્ણાંક
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    પૂર્ણાંક		લીટીઓ = 0;
    પૂર્ણાંક		width;
    char_u	*લીટી;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    લીટીઓ = diff_check_fill(wp, lnum);
#endif

    જો (!wp->w_p_wrap)
	પાછા લીટીઓ + 1;

    જો (wp->w_width == 0)
	પાછા લીટીઓ + 1;

    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, લીટી, લીટી);
    // "column" is a byte index, advance the pointer until it is reached.
    જ્યારે (*cts.cts_ptr != NUL && cts.cts_ptr < લીટી + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ખાલી, ખાલી);
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
    જો (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, ખાલી, ખાલી) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    જો (width <= 0)
	પાછા 9999;

    લીટીઓ += 1;
    જો (col > width)
	લીટીઓ += (col - width) / (width + win_col_off2(wp)) + 1;
    પાછા લીટીઓ;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    પૂર્ણાંક
plines_m_win(win_T *wp, linenr_T first, linenr_T last, પૂર્ણાંક max)
{
    પૂર્ણાંક		ગણતરી = 0;

    જ્યારે (first <= last && ગણતરી < max)
    {
#ifdef FEAT_FOLDING
	પૂર્ણાંક	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_ગણતરી(wp, first, ખાલી);
	જો (x > 0)
	{
	    ++ગણતરી;	    // count 1 for "+-- folded" line
	    first += x;
	}
	નહીંતર
#endif
	{
#ifdef FEAT_DIFF
	    જો (first == wp->w_topline)
		ગણતરી += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    નહીંતર
#endif
		ગણતરી += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    જો (first == wp->w_buffer->b_ml.ml_લીટી_ગણતરી + 1)
	ગણતરી += diff_check_fill(wp, first);
#endif
    પાછા MIN(max, ગણતરી);
}

    પૂર્ણાંક
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    પૂર્ણાંક		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    જો (pos->col == MAXCOL)
	પાછા NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    જો (pos->col > ptrlen)
	પાછા NUL;
    જો (has_mbyte)
	પાછા (*mb_ptr2char)(ptr);
    પાછા (પૂર્ણાંક)*ptr;
}

    પૂર્ણાંક
gchar_cursor(શૂન્ય)
{
    જો (has_mbyte)
	પાછા (*mb_ptr2char)(ml_get_cursor());
    પાછા (પૂર્ણાંક)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    પૂર્ણાંક
char_before_cursor(શૂન્ય)
{
    જો (curwin->w_cursor.col == 0)
	પાછા -1;

    char_u *લીટી = ml_get_curline();

    જો (has_mbyte)
    {
	char_u *p = લીટી + curwin->w_cursor.col;
	પૂર્ણાંક prev_len = (*mb_head_off)(લીટી, p - 1) + 1;
	પાછા mb_ptr2char(p - prev_len);
    }
    પાછા લીટી[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    શૂન્ય
pchar_cursor(પૂર્ણાંક c)
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
    જો (*p == ',')
	++p;
    જ્યારે (*p == ' ')
	++p;
    પાછા p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    શૂન્ય
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer == buf && wp->w_status_height)
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
    પૂર્ણાંક
ask_yesno(char_u *str, પૂર્ણાંક direct)
{
    પૂર્ણાંક	    r = ' ';
    પૂર્ણાંક	    save_State = State;

    જો (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ચાવીઓ;		// no mapping here, but recognize keys

    જ્યારે (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	જો (direct)
	    r = get_keystroke();
	નહીંતર
	    r = plain_vgetc();
	જો (r == Ctrl_C || r == ESC)
	    r = 'n';
	સંદેશ_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ચાવીઓ;

    પાછા r;
}

#જો defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    શૂન્ય
get_mode(char_u *buf)
{
    પૂર્ણાંક		i = 0;

    જો (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    નહીંતર જો (term_use_loop())
    {
	જો (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    નહીંતર જો (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	જો (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	નહીંતર જો (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    નહીંતર જો (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    નહીંતર જો (State & MODE_ઉમેરો)
    {
	જો (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	નહીંતર
	{
	    જો (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    નહીંતર
		buf[i++] = 'i';
	}

	જો (ins_compl_active())
	    buf[i++] = 'c';
	નહીંતર જો (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    નહીંતર જો ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	જો (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	નહીંતર જો (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	જો ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    નહીંતર જો (VIsual_active)
    {
	જો (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	નહીંતર
	{
	    buf[i++] = VIsual_mode;
	    જો (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    નહીંતર
    {
	buf[i++] = 'n';
	જો (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	નહીંતર જો (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	નહીંતર જો (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    શૂન્ય
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    જો (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	પાછા;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    જો (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    સ્થિર શૂન્ય
may_add_state_char(garray_T *gap, char_u *include, પૂર્ણાંક c)
{
    જો (include == ખાલી || vim_strchr(include, c) != ખાલી)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    શૂન્ય
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ખાલી;
    પૂર્ણાંક		i;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    ga_init2(&ga, 1, 20);
    જો (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    જો (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == ખાલી))
	may_add_state_char(&ga, include, 'm');
    જો (op_pending())
	may_add_state_char(&ga, include, 'o');
    જો (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    જો (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    જો (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    જો (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    માટે (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    જો (સંદેશ_scrolled > 0)
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
    પૂર્ણાંક
get_keystroke(શૂન્ય)
{
    char_u	*buf = ખાલી;
    પૂર્ણાંક		buflen = 150;
    પૂર્ણાંક		maxlen;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		n;
    પૂર્ણાંક		save_mapped_ctrl_c = mapped_ctrl_c;
    પૂર્ણાંક		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    માટે (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	જો (buf == ખાલી)
	    buf = alloc(buflen);
	નહીંતર જો (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    જો (buf == ખાલી)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	જો (buf == ખાલી)
	{
	    do_outofmem_સંદેશ((long_u)buflen);
	    પાછા ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	જો (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_આગત_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	નહીંતર જો (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	જો ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ચાલુ;

	જો (n == KEYLEN_REMOVED)  // key code removed
	{
	    જો (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ચાલુ;
	}
	જો (n > 0)		// found a termcode: adjust length
	    len = n;
	જો (len == 0)		// nothing typed yet
	    ચાલુ;

	// Handle modifier and/or special key code.
	n = buf[0];
	જો (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    જો (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ચાવી(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		જો (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		જો (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ચાલુ;
	    }
	    તોડો;
	}
	જો (has_mbyte)
	{
	    જો (MB_BYTE2LEN(n) > len)
		ચાલુ;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	જો (n == intr_char)
	    n = ESC;
#endif
	તોડો;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    પાછા merge_modify_Other_ચાવીઓ(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    સ્થિર પૂર્ણાંક
vim_append_digit_int(પૂર્ણાંક *કિંમત, પૂર્ણાંક digit)
{
    પૂર્ણાંક x = *કિંમત;
    જો (x > ((INT_MAX - digit) / 10))
	પાછા FAIL;
    *કિંમત = x * 10 + digit;
    પાછા OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    પૂર્ણાંક
get_સંખ્યા(
    પૂર્ણાંક	    colon,			// allow colon to abort
    પૂર્ણાંક	    *mouse_used)
{
    પૂર્ણાંક	n = 0;
    પૂર્ણાંક	c;
    પૂર્ણાંક typed = 0;

    જો (mouse_used != ખાલી)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    જો (સંદેશ_silent != 0)
	પાછા 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_ચાવીઓ;		// no mapping here, but recognize keys
    માટે (;;)
    {
	windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
	c = safe_vgetc();
	જો (VIM_ISDIGIT(c))
	{
	    જો (vim_append_digit_int(&n, c - '0') == FAIL)
		પાછા 0;
	    સંદેશ_putchar(c);
	    ++typed;
	}
	નહીંતર જો (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    જો (typed > 0)
	    {
		સંદેશ_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	નહીંતર જો (mouse_used != ખાલી && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    તોડો;
	}
	નહીંતર જો (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    જો (!exmode_active)
		cmdline_row = સંદેશ_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    તોડો;
	}
	નહીંતર જો (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    તોડો;
	}
	નહીંતર જો (c == CAR || c == NL )
	    તોડો;
    }
    --no_mapping;
    --allow_ચાવીઓ;
    પાછા n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    પૂર્ણાંક
prompt_for_સંખ્યા(પૂર્ણાંક *mouse_used)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		save_cmdline_row;
    પૂર્ણાંક		save_State;

    // When using ":silent" assume that <CR> was entered.
    જો (mouse_used != ખાલી)
	સંદેશ_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    નહીંતર
	સંદેશ_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_સંખ્યા(TRUE, mouse_used);
    જો (ચાવી_Typed)
    {
	// don't call wait_return() now
	જો (સંદેશ_row > 0)
	    cmdline_row = સંદેશ_row - 1;
	need_wait_return = FALSE;
	સંદેશ_didany = FALSE;
	સંદેશ_didout = FALSE;
    }
    નહીંતર
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    પાછા i;
}

    શૂન્ય
msgmore(long n)
{
    long pn;

    જો (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	પાછા;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    જો (keep_સંદેશ != ખાલી && !keep_સંદેશ_more)
	પાછા;

    જો (n > 0)
	pn = n;
    નહીંતર
	pn = -n;

    જો (pn > p_report)
    {
	જો (n > 0)
	    vim_snprintf(સંદેશ_buf, સંદેશ_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	નહીંતર
	    vim_snprintf(સંદેશ_buf, સંદેશ_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	જો (got_int)
	    vim_strcat((char_u *)સંદેશ_buf, (char_u *)_(" (Interrupted)"),
								  સંદેશ_BUF_LEN);
	જો (સંદેશ(સંદેશ_buf))
	{
	    set_keep_સંદેશ((char_u *)સંદેશ_buf, 0);
	    keep_સંદેશ_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    શૂન્ય
beep_flush(શૂન્ય)
{
    જો (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    શૂન્ય
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    જો (emsg_silent != 0 || in_assert_fails)
	પાછા;

    જો (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	સ્થિર પૂર્ણાંક		did_init = FALSE;
	સ્થિર elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	જો (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    જો (p_vb
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
		જો (has_vtp_working() != 0
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
	    નહીંતર
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    જો (vim_strchr(p_debug, 'e') != ખાલી)
    {
	સંદેશ_source(HL_ATTR(HLF_W));
	સંદેશ_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    શૂન્ય
init_homedir(શૂન્ય)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#નહીંતર
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    જો (var == ખાલી || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	જો (homepath == ખાલી || *homepath == NUL)
	    homepath = (char_u *)"\\";
	જો (homedrive != ખાલી
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((અક્ષર *)NameBuff, "%s%s", homedrive, homepath);
	    જો (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    જો (var == ખાલી)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    જો (var != ખાલી && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	જો (p != ખાલી)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    જો (exp != ખાલી && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((અક્ષર *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    જો (var != ખાલી && *var == NUL)	// empty is same as not set
	var = ખાલી;

    જો (enc_utf8 && var != ખાલી)
    {
	પૂર્ણાંક	len;
	char_u  *pp = ખાલી;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (પૂર્ણાંક)STRLEN(var), &pp, &len);
	જો (pp != ખાલી)
	{
	    homedir = pp;
	    પાછા;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    જો (var == ખાલી)
	var = (char_u *)"C:/";
#endif

    જો (var != ખાલી)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	જો (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((અક્ષર *)NameBuff) == 0)
	{
	    જો (!mch_chdir((અક્ષર *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    જો (mch_chdir((અક્ષર *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#જો defined(EXITFREE)
    શૂન્ય
free_homedir(શૂન્ય)
{
    vim_free(homedir);
}

    શૂન્ય
free_users(શૂન્ય)
{
    ga_clear_strings(&ga_users);
}
#endif

#જો defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    શૂન્ય
init_vimdir(શૂન્ય)
{
    પૂર્ણાંક	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    જો (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    જો (mustfree)
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
    પાછા expand_env_save_opt(src, FALSE, ખાલી);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, પૂર્ણાંક one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    જો (p != ખાલી)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ખાલી);
    પાછા p;
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
    પૂર્ણાંક		dstlen)		// maximum length of the result
{
    પાછા expand_env_esc(src, dst, dstlen, ખાલી, FALSE, ખાલી);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    પૂર્ણાંક		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    પૂર્ણાંક		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    પૂર્ણાંક		c;
    char_u	*var;
    પૂર્ણાંક		copy_char;
    પૂર્ણાંક		mustfree;	// var was allocated, need to free it later
    પૂર્ણાંક		at_start = TRUE; // at start of a name
    પૂર્ણાંક		startstr_len = 0;
    char_u	*dst_start = dst;

    જો (startstr != ખાલી)
	startstr_len = (પૂર્ણાંક)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    જ્યારે (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	જો (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (શૂન્ય)skip_પદાવલિ(&src, ખાલી);
	    જો (*src == '`')
		++src;
	    len = src - var;
	    જો (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (પૂર્ણાંક)len;
	    ચાલુ;
	}
#endif
	copy_char = TRUE;
	જો ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#જો defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    જો (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		જો (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    જ્યારે (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		નહીંતર
#endif
		{
		    જ્યારે (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#જો defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#જો defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		જો (src[1] == '{' && *tail != '}')
# નહીંતર
		જો (*src == '%' && *tail != '%')
# endif
		    var = ખાલી;
		નહીંતર
		{
# ifdef UNIX
		    જો (src[1] == '{')
# નહીંતર
		    જો (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#જો defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    નહીંતર જો (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ખાલી)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    નહીંતર					// user directory
	    {
#જો defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		જ્યારે (	   c-- > 0
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
#  જો defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    સંરચના passwd *pw = (*dst == NUL)
					? ખાલી : getpwnam((અક્ષર *)dst + 1);

		    var = (pw == ખાલી) ? ખાલી : (char_u *)pw->pw_dir;
		}
		જો (var == ખાલી)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ખાલી,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# નહીંતર	// !UNIX, thus VMS
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
		    જ્યારે (*next_path)
		    {
			માટે (path = next_path; *next_path && *next_path != ',';
				next_path++);
			જો (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (અક્ષર *)test,
			    કદ(test),
			    "%s/%s",
			    path,
			    dst + 1);
			જો (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    જો (var != ખાલી)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    તોડો;
			}
		    }
		}
# endif // UNIX
#નહીંતર
		// cannot expand user's home directory, so don't try
		var = ખાલી;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    જો (p_ssl && var != ખાલી && vim_strchr(var, '\\') != ખાલી)
	    {
		char_u	*p = vim_strsave(var);

		જો (p != ખાલી)
		{
		    જો (mustfree)
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
	    જો (esc_chars != ખાલી && var != ખાલી
		    && vim_strpbrk(var, esc_chars) != ખાલી)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		જો (p != ખાલી)
		{
		    જો (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    જો (var != ખાલી && *var != NUL)
	    {
		c = (પૂર્ણાંક)STRLEN(var);

		જો (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    જો (after_pathsep(dst, dst + c)
#જો defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    જો (mustfree)
		vim_free(var);
	}

	જો (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    જો (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    નહીંતર જો ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    જો (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		જો (startstr != ખાલી && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    પાછા (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    સ્થિર char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    જો (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	પાછા newend;
    પાછા pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    સ્થિર char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    જો (vimdir == ખાલી || *vimdir == NUL)
	પાછા ખાલી;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    જો (p.string != ખાલી && mch_isdir(p.string))
	પાછા p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    જો (p.string != ખાલી && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	જો (fname.string != ખાલી)
	{
	    પૂર્ણાંક exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    જો (exists)
		પાછા p.string;
	}
    }
    vim_free(p.string);
    પાછા ખાલી;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, પૂર્ણાંક *mustfree)
{
    char_u	*p = ખાલી;
    char_u	*pend;
    પૂર્ણાંક		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    જો (STRCMP(name, "HOME") == 0)
	પાછા homedir;

    // Use Wide function
    wn = enc_to_utf16(name, ખાલી);
    જો (wn == ખાલી)
	પાછા ખાલી;

    wp = _wgetenv(wn);
    vim_free(wn);

    જો (wp != ખાલી && *wp == NUL)   // empty is the same as not set
	wp = ખાલી;

    જો (wp != ખાલી)
    {
	p = utf16_to_enc(wp, ખાલી);
	જો (p == ખાલી)
	    પાછા ખાલી;

	*mustfree = TRUE;
	પાછા p;
    }
#નહીંતર
    p = mch_getenv(name);
    જો (p != ખાલી && *p == NUL)	    // empty is the same as not set
	p = ખાલી;

    જો (p != ખાલી)
	પાછા p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    જો (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	સ્થિર અક્ષર userSettingsPath[MAXPATHL];

	જો (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    પાછા (char_u *)userSettingsPath;
	નહીંતર
	    પાછા ખાલી;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    જો (!vimruntime && STRCMP(name, "VIM") != 0)
	પાછા ખાલી;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    જો (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	જો (wp != ખાલી && *wp == NUL)	    // empty is the same as not set
	    wp = ખાલી;
	જો (wp != ખાલી)
	{
	    char_u *q = utf16_to_enc(wp, ખાલી);
	    જો (q != ખાલી)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		જો (p == ખાલી)
		    p = q;
	    }
	}
#નહીંતર
	p = mch_getenv((char_u *)"VIM");
	જો (p != ખાલી && *p == NUL)	    // empty is the same as not set
	    p = ખાલી;
	જો (p != ખાલી)
	{
	    p = vim_version_dir(p);
	    જો (p != ખાલી)
		*mustfree = TRUE;
	    નહીંતર
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    જો (p == ખાલી)
    {
	જો (p_hf != ખાલી && vim_strchr(p_hf, '$') == ખાલી)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	નહીંતર
	    p = exe_name;
#endif
	જો (p != ખાલી)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    જો (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    જો (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		જો (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    જો (pnew != ખાલી)
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
	    જો (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    જો (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    જો (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    જો (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    જો (p != ખાલી && !mch_isdir(p))
		VIM_CLEAR(p);
	    નહીંતર
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		જો (vimruntime && (pend = vim_version_dir(p)) != ખાલી)
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
    જો (p == ખાલી)
    {
	// Only use default_vimruntime_dir when it is not empty
	જો (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	નહીંતર જો (*default_vim_dir != NUL)
	{
	    જો (vimruntime && (p = vim_version_dir(default_vim_dir)) != ખાલી)
		*mustfree = TRUE;
	    નહીંતર
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
    જો (p != ખાલી)
    {
	જો (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	નહીંતર
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    પાછા p;
}

    શૂન્ય
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((અક્ષર *)var);
#નહીંતર
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    શૂન્ય
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    જો (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    નહીંતર જો (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#જો defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    શૂન્ય
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    જો (STRICMP(name, "HOME") == 0)
	init_homedir();
    નહીંતર જો (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    નહીંતર જો (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    શૂન્ય
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((અક્ષર *)name, (અક્ષર *)val, 1);
#નહીંતર
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    જો (envbuf != ખાલી)
    {
	sprintf((અક્ષર *)envbuf, "%s=%s", name, val);
	putenv((અક્ષર *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    જો (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	જો (buf != ખાલી)
	{
	    bindtextdomain(VIMPACKAGE, (અક્ષર *)buf);
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
    પૂર્ણાંક		idx)
{
#જો defined(AMIGA)
    // No environ[] on the Amiga.
    પાછા ખાલી;
#નહીંતર
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    બાહ્ય અક્ષર		**environ;
# endif
    char_u		*str;
    પૂર્ણાંક			n;

    str = (char_u *)environ[idx];
    જો (str == ખાલી)
	પાછા ખાલી;

    માટે (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	જો (str[n] == '=' || str[n] == NUL)
	    તોડો;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    પાછા xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    સ્થિર શૂન્ય
add_user(char_u *user, પૂર્ણાંક need_copy)
{
    char_u	*user_copy = (user != ખાલી && need_copy)
						    ? vim_strsave(user) : user;

    જો (user_copy == ખાલી || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	જો (need_copy)
	    vim_free(user_copy);
	પાછા;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    સ્થિર શૂન્ય
init_users(શૂન્ય)
{
    સ્થિર પૂર્ણાંક	lazy_init_done = FALSE;

    જો (lazy_init_done)
	પાછા;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, કદ(char_u *), 20);

#જો defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	સંરચના passwd*	pw;

	setpwent();
	જ્યારે ((pw = getpwent()) != ખાલી)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#અથવાજો defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	જો (NetUserEnum(ખાલી, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, ખાલી) == NERR_Success)
	{
	    માટે (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, ખાલી), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#જો defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	જો (user_env != ખાલી && *user_env != NUL)
	{
	    પૂર્ણાંક	i;

	    માટે (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		જો (STRCMP(local_user, user_env) == 0)
		    તોડો;
	    }

	    જો (i == ga_users.ga_len)
	    {
		સંરચના passwd	*pw = getpwnam((અક્ષર *)user_env);

		જો (pw != ખાલી)
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
get_users(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    init_users();
    જો (idx < ga_users.ga_len)
	પાછા ((char_u **)ga_users.ga_data)[idx];
    પાછા ખાલી;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    પૂર્ણાંક
match_user(char_u *name)
{
    પૂર્ણાંક i;
    પૂર્ણાંક n = (પૂર્ણાંક)STRLEN(name);
    પૂર્ણાંક result = 0;

    init_users();
    માટે (i = 0; i < ga_users.ga_len; i++)
    {
	જો (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    પાછા 2; // full match
	જો (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    પાછા result;
}

    સ્થિર શૂન્ય
prepare_to_exit(શૂન્ય)
{
#જો defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    નહીંતર
#endif
    {
	windgoto((પૂર્ણાંક)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	જો (!full_screen)
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
    શૂન્ય
preserve_exit(શૂન્ય)
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
	જો (buf->b_ml.ml_mfp != ખાલી && buf->b_ml.ml_mfp->mf_fname != ખાલી)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    તોડો;
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

સ્થિર પૂર્ણાંક	breakcheck_ગણતરી = 0;

    શૂન્ય
લીટી_breakcheck(શૂન્ય)
{
    જો (++breakcheck_ગણતરી >= BREAKCHECK_SKIP)
    {
	breakcheck_ગણતરી = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    શૂન્ય
fast_breakcheck(શૂન્ય)
{
    જો (++breakcheck_ગણતરી >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_ગણતરી = 0;
	ui_breakcheck();
    }
}

#જો defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    શૂન્ય
veryfast_breakcheck(શૂન્ય)
{
    જો (++breakcheck_ગણતરી >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_ગણતરી = 0;
	ui_breakcheck();
    }
}
#endif

#જો defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    પૂર્ણાંક		flags,		// can be SHELL_SILENT
    પૂર્ણાંક		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = ખાલી;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i = 0;
    FILE	*fd;

    જો (check_restricted() || check_secure())
	પાછા ખાલી;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    જો ((tempname = vim_tempname('o', TRUE)) == ખાલી)
    {
	emsg(_(e_cant_get_temp_file_name));
	પાછા ખાલી;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    જો (command == ખાલી)
	જાઓ done;

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
    fd = mch_fopen((અક્ષર *)tempname, "r");
# નહીંતર
    fd = mch_fopen((અક્ષર *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    જો (fd == ખાલી
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	જો (fd != ખાલી)
	    fclose(fd);
	જાઓ done;
    }

    buffer = alloc(len + 1);
    જો (buffer != ખાલી)
	i = (પૂર્ણાંક)fread((અક્ષર *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    જો (buffer == ખાલી)
	જાઓ done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    જો (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    નહીંતર જો (ret_len == ખાલી)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	માટે (i = 0; i < len; ++i)
	    જો (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    નહીંતર
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    પાછા buffer;
}

# જો defined(FEAT_EVAL)

    સ્થિર શૂન્ય
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    પૂર્ણાંક		retlist)
{
    char_u	*res = ખાલી;
    char_u	*p;
    char_u	*infile = ખાલી;
    પૂર્ણાંક		err = FALSE;
    FILE	*fd;
    list_T	*list = ખાલી;
    પૂર્ણાંક		flags = SHELL_SILENT;
    પૂર્ણાંક		use_argv = FALSE;
    અક્ષર	**argv = ખાલી;
    પૂર્ણાંક		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
    જો (check_restricted() || check_secure())
	જાઓ errret;

    જો (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_સંખ્યા_or_list_arg(argvars, 1)
								      == FAIL))
	પાછા;

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	જો ((infile = vim_tempname('i', TRUE)) == ખાલી)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    જાઓ errret;
	}

	fd = mch_fopen((અક્ષર *)infile, WRITEBIN);
	જો (fd == ખાલી)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    જાઓ errret;
	}
	જો (argvars[1].v_type == VAR_સંખ્યા)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_સંખ્યા);
	    જો (buf == ખાલી)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_સંખ્યા);
		fclose(fd);
		જાઓ errret;
	    }

	    માટે (lnum = 1; lnum <= buf->b_ml.ml_લીટી_ગણતરી; lnum++)
	    {
		માટે (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    જો (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			તોડો;
		    }
		જો (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    તોડો;
		}
	    }
	}
	નહીંતર જો (argvars[1].v_type == VAR_LIST)
	{
	    જો (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	નહીંતર
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    જો (p == ખાલી)
	    {
		fclose(fd);
		જાઓ errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    જો (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	જો (fclose(fd) != 0)
	    err = TRUE;
	જો (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    જાઓ errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    જો (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	જો (l == ખાલી || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    જાઓ errret;
	}
	જો (build_argv_from_list(l, &argv, &argc) == FAIL)
	    જાઓ errret;
	જો (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    જાઓ errret;
	}
	use_argv = TRUE;

	જો (p_verbose > 3)
	{
	    પૂર્ણાંક		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    માટે (i = 0; i < argc; ++i)
	    {
		જો (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (અક્ષર *)ga.ga_data);
	    સંદેશ_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    જો (!સંદેશ_silent)
	flags += SHELL_COOKED;

    જો (retlist)
    {
	પૂર્ણાંક		len;
	listitem_T	*li;
	char_u		*s = ખાલી;
	char_u		*start;
	char_u		*end;
	પૂર્ણાંક		i;

	જો (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	નહીંતર
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	જો (res == ખાલી)
	    જાઓ errret;

	list = list_alloc();
	જો (list == ખાલી)
	    જાઓ errret;

	માટે (i = 0; i < len; ++i)
	{
	    start = res + i;
	    જ્યારે (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    જો (s == ખાલી)
		જાઓ errret;

	    માટે (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    જો (li == ખાલી)
	    {
		vim_free(s);
		જાઓ errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = ખાલી;
    }
    નહીંતર
    {
	જો (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, ખાલી);
	નહીંતર
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ખાલી);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	જો (res != ખાલી)
	{
	    char_u	*s, *d;

	    d = res;
	    માટે (s = res; *s; ++s)
	    {
		જો (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ખાલી;
    }

errret:
    જો (argv != ખાલી)
    {
	પૂર્ણાંક i;
	માટે (i = 0; argv[i] != ખાલી; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    જો (infile != ખાલી)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    જો (res != ખાલી)
	vim_free(res);
    જો (list != ખાલી)
	list_free(list);
}

/*
 * "system()" function
 */
    શૂન્ય
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    શૂન્ય
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
    પૂર્ણાંક
goto_im(શૂન્ય)
{
    પાછા (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(શૂન્ય)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#નહીંતર
    p = skiptowhite(p_sh);
    જો (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    નહીંતર
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	માટે (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    જો (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    પાછા p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    પૂર્ણાંક
path_is_url(char_u *p)
{
    જો (STRNCMP(p, "://", (size_t)3) == 0)
	પાછા URL_SLASH;
    નહીંતર જો (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	પાછા URL_BACKSLASH;
    પાછા 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    પૂર્ણાંક
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    જો (!ASCII_ISALPHA(*fname))
	પાછા 0;

    // check body: alpha or dash
    માટે (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    જો (p[-1] == '-')
	પાછા 0;

    // "://" or ":\\" must follow
    પાછા path_is_url(p);
}

#જો defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    જો (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    નહીંતર
	sve->sve_did_save = FALSE;
    પાછા v_event;
}

    શૂન્ય
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    જો (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    નહીંતર
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    શૂન્ય
may_trigger_modechanged(શૂન્ય)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    જો (!has_modechanged() || got_int)
	પાછા;

    get_mode(curr_mode);
    જો (STRCMP(curr_mode, last_mode) == 0)
	પાછા;

    v_event = get_v_event(&save_v_event);
    (શૂન્ય)dict_add_string(v_event, "new_mode", curr_mode);
    (શૂન્ય)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_વસ્તુઓ_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((અક્ષર *)pattern_buf, કદ(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, ખાલી, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    પૂર્ણાંક
vim_append_digit_long(long *કિંમત, પૂર્ણાંક digit)
{
    long x = *કિંમત;
    જો (x > ((LONG_MAX - (long)digit) / 10))
	પાછા FAIL;
    *કિંમત = x * 10 + (long)digit;
    પાછા OK;
}

// Return something that fits into an int.
    પૂર્ણાંક
trim_to_int(vimlong_T x)
{
    પાછા x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
