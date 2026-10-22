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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ಯದಿ defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#ಯದಿ defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ಸ್ಥಿರ garray_T	ga_users;

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
    ಪೂರ್ಣಾಙ್ಕ
get_leader_len(
    char_u	*ಸಾಲು,
    char_u	**flags,
    ಪೂರ್ಣಾಙ್ಕ		backward,
    ಪೂರ್ಣಾಙ್ಕ		include_space)
{
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		result;
    ಪೂರ್ಣಾಙ್ಕ		got_com = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    ಪೂರ್ಣಾಙ್ಕ		ಮಧ್ಯ_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ರಿಕ್ತ;

    result = i = 0;
    ಯಾವತ್ (VIM_ISWHITE(ಸಾಲು[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    ಯಾವತ್ (ಸಾಲು[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	ಪ್ರತಿ (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ಯದಿ (!got_com && flags != ರಿಕ್ತ)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (ಶೂನ್ಯ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ಯದಿ (string == ರಿಕ್ತ)	    // missing ':', ignore this part
		ಅನುವರ್ತ;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ಯದಿ (ಮಧ್ಯ_match_len != 0
		    && vim_strchr(part_buf, COM_ಮಧ್ಯ) == ರಿಕ್ತ
		    && vim_strchr(part_buf, COM_END) == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ಯದಿ (got_com && vim_strchr(part_buf, COM_NEST) == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    // When 'O' flag present and using "O" command skip this one.
	    ಯದಿ (backward && vim_strchr(part_buf, COM_NOBACK) != ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ಯದಿ (VIM_ISWHITE(string[0]))
	    {
		ಯದಿ (i == 0 || !VIM_ISWHITE(ಸಾಲು[i - 1]))
		    ಅನುವರ್ತ;  // missing white space
		ಯಾವತ್ (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    ಪ್ರತಿ (j = 0; string[j] != NUL && string[j] == ಸಾಲು[i + j]; ++j)
		;
	    ಯದಿ (string[j] != NUL)
		ಅನುವರ್ತ;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ಯದಿ (vim_strchr(part_buf, COM_BLANK) != ರಿಕ್ತ
			   && !VIM_ISWHITE(ಸಾಲು[i + j]) && ಸಾಲು[i + j] != NUL)
		ಅನುವರ್ತ;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ಯದಿ (vim_strchr(part_buf, COM_ಮಧ್ಯ) != ರಿಕ್ತ)
	    {
		ಯದಿ (ಮಧ್ಯ_match_len == 0)
		{
		    ಮಧ್ಯ_match_len = j;
		    saved_flags = prev_list;
		}
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (ಮಧ್ಯ_match_len != 0 && j > ಮಧ್ಯ_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		ಮಧ್ಯ_match_len = 0;

	    ಯದಿ (ಮಧ್ಯ_match_len == 0)
		i += j;
	    found_one = TRUE;
	    ಭಙ್ಗ;
	}

	ಯದಿ (ಮಧ್ಯ_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ಯದಿ (!got_com && flags != ರಿಕ್ತ)
		*flags = saved_flags;
	    i += ಮಧ್ಯ_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	ಯದಿ (!found_one)
	    ಭಙ್ಗ;

	result = i;

	// Include any trailing white space.
	ಯಾವತ್ (VIM_ISWHITE(ಸಾಲು[i]))
	    ++i;

	ಯದಿ (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ಯದಿ (vim_strchr(part_buf, COM_NEST) == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_last_leader_offset(char_u *ಸಾಲು, char_u **flags)
{
    ಪೂರ್ಣಾಙ್ಕ		result = -1;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    ಪೂರ್ಣಾಙ್ಕ		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    ಪೂರ್ಣಾಙ್ಕ		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು);
    ಯಾವತ್ (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	ಪ್ರತಿ (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (ಶೂನ್ಯ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ಯದಿ (string == ರಿಕ್ತ)	// If everything is fine, this cannot actually
				// happen.
		ಅನುವರ್ತ;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ಯದಿ (VIM_ISWHITE(string[0]))
	    {
		ಯದಿ (i == 0 || !VIM_ISWHITE(ಸಾಲು[i - 1]))
		    ಅನುವರ್ತ;
		ಯಾವತ್ (VIM_ISWHITE(*string))
		    ++string;
	    }
	    ಪ್ರತಿ (j = 0; string[j] != NUL && string[j] == ಸಾಲು[i + j]; ++j)
		/* do nothing */;
	    ಯದಿ (string[j] != NUL)
		ಅನುವರ್ತ;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ಯದಿ (vim_strchr(part_buf, COM_BLANK) != ರಿಕ್ತ
		    && !VIM_ISWHITE(ಸಾಲು[i + j]) && ಸಾಲು[i + j] != NUL)
		ಅನುವರ್ತ;

	    ಯದಿ (vim_strchr(part_buf, COM_ಮಧ್ಯ) != ರಿಕ್ತ)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		ಪ್ರತಿ (j = 0; VIM_ISWHITE(ಸಾಲು[j]) && j <= i; j++)
		    ;
		ಯದಿ (j < i)
		    ಅನುವರ್ತ;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    ಯದಿ (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    ಭಙ್ಗ;
	}

	ಯದಿ (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    ಪೂರ್ಣಾಙ್ಕ     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ಯದಿ (vim_strchr(part_buf, COM_NEST) != ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    ಯಾವತ್ (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(com_leader);

	    ಪ್ರತಿ (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(ಶೂನ್ಯ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ಯದಿ (flags_save == com_flags)
		    ಅನುವರ್ತ;
		string = vim_strchr(part_buf2, ':');
		++string;
		ಯಾವತ್ (VIM_ISWHITE(*string))
		    ++string;
		len2 = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(string);
		ಯದಿ (len2 == 0)
		    ಅನುವರ್ತ;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		ಪ್ರತಿ (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ಯದಿ (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ಯದಿ (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines(linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ plines_win(curwin, lnum, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		limit_winheight)	// when TRUE limit to window height
{
#ಯದಿ defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ಪ್ರತ್ಯಾಗಮ plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines_nofill(linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ plines_win_nofill(curwin, lnum, TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		limit_winheight)	// when TRUE limit to window height
{
#endif
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲುಗಳು;

    ಯದಿ (wp->w_width == 0)
	ಪ್ರತ್ಯಾಗಮ 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ಯದಿ (ಸಾಲು_Folded(wp, lnum) == TRUE)
	ಪ್ರತ್ಯಾಗಮ 1;
#endif

    ಯದಿ (!wp->w_p_wrap)
	ಸಾಲುಗಳು = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_ಎಣಿಕೆ_above_below(wp->w_buffer, lnum)
#endif
	;
    ಅನ್ಯಥಾ
	ಸಾಲುಗಳು = plines_win_nofold(wp, lnum);

    ಯದಿ (limit_winheight && ಸಾಲುಗಳು > wp->w_height)
	ಪ್ರತ್ಯಾಗಮ wp->w_height;
    ಪ್ರತ್ಯಾಗಮ ಸಾಲುಗಳು;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    ಪೂರ್ಣಾಙ್ಕ		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ಯದಿ (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	ಪ್ರತ್ಯಾಗಮ 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (ಪೂರ್ಣಾಙ್ಕ)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ಯದಿ (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    ಯದಿ (width <= 0)
	ಪ್ರತ್ಯಾಗಮ 32000;
    ಯದಿ (col <= width)
	ಪ್ರತ್ಯಾಗಮ 1;
    col -= width;
    width += win_col_off2(wp);
    ಪ್ರತ್ಯಾಗಮ (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲುಗಳು = 0;
    ಪೂರ್ಣಾಙ್ಕ		width;
    char_u	*ಸಾಲು;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ಸಾಲುಗಳು = diff_check_fill(wp, lnum);
#endif

    ಯದಿ (!wp->w_p_wrap)
	ಪ್ರತ್ಯಾಗಮ ಸಾಲುಗಳು + 1;

    ಯದಿ (wp->w_width == 0)
	ಪ್ರತ್ಯಾಗಮ ಸಾಲುಗಳು + 1;

    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
    // "column" is a byte index, advance the pointer until it is reached.
    ಯಾವತ್ (*cts.cts_ptr != NUL && cts.cts_ptr < ಸಾಲು + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ರಿಕ್ತ, ರಿಕ್ತ);
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
    ಯದಿ (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, ರಿಕ್ತ, ರಿಕ್ತ) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    ಯದಿ (width <= 0)
	ಪ್ರತ್ಯಾಗಮ 9999;

    ಸಾಲುಗಳು += 1;
    ಯದಿ (col > width)
	ಸಾಲುಗಳು += (col - width) / (width + win_col_off2(wp)) + 1;
    ಪ್ರತ್ಯಾಗಮ ಸಾಲುಗಳು;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    ಪೂರ್ಣಾಙ್ಕ
plines_m_win(win_T *wp, linenr_T first, linenr_T last, ಪೂರ್ಣಾಙ್ಕ max)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

    ಯಾವತ್ (first <= last && ಎಣಿಕೆ < max)
    {
#ifdef FEAT_FOLDING
	ಪೂರ್ಣಾಙ್ಕ	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_ಎಣಿಕೆ(wp, first, ರಿಕ್ತ);
	ಯದಿ (x > 0)
	{
	    ++ಎಣಿಕೆ;	    // count 1 for "+-- folded" line
	    first += x;
	}
	ಅನ್ಯಥಾ
#endif
	{
#ifdef FEAT_DIFF
	    ಯದಿ (first == wp->w_topline)
		ಎಣಿಕೆ += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    ಅನ್ಯಥಾ
#endif
		ಎಣಿಕೆ += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    ಯದಿ (first == wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಎಣಿಕೆ += diff_check_fill(wp, first);
#endif
    ಪ್ರತ್ಯಾಗಮ MIN(max, ಎಣಿಕೆ);
}

    ಪೂರ್ಣಾಙ್ಕ
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ಯದಿ (pos->col == MAXCOL)
	ಪ್ರತ್ಯಾಗಮ NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ಯದಿ (pos->col > ptrlen)
	ಪ್ರತ್ಯಾಗಮ NUL;
    ಯದಿ (has_mbyte)
	ಪ್ರತ್ಯಾಗಮ (*mb_ptr2char)(ptr);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)*ptr;
}

    ಪೂರ್ಣಾಙ್ಕ
gchar_cursor(ಶೂನ್ಯ)
{
    ಯದಿ (has_mbyte)
	ಪ್ರತ್ಯಾಗಮ (*mb_ptr2char)(ml_get_cursor());
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    ಪೂರ್ಣಾಙ್ಕ
char_before_cursor(ಶೂನ್ಯ)
{
    ಯದಿ (curwin->w_cursor.col == 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    char_u *ಸಾಲು = ml_get_curline();

    ಯದಿ (has_mbyte)
    {
	char_u *p = ಸಾಲು + curwin->w_cursor.col;
	ಪೂರ್ಣಾಙ್ಕ prev_len = (*mb_head_off)(ಸಾಲು, p - 1) + 1;
	ಪ್ರತ್ಯಾಗಮ mb_ptr2char(p - prev_len);
    }
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    ಶೂನ್ಯ
pchar_cursor(ಪೂರ್ಣಾಙ್ಕ c)
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
    ಯದಿ (*p == ',')
	++p;
    ಯಾವತ್ (*p == ' ')
	++p;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    ಶೂನ್ಯ
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer == buf && wp->w_status_height)
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
    ಪೂರ್ಣಾಙ್ಕ
ask_yesno(char_u *str, ಪೂರ್ಣಾಙ್ಕ direct)
{
    ಪೂರ್ಣಾಙ್ಕ	    r = ' ';
    ಪೂರ್ಣಾಙ್ಕ	    save_State = State;

    ಯದಿ (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;		// no mapping here, but recognize keys

    ಯಾವತ್ (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ಯದಿ (direct)
	    r = get_keystroke();
	ಅನ್ಯಥಾ
	    r = plain_vgetc();
	ಯದಿ (r == Ctrl_C || r == ESC)
	    r = 'n';
	ಸಂದೇಶ_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

    ಪ್ರತ್ಯಾಗಮ r;
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    ಶೂನ್ಯ
get_mode(char_u *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    ಯದಿ (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ಅನ್ಯಥಾ ಯದಿ (term_use_loop())
    {
	ಯದಿ (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	ಯದಿ (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	ಅನ್ಯಥಾ ಯದಿ (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
    {
	ಯದಿ (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ಅನ್ಯಥಾ
		buf[i++] = 'i';
	}

	ಯದಿ (ins_compl_active())
	    buf[i++] = 'c';
	ಅನ್ಯಥಾ ಯದಿ (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    ಅನ್ಯಥಾ ಯದಿ ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	ಯದಿ (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	ಅನ್ಯಥಾ ಯದಿ (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	ಯದಿ ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ಅನ್ಯಥಾ ಯದಿ (VIsual_active)
    {
	ಯದಿ (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	ಅನ್ಯಥಾ
	{
	    buf[i++] = VIsual_mode;
	    ಯದಿ (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ಅನ್ಯಥಾ
    {
	buf[i++] = 'n';
	ಯದಿ (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ಅನ್ಯಥಾ ಯದಿ (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ಅನ್ಯಥಾ ಯದಿ (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    ಶೂನ್ಯ
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    ಯದಿ (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ಯದಿ (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_add_state_char(garray_T *gap, char_u *include, ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (include == ರಿಕ್ತ || vim_strchr(include, c) != ರಿಕ್ತ)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    ಶೂನ್ಯ
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ga_init2(&ga, 1, 20);
    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ಯದಿ (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == ರಿಕ್ತ))
	may_add_state_char(&ga, include, 'm');
    ಯದಿ (op_pending())
	may_add_state_char(&ga, include, 'o');
    ಯದಿ (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    ಯದಿ (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    ಯದಿ (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    ಯದಿ (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    ಪ್ರತಿ (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    ಯದಿ (ಸಂದೇಶ_scrolled > 0)
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
    ಪೂರ್ಣಾಙ್ಕ
get_keystroke(ಶೂನ್ಯ)
{
    char_u	*buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		buflen = 150;
    ಪೂರ್ಣಾಙ್ಕ		maxlen;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		save_mapped_ctrl_c = mapped_ctrl_c;
    ಪೂರ್ಣಾಙ್ಕ		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    ಪ್ರತಿ (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ಯದಿ (buf == ರಿಕ್ತ)
	    buf = alloc(buflen);
	ಅನ್ಯಥಾ ಯದಿ (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ಯದಿ (buf == ರಿಕ್ತ)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    do_outofmem_ಸಂದೇಶ((long_u)buflen);
	    ಪ್ರತ್ಯಾಗಮ ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ಯದಿ (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_ಒಳಹರಿವು_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ಯದಿ ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ಅನುವರ್ತ;

	ಯದಿ (n == KEYLEN_REMOVED)  // key code removed
	{
	    ಯದಿ (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ಅನುವರ್ತ;
	}
	ಯದಿ (n > 0)		// found a termcode: adjust length
	    len = n;
	ಯದಿ (len == 0)		// nothing typed yet
	    ಅನುವರ್ತ;

	// Handle modifier and/or special key code.
	n = buf[0];
	ಯದಿ (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    ಯದಿ (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ಕೀಲಿ(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		ಯದಿ (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		ಯದಿ (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ಅನುವರ್ತ;
	    }
	    ಭಙ್ಗ;
	}
	ಯದಿ (has_mbyte)
	{
	    ಯದಿ (MB_BYTE2LEN(n) > len)
		ಅನುವರ್ತ;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	ಯದಿ (n == intr_char)
	    n = ESC;
#endif
	ಭಙ್ಗ;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ಪ್ರತ್ಯಾಗಮ merge_modify_Other_ಕೀಲಿಗಳು(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim_append_digit_int(ಪೂರ್ಣಾಙ್ಕ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ digit)
{
    ಪೂರ್ಣಾಙ್ಕ x = *ಮೌಲ್ಯ;
    ಯದಿ (x > ((INT_MAX - digit) / 10))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *ಮೌಲ್ಯ = x * 10 + digit;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_ಸಂಖ್ಯೆ(
    ಪೂರ್ಣಾಙ್ಕ	    colon,			// allow colon to abort
    ಪೂರ್ಣಾಙ್ಕ	    *mouse_used)
{
    ಪೂರ್ಣಾಙ್ಕ	n = 0;
    ಪೂರ್ಣಾಙ್ಕ	c;
    ಪೂರ್ಣಾಙ್ಕ typed = 0;

    ಯದಿ (mouse_used != ರಿಕ್ತ)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ಯದಿ (ಸಂದೇಶ_silent != 0)
	ಪ್ರತ್ಯಾಗಮ 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;		// no mapping here, but recognize keys
    ಪ್ರತಿ (;;)
    {
	windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col);
	c = safe_vgetc();
	ಯದಿ (VIM_ISDIGIT(c))
	{
	    ಯದಿ (vim_append_digit_int(&n, c - '0') == FAIL)
		ಪ್ರತ್ಯಾಗಮ 0;
	    ಸಂದೇಶ_putchar(c);
	    ++typed;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ಯದಿ (typed > 0)
	    {
		ಸಂದೇಶ_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ಅನ್ಯಥಾ ಯದಿ (mouse_used != ರಿಕ್ತ && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ಯದಿ (!exmode_active)
		cmdline_row = ಸಂದೇಶ_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == CAR || c == NL )
	    ಭಙ್ಗ;
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    ಪೂರ್ಣಾಙ್ಕ
prompt_for_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಙ್ಕ *mouse_used)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		save_cmdline_row;
    ಪೂರ್ಣಾಙ್ಕ		save_State;

    // When using ":silent" assume that <CR> was entered.
    ಯದಿ (mouse_used != ರಿಕ್ತ)
	ಸಂದೇಶ_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_ಸಂಖ್ಯೆ(TRUE, mouse_used);
    ಯದಿ (ಕೀಲಿ_Typed)
    {
	// don't call wait_return() now
	ಯದಿ (ಸಂದೇಶ_row > 0)
	    cmdline_row = ಸಂದೇಶ_row - 1;
	need_wait_return = FALSE;
	ಸಂದೇಶ_didany = FALSE;
	ಸಂದೇಶ_didout = FALSE;
    }
    ಅನ್ಯಥಾ
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ಪ್ರತ್ಯಾಗಮ i;
}

    ಶೂನ್ಯ
msgmore(long n)
{
    long pn;

    ಯದಿ (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ಪ್ರತ್ಯಾಗಮ;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ಯದಿ (keep_ಸಂದೇಶ != ರಿಕ್ತ && !keep_ಸಂದೇಶ_more)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (n > 0)
	pn = n;
    ಅನ್ಯಥಾ
	pn = -n;

    ಯದಿ (pn > p_report)
    {
	ಯದಿ (n > 0)
	    vim_snprintf(ಸಂದೇಶ_buf, ಸಂದೇಶ_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ಅನ್ಯಥಾ
	    vim_snprintf(ಸಂದೇಶ_buf, ಸಂದೇಶ_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ಯದಿ (got_int)
	    vim_strcat((char_u *)ಸಂದೇಶ_buf, (char_u *)_(" (Interrupted)"),
								  ಸಂದೇಶ_BUF_LEN);
	ಯದಿ (ಸಂದೇಶ(ಸಂದೇಶ_buf))
	{
	    set_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_buf, 0);
	    keep_ಸಂದೇಶ_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    ಶೂನ್ಯ
beep_flush(ಶೂನ್ಯ)
{
    ಯದಿ (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    ಶೂನ್ಯ
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    ಯದಿ (emsg_silent != 0 || in_assert_fails)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		did_init = FALSE;
	ಸ್ಥಿರ elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ಯದಿ (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    ಯದಿ (p_vb
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
		ಯದಿ (has_vtp_working() != 0
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
	    ಅನ್ಯಥಾ
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ಯದಿ (vim_strchr(p_debug, 'e') != ರಿಕ್ತ)
    {
	ಸಂದೇಶ_source(HL_ATTR(HLF_W));
	ಸಂದೇಶ_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    ಶೂನ್ಯ
init_homedir(ಶೂನ್ಯ)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ಅನ್ಯಥಾ
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ಯದಿ (var == ರಿಕ್ತ || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ಯದಿ (homepath == ರಿಕ್ತ || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ಯದಿ (homedrive != ರಿಕ್ತ
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((ವರ್ಣ *)NameBuff, "%s%s", homedrive, homepath);
	    ಯದಿ (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ಯದಿ (var == ರಿಕ್ತ)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ಯದಿ (var != ರಿಕ್ತ && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    ಯದಿ (exp != ರಿಕ್ತ && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((ವರ್ಣ *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    ಯದಿ (var != ರಿಕ್ತ && *var == NUL)	// empty is same as not set
	var = ರಿಕ್ತ;

    ಯದಿ (enc_utf8 && var != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	len;
	char_u  *pp = ರಿಕ್ತ;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(var), &pp, &len);
	ಯದಿ (pp != ರಿಕ್ತ)
	{
	    homedir = pp;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ಯದಿ (var == ರಿಕ್ತ)
	var = (char_u *)"C:/";
#endif

    ಯದಿ (var != ರಿಕ್ತ)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ಯದಿ (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((ವರ್ಣ *)NameBuff) == 0)
	{
	    ಯದಿ (!mch_chdir((ವರ್ಣ *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ಯದಿ (mch_chdir((ವರ್ಣ *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_homedir(ಶೂನ್ಯ)
{
    vim_free(homedir);
}

    ಶೂನ್ಯ
free_users(ಶೂನ್ಯ)
{
    ga_clear_strings(&ga_users);
}
#endif

#ಯದಿ defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    ಶೂನ್ಯ
init_vimdir(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ಯದಿ (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ಯದಿ (mustfree)
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
    ಪ್ರತ್ಯಾಗಮ expand_env_save_opt(src, FALSE, ರಿಕ್ತ);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, ಪೂರ್ಣಾಙ್ಕ one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ಯದಿ (p != ರಿಕ್ತ)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ p;
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
    ಪೂರ್ಣಾಙ್ಕ		dstlen)		// maximum length of the result
{
    ಪ್ರತ್ಯಾಗಮ expand_env_esc(src, dst, dstlen, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    ಪೂರ್ಣಾಙ್ಕ		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    ಪೂರ್ಣಾಙ್ಕ		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*var;
    ಪೂರ್ಣಾಙ್ಕ		copy_char;
    ಪೂರ್ಣಾಙ್ಕ		mustfree;	// var was allocated, need to free it later
    ಪೂರ್ಣಾಙ್ಕ		at_start = TRUE; // at start of a name
    ಪೂರ್ಣಾಙ್ಕ		startstr_len = 0;
    char_u	*dst_start = dst;

    ಯದಿ (startstr != ರಿಕ್ತ)
	startstr_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    ಯಾವತ್ (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	ಯದಿ (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&src, ರಿಕ್ತ);
	    ಯದಿ (*src == '`')
		++src;
	    len = src - var;
	    ಯದಿ (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (ಪೂರ್ಣಾಙ್ಕ)len;
	    ಅನುವರ್ತ;
	}
#endif
	copy_char = TRUE;
	ಯದಿ ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#ಯದಿ defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ಯದಿ (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		ಯದಿ (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    ಯಾವತ್ (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		ಅನ್ಯಥಾ
#endif
		{
		    ಯಾವತ್ (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#ಯದಿ defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#ಯದಿ defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ಯದಿ (src[1] == '{' && *tail != '}')
# ಅನ್ಯಥಾ
		ಯದಿ (*src == '%' && *tail != '%')
# endif
		    var = ರಿಕ್ತ;
		ಅನ್ಯಥಾ
		{
# ifdef UNIX
		    ಯದಿ (src[1] == '{')
# ಅನ್ಯಥಾ
		    ಯದಿ (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ಯದಿ defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    ಅನ್ಯಥಾ ಯದಿ (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ರಿಕ್ತ)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    ಅನ್ಯಥಾ					// user directory
	    {
#ಯದಿ defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		ಯಾವತ್ (	   c-- > 0
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
#  ಯದಿ defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ಸಂರಚನಾ passwd *pw = (*dst == NUL)
					? ರಿಕ್ತ : getpwnam((ವರ್ಣ *)dst + 1);

		    var = (pw == ರಿಕ್ತ) ? ರಿಕ್ತ : (char_u *)pw->pw_dir;
		}
		ಯದಿ (var == ರಿಕ್ತ)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ರಿಕ್ತ,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ಅನ್ಯಥಾ	// !UNIX, thus VMS
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
		    ಯಾವತ್ (*next_path)
		    {
			ಪ್ರತಿ (path = next_path; *next_path && *next_path != ',';
				next_path++);
			ಯದಿ (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (ವರ್ಣ *)test,
			    ಪರಿಮಾಣ(test),
			    "%s/%s",
			    path,
			    dst + 1);
			ಯದಿ (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ಯದಿ (var != ರಿಕ್ತ)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    ಭಙ್ಗ;
			}
		    }
		}
# endif // UNIX
#ಅನ್ಯಥಾ
		// cannot expand user's home directory, so don't try
		var = ರಿಕ್ತ;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ಯದಿ (p_ssl && var != ರಿಕ್ತ && vim_strchr(var, '\\') != ರಿಕ್ತ)
	    {
		char_u	*p = vim_strsave(var);

		ಯದಿ (p != ರಿಕ್ತ)
		{
		    ಯದಿ (mustfree)
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
	    ಯದಿ (esc_chars != ರಿಕ್ತ && var != ರಿಕ್ತ
		    && vim_strpbrk(var, esc_chars) != ರಿಕ್ತ)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ಯದಿ (p != ರಿಕ್ತ)
		{
		    ಯದಿ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ಯದಿ (var != ರಿಕ್ತ && *var != NUL)
	    {
		c = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(var);

		ಯದಿ (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ಯದಿ (after_pathsep(dst, dst + c)
#ಯದಿ defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    ಯದಿ (mustfree)
		vim_free(var);
	}

	ಯದಿ (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    ಯದಿ (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    ಯದಿ (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ಯದಿ (startstr != ರಿಕ್ತ && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    ಪ್ರತ್ಯಾಗಮ (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ಸ್ಥಿರ char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ಯದಿ (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ಪ್ರತ್ಯಾಗಮ newend;
    ಪ್ರತ್ಯಾಗಮ pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ಸ್ಥಿರ char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ಯದಿ (vimdir == ರಿಕ್ತ || *vimdir == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    ಯದಿ (p.string != ರಿಕ್ತ && mch_isdir(p.string))
	ಪ್ರತ್ಯಾಗಮ p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ಯದಿ (p.string != ರಿಕ್ತ && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ಯದಿ (fname.string != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ಯದಿ (exists)
		ಪ್ರತ್ಯಾಗಮ p.string;
	}
    }
    vim_free(p.string);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, ಪೂರ್ಣಾಙ್ಕ *mustfree)
{
    char_u	*p = ರಿಕ್ತ;
    char_u	*pend;
    ಪೂರ್ಣಾಙ್ಕ		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ಯದಿ (STRCMP(name, "HOME") == 0)
	ಪ್ರತ್ಯಾಗಮ homedir;

    // Use Wide function
    wn = enc_to_utf16(name, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    wp = _wgetenv(wn);
    vim_free(wn);

    ಯದಿ (wp != ರಿಕ್ತ && *wp == NUL)   // empty is the same as not set
	wp = ರಿಕ್ತ;

    ಯದಿ (wp != ರಿಕ್ತ)
    {
	p = utf16_to_enc(wp, ರಿಕ್ತ);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	*mustfree = TRUE;
	ಪ್ರತ್ಯಾಗಮ p;
    }
#ಅನ್ಯಥಾ
    p = mch_getenv(name);
    ಯದಿ (p != ರಿಕ್ತ && *p == NUL)	    // empty is the same as not set
	p = ರಿಕ್ತ;

    ಯದಿ (p != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ಯದಿ (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ಸ್ಥಿರ ವರ್ಣ userSettingsPath[MAXPATHL];

	ಯದಿ (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)userSettingsPath;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ಯದಿ (!vimruntime && STRCMP(name, "VIM") != 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ಯದಿ (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ಯದಿ (wp != ರಿಕ್ತ && *wp == NUL)	    // empty is the same as not set
	    wp = ರಿಕ್ತ;
	ಯದಿ (wp != ರಿಕ್ತ)
	{
	    char_u *q = utf16_to_enc(wp, ರಿಕ್ತ);
	    ಯದಿ (q != ರಿಕ್ತ)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		ಯದಿ (p == ರಿಕ್ತ)
		    p = q;
	    }
	}
#ಅನ್ಯಥಾ
	p = mch_getenv((char_u *)"VIM");
	ಯದಿ (p != ರಿಕ್ತ && *p == NUL)	    // empty is the same as not set
	    p = ರಿಕ್ತ;
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    p = vim_version_dir(p);
	    ಯದಿ (p != ರಿಕ್ತ)
		*mustfree = TRUE;
	    ಅನ್ಯಥಾ
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ಯದಿ (p == ರಿಕ್ತ)
    {
	ಯದಿ (p_hf != ರಿಕ್ತ && vim_strchr(p_hf, '$') == ರಿಕ್ತ)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ಅನ್ಯಥಾ
	    p = exe_name;
#endif
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ಯದಿ (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ಯದಿ (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		ಯದಿ (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ಯದಿ (pnew != ರಿಕ್ತ)
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
	    ಯದಿ (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ಯದಿ (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    ಯದಿ (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    ಯದಿ (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ಯದಿ (p != ರಿಕ್ತ && !mch_isdir(p))
		VIM_CLEAR(p);
	    ಅನ್ಯಥಾ
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ಯದಿ (vimruntime && (pend = vim_version_dir(p)) != ರಿಕ್ತ)
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
    ಯದಿ (p == ರಿಕ್ತ)
    {
	// Only use default_vimruntime_dir when it is not empty
	ಯದಿ (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (*default_vim_dir != NUL)
	{
	    ಯದಿ (vimruntime && (p = vim_version_dir(default_vim_dir)) != ರಿಕ್ತ)
		*mustfree = TRUE;
	    ಅನ್ಯಥಾ
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
    ಯದಿ (p != ರಿಕ್ತ)
    {
	ಯದಿ (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

    ಶೂನ್ಯ
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((ವರ್ಣ *)var);
#ಅನ್ಯಥಾ
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    ಶೂನ್ಯ
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ಯದಿ (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    ಶೂನ್ಯ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ಯದಿ (STRICMP(name, "HOME") == 0)
	init_homedir();
    ಅನ್ಯಥಾ ಯದಿ (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    ಶೂನ್ಯ
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((ವರ್ಣ *)name, (ವರ್ಣ *)val, 1);
#ಅನ್ಯಥಾ
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ಯದಿ (envbuf != ರಿಕ್ತ)
    {
	sprintf((ವರ್ಣ *)envbuf, "%s=%s", name, val);
	putenv((ವರ್ಣ *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ಯದಿ (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    bindtextdomain(VIMPACKAGE, (ವರ್ಣ *)buf);
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
    ಪೂರ್ಣಾಙ್ಕ		idx)
{
#ಯದಿ defined(AMIGA)
    // No environ[] on the Amiga.
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ಅನ್ಯಥಾ
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    ಬಾಹ್ಯ ವರ್ಣ		**environ;
# endif
    char_u		*str;
    ಪೂರ್ಣಾಙ್ಕ			n;

    str = (char_u *)environ[idx];
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ಯದಿ (str[n] == '=' || str[n] == NUL)
	    ಭಙ್ಗ;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ಪ್ರತ್ಯಾಗಮ xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_user(char_u *user, ಪೂರ್ಣಾಙ್ಕ need_copy)
{
    char_u	*user_copy = (user != ರಿಕ್ತ && need_copy)
						    ? vim_strsave(user) : user;

    ಯದಿ (user_copy == ರಿಕ್ತ || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ಯದಿ (need_copy)
	    vim_free(user_copy);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_users(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	lazy_init_done = FALSE;

    ಯದಿ (lazy_init_done)
	ಪ್ರತ್ಯಾಗಮ;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, ಪರಿಮಾಣ(char_u *), 20);

#ಯದಿ defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ಸಂರಚನಾ passwd*	pw;

	setpwent();
	ಯಾವತ್ ((pw = getpwent()) != ರಿಕ್ತ)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#ನೋಚೇದ್ಯದಿ defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ಯದಿ (NetUserEnum(ರಿಕ್ತ, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, ರಿಕ್ತ) == NERR_Success)
	{
	    ಪ್ರತಿ (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, ರಿಕ್ತ), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#ಯದಿ defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ಯದಿ (user_env != ರಿಕ್ತ && *user_env != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ	i;

	    ಪ್ರತಿ (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		ಯದಿ (STRCMP(local_user, user_env) == 0)
		    ಭಙ್ಗ;
	    }

	    ಯದಿ (i == ga_users.ga_len)
	    {
		ಸಂರಚನಾ passwd	*pw = getpwnam((ವರ್ಣ *)user_env);

		ಯದಿ (pw != ರಿಕ್ತ)
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
get_users(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    init_users();
    ಯದಿ (idx < ga_users.ga_len)
	ಪ್ರತ್ಯಾಗಮ ((char_u **)ga_users.ga_data)[idx];
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    ಪೂರ್ಣಾಙ್ಕ
match_user(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name);
    ಪೂರ್ಣಾಙ್ಕ result = 0;

    init_users();
    ಪ್ರತಿ (i = 0; i < ga_users.ga_len; i++)
    {
	ಯದಿ (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    ಪ್ರತ್ಯಾಗಮ 2; // full match
	ಯದಿ (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_to_exit(ಶೂನ್ಯ)
{
#ಯದಿ defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    ಅನ್ಯಥಾ
#endif
    {
	windgoto((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ಯದಿ (!full_screen)
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
    ಶೂನ್ಯ
preserve_exit(ಶೂನ್ಯ)
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
	ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ && buf->b_ml.ml_mfp->mf_fname != ರಿಕ್ತ)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    ಭಙ್ಗ;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	breakcheck_ಎಣಿಕೆ = 0;

    ಶೂನ್ಯ
ಸಾಲು_breakcheck(ಶೂನ್ಯ)
{
    ಯದಿ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP)
    {
	breakcheck_ಎಣಿಕೆ = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    ಶೂನ್ಯ
fast_breakcheck(ಶೂನ್ಯ)
{
    ಯದಿ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_ಎಣಿಕೆ = 0;
	ui_breakcheck();
    }
}

#ಯದಿ defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    ಶೂನ್ಯ
veryfast_breakcheck(ಶೂನ್ಯ)
{
    ಯದಿ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_ಎಣಿಕೆ = 0;
	ui_breakcheck();
    }
}
#endif

#ಯದಿ defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    ಪೂರ್ಣಾಙ್ಕ		flags,		// can be SHELL_SILENT
    ಪೂರ್ಣಾಙ್ಕ		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i = 0;
    FILE	*fd;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ಯದಿ ((tempname = vim_tempname('o', TRUE)) == ರಿಕ್ತ)
    {
	emsg(_(e_cant_get_temp_file_name));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ಯದಿ (command == ರಿಕ್ತ)
	ಗಚ್ಛ done;

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
    fd = mch_fopen((ವರ್ಣ *)tempname, "r");
# ಅನ್ಯಥಾ
    fd = mch_fopen((ವರ್ಣ *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ಯದಿ (fd == ರಿಕ್ತ
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ಯದಿ (fd != ರಿಕ್ತ)
	    fclose(fd);
	ಗಚ್ಛ done;
    }

    buffer = alloc(len + 1);
    ಯದಿ (buffer != ರಿಕ್ತ)
	i = (ಪೂರ್ಣಾಙ್ಕ)fread((ವರ್ಣ *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ಯದಿ (buffer == ರಿಕ್ತ)
	ಗಚ್ಛ done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ಯದಿ (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    ಅನ್ಯಥಾ ಯದಿ (ret_len == ರಿಕ್ತ)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ಯದಿ (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    ಅನ್ಯಥಾ
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    ಪ್ರತ್ಯಾಗಮ buffer;
}

# ಯದಿ defined(FEAT_EVAL)

    ಸ್ಥಿರ ಶೂನ್ಯ
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		retlist)
{
    char_u	*res = ರಿಕ್ತ;
    char_u	*p;
    char_u	*infile = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		err = FALSE;
    FILE	*fd;
    list_T	*list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		flags = SHELL_SILENT;
    ಪೂರ್ಣಾಙ್ಕ		use_argv = FALSE;
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
    ಯದಿ (check_restricted() || check_secure())
	ಗಚ್ಛ errret;

    ಯದಿ (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 1)
								      == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ಯದಿ ((infile = vim_tempname('i', TRUE)) == ರಿಕ್ತ)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ಗಚ್ಛ errret;
	}

	fd = mch_fopen((ವರ್ಣ *)infile, WRITEBIN);
	ಯದಿ (fd == ರಿಕ್ತ)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    ಗಚ್ಛ errret;
	}
	ಯದಿ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_ಸಂಖ್ಯೆ);
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_ಸಂಖ್ಯೆ);
		fclose(fd);
		ಗಚ್ಛ errret;
	    }

	    ಪ್ರತಿ (lnum = 1; lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; lnum++)
	    {
		ಪ್ರತಿ (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ಯದಿ (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			ಭಙ್ಗ;
		    }
		ಯದಿ (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_LIST)
	{
	    ಯದಿ (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		fclose(fd);
		ಗಚ್ಛ errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ಯದಿ (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ಯದಿ (fclose(fd) != 0)
	    err = TRUE;
	ಯದಿ (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    ಗಚ್ಛ errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ಯದಿ (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ಯದಿ (l == ರಿಕ್ತ || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    ಗಚ್ಛ errret;
	}
	ಯದಿ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ಗಚ್ಛ errret;
	ಯದಿ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ಗಚ್ಛ errret;
	}
	use_argv = TRUE;

	ಯದಿ (p_verbose > 3)
	{
	    ಪೂರ್ಣಾಙ್ಕ		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    ಪ್ರತಿ (i = 0; i < argc; ++i)
	    {
		ಯದಿ (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (ವರ್ಣ *)ga.ga_data);
	    ಸಂದೇಶ_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ಯದಿ (!ಸಂದೇಶ_silent)
	flags += SHELL_COOKED;

    ಯದಿ (retlist)
    {
	ಪೂರ್ಣಾಙ್ಕ		len;
	listitem_T	*li;
	char_u		*s = ರಿಕ್ತ;
	char_u		*start;
	char_u		*end;
	ಪೂರ್ಣಾಙ್ಕ		i;

	ಯದಿ (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	ಅನ್ಯಥಾ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ಯದಿ (res == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;

	list = list_alloc();
	ಯದಿ (list == ರಿಕ್ತ)
	    ಗಚ್ಛ errret;

	ಪ್ರತಿ (i = 0; i < len; ++i)
	{
	    start = res + i;
	    ಯಾವತ್ (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    ಯದಿ (s == ರಿಕ್ತ)
		ಗಚ್ಛ errret;

	    ಪ್ರತಿ (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    ಯದಿ (li == ರಿಕ್ತ)
	    {
		vim_free(s);
		ಗಚ್ಛ errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ರಿಕ್ತ);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ಯದಿ (res != ರಿಕ್ತ)
	{
	    char_u	*s, *d;

	    d = res;
	    ಪ್ರತಿ (s = res; *s; ++s)
	    {
		ಯದಿ (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ರಿಕ್ತ;
    }

errret:
    ಯದಿ (argv != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ (i = 0; argv[i] != ರಿಕ್ತ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ಯದಿ (infile != ರಿಕ್ತ)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ಯದಿ (res != ರಿಕ್ತ)
	vim_free(res);
    ಯದಿ (list != ರಿಕ್ತ)
	list_free(list);
}

/*
 * "system()" function
 */
    ಶೂನ್ಯ
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಙ್ಕ
goto_im(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(ಶೂನ್ಯ)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ಅನ್ಯಥಾ
    p = skiptowhite(p_sh);
    ಯದಿ (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ಅನ್ಯಥಾ
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	ಪ್ರತಿ (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ಯದಿ (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    ಪೂರ್ಣಾಙ್ಕ
path_is_url(char_u *p)
{
    ಯದಿ (STRNCMP(p, "://", (size_t)3) == 0)
	ಪ್ರತ್ಯಾಗಮ URL_SLASH;
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ಪ್ರತ್ಯಾಗಮ URL_BACKSLASH;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ಯದಿ (!ASCII_ISALPHA(*fname))
	ಪ್ರತ್ಯಾಗಮ 0;

    // check body: alpha or dash
    ಪ್ರತಿ (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ಯದಿ (p[-1] == '-')
	ಪ್ರತ್ಯಾಗಮ 0;

    // "://" or ":\\" must follow
    ಪ್ರತ್ಯಾಗಮ path_is_url(p);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ಯದಿ (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ಅನ್ಯಥಾ
	sve->sve_did_save = FALSE;
    ಪ್ರತ್ಯಾಗಮ v_event;
}

    ಶೂನ್ಯ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ಯದಿ (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ಅನ್ಯಥಾ
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    ಶೂನ್ಯ
may_trigger_modechanged(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ಯದಿ (!has_modechanged() || got_int)
	ಪ್ರತ್ಯಾಗಮ;

    get_mode(curr_mode);
    ಯದಿ (STRCMP(curr_mode, last_mode) == 0)
	ಪ್ರತ್ಯಾಗಮ;

    v_event = get_v_event(&save_v_event);
    (ಶೂನ್ಯ)dict_add_string(v_event, "new_mode", curr_mode);
    (ಶೂನ್ಯ)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_ವಸ್ತುಗಳು_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((ವರ್ಣ *)pattern_buf, ಪರಿಮಾಣ(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, ರಿಕ್ತ, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    ಪೂರ್ಣಾಙ್ಕ
vim_append_digit_long(long *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ digit)
{
    long x = *ಮೌಲ್ಯ;
    ಯದಿ (x > ((LONG_MAX - (long)digit) / 10))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *ಮೌಲ್ಯ = x * 10 + (long)digit;
    ಪ್ರತ್ಯಾಗಮ OK;
}

// Return something that fits into an int.
    ಪೂರ್ಣಾಙ್ಕ
trim_to_int(vimlong_T x)
{
    ಪ್ರತ್ಯಾಗಮ x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
