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

#ಆದರೆ defined(__HAIKU__)
# include <storage/FindDirectory.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
# include <lm.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು URL_SLASH	1		// path_is_url() has found "://"
#ವ್ಯಾಖ್ಯಾನಿಸು URL_BACKSLASH	2		// path_is_url() has found ":\\"

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
    ಪೂರ್ಣಾಂಕ
get_leader_len(
    char_u	*ಸಾಲು,
    char_u	**flags,
    ಪೂರ್ಣಾಂಕ		backward,
    ಪೂರ್ಣಾಂಕ		include_space)
{
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		got_com = FALSE;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    ಪೂರ್ಣಾಂಕ		ಮಧ್ಯ_ಹೊಂದಿಕೆ_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ಖಾಲಿ;

    ಫಲಿತಾಂಶ = i = 0;
    ಆಗ (VIM_ISWHITE(ಸಾಲು[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    ಆಗ (ಸಾಲು[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ಸಿಕ್ಕಿತು_one = FALSE;
	ಗಾಗಿ (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ಆದರೆ (!got_com && flags != ಖಾಲಿ)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (ಶೂನ್ಯ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ಆದರೆ (string == ಖಾಲಿ)	    // missing ':', ignore this part
		ಮುಂದುವರಿ;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ಆದರೆ (ಮಧ್ಯ_ಹೊಂದಿಕೆ_len != 0
		    && vim_strchr(part_buf, COM_ಮಧ್ಯ) == ಖಾಲಿ
		    && vim_strchr(part_buf, COM_ಕೊನೆ) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ಆದರೆ (got_com && vim_strchr(part_buf, COM_NEST) == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    // When 'O' flag present and using "O" command skip this one.
	    ಆದರೆ (backward && vim_strchr(part_buf, COM_NOBACK) != ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ಆದರೆ (VIM_ISWHITE(string[0]))
	    {
		ಆದರೆ (i == 0 || !VIM_ISWHITE(ಸಾಲು[i - 1]))
		    ಮುಂದುವರಿ;  // missing white space
		ಆಗ (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    ಗಾಗಿ (j = 0; string[j] != NUL && string[j] == ಸಾಲು[i + j]; ++j)
		;
	    ಆದರೆ (string[j] != NUL)
		ಮುಂದುವರಿ;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ಆದರೆ (vim_strchr(part_buf, COM_BLANK) != ಖಾಲಿ
			   && !VIM_ISWHITE(ಸಾಲು[i + j]) && ಸಾಲು[i + j] != NUL)
		ಮುಂದುವರಿ;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ಆದರೆ (vim_strchr(part_buf, COM_ಮಧ್ಯ) != ಖಾಲಿ)
	    {
		ಆದರೆ (ಮಧ್ಯ_ಹೊಂದಿಕೆ_len == 0)
		{
		    ಮಧ್ಯ_ಹೊಂದಿಕೆ_len = j;
		    saved_flags = prev_list;
		}
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (ಮಧ್ಯ_ಹೊಂದಿಕೆ_len != 0 && j > ಮಧ್ಯ_ಹೊಂದಿಕೆ_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		ಮಧ್ಯ_ಹೊಂದಿಕೆ_len = 0;

	    ಆದರೆ (ಮಧ್ಯ_ಹೊಂದಿಕೆ_len == 0)
		i += j;
	    ಸಿಕ್ಕಿತು_one = TRUE;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (ಮಧ್ಯ_ಹೊಂದಿಕೆ_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ಆದರೆ (!got_com && flags != ಖಾಲಿ)
		*flags = saved_flags;
	    i += ಮಧ್ಯ_ಹೊಂದಿಕೆ_len;
	    ಸಿಕ್ಕಿತು_one = TRUE;
	}

	// No match found, stop scanning.
	ಆದರೆ (!ಸಿಕ್ಕಿತು_one)
	    ನಿಲ್ಲಿಸು;

	ಫಲಿತಾಂಶ = i;

	// Include any trailing white space.
	ಆಗ (VIM_ISWHITE(ಸಾಲು[i]))
	    ++i;

	ಆದರೆ (include_space)
	    ಫಲಿತಾಂಶ = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ಆದರೆ (vim_strchr(part_buf, COM_NEST) == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    ಪೂರ್ಣಾಂಕ
get_ಕೊನೆಯ_leader_ಸ್ಥಾನಾಂತರ(char_u *ಸಾಲು, char_u **flags)
{
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = -1;
    ಪೂರ್ಣಾಂಕ		i, j;
    ಪೂರ್ಣಾಂಕ		ಅಧೋ_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು);
    ಆಗ (--i >= ಅಧೋ_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ಸಿಕ್ಕಿತು_one = FALSE;
	ಗಾಗಿ (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (ಶೂನ್ಯ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ಆದರೆ (string == ಖಾಲಿ)	// If everything is fine, this cannot actually
				// happen.
		ಮುಂದುವರಿ;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ಆದರೆ (VIM_ISWHITE(string[0]))
	    {
		ಆದರೆ (i == 0 || !VIM_ISWHITE(ಸಾಲು[i - 1]))
		    ಮುಂದುವರಿ;
		ಆಗ (VIM_ISWHITE(*string))
		    ++string;
	    }
	    ಗಾಗಿ (j = 0; string[j] != NUL && string[j] == ಸಾಲು[i + j]; ++j)
		/* do nothing */;
	    ಆದರೆ (string[j] != NUL)
		ಮುಂದುವರಿ;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ಆದರೆ (vim_strchr(part_buf, COM_BLANK) != ಖಾಲಿ
		    && !VIM_ISWHITE(ಸಾಲು[i + j]) && ಸಾಲು[i + j] != NUL)
		ಮುಂದುವರಿ;

	    ಆದರೆ (vim_strchr(part_buf, COM_ಮಧ್ಯ) != ಖಾಲಿ)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		ಗಾಗಿ (j = 0; VIM_ISWHITE(ಸಾಲು[j]) && j <= i; j++)
		    ;
		ಆದರೆ (j < i)
		    ಮುಂದುವರಿ;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    ಸಿಕ್ಕಿತು_one = TRUE;

	    ಆದರೆ (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (ಸಿಕ್ಕಿತು_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    ಪೂರ್ಣಾಂಕ     len1, len2, off;

	    ಫಲಿತಾಂಶ = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ಆದರೆ (vim_strchr(part_buf, COM_NEST) != ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಅಧೋ_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    ಆಗ (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (ಪೂರ್ಣಾಂಕ)STRLEN(com_leader);

	    ಗಾಗಿ (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(ಶೂನ್ಯ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ಆದರೆ (flags_save == com_flags)
		    ಮುಂದುವರಿ;
		string = vim_strchr(part_buf2, ':');
		++string;
		ಆಗ (VIM_ISWHITE(*string))
		    ++string;
		len2 = (ಪೂರ್ಣಾಂಕ)STRLEN(string);
		ಆದರೆ (len2 == 0)
		    ಮುಂದುವರಿ;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		ಗಾಗಿ (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ಆದರೆ (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ಆದರೆ (i - off < ಅಧೋ_check_bound)
			    ಅಧೋ_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    ಪೂರ್ಣಾಂಕ
plines(linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ plines_win(curwin, lnum, TRUE);
}

    ಪೂರ್ಣಾಂಕ
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ_winheight)	// when TRUE limit to window height
{
#ಆದರೆ defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ಹಿಂತಿರುಗಿ plines_win_nofill(wp, lnum, ಎಲ್ಲೆ_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    ಪೂರ್ಣಾಂಕ
plines_nofill(linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ plines_win_nofill(curwin, lnum, TRUE);
}

    ಪೂರ್ಣಾಂಕ
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		ಎಲ್ಲೆ_winheight)	// when TRUE limit to window height
{
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು;

    ಆದರೆ (wp->w_ಅಗಲ == 0)
	ಹಿಂತಿರುಗಿ 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ಆದರೆ (ಸಾಲು_Folded(wp, lnum) == TRUE)
	ಹಿಂತಿರುಗಿ 1;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!wp->w_p_wrap)
	ಸಾಲುಗಳು = 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_ಎಣಿಕೆ_above_ಕೆಳಗೆ(wp->w_ಬಫರ್, lnum)
#ಆದರೆ_ಅಂತ್ಯ
	;
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಾಲುಗಳು = plines_win_nofold(wp, lnum);

    ಆದರೆ (ಎಲ್ಲೆ_winheight && ಸಾಲುಗಳು > wp->w_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ wp->w_ಎತ್ತರ;
    ಹಿಂತಿರುಗಿ ಸಾಲುಗಳು;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    ಪೂರ್ಣಾಂಕ
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    ದೀರ್ಘ	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ಆದರೆ (*s == NUL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_ಪಠ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಹಿಂತಿರುಗಿ 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ಆದರೆ (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	ಲಂಬಸಾಲು += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    ಅಗಲ = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
    ಆದರೆ (ಅಗಲ <= 0)
	ಹಿಂತಿರುಗಿ 32000;
    ಆದರೆ (ಲಂಬಸಾಲು <= ಅಗಲ)
	ಹಿಂತಿರುಗಿ 1;
    ಲಂಬಸಾಲು -= ಅಗಲ;
    ಅಗಲ += win_ಲಂಬಸಾಲು_off2(wp);
    ಹಿಂತಿರುಗಿ (ಲಂಬಸಾಲು + (ಅಗಲ - 1)) / ಅಗಲ + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    ಪೂರ್ಣಾಂಕ
plines_win_ಲಂಬಸಾಲು(win_T *wp, linenr_T lnum, ದೀರ್ಘ ಸ್ತಂಭ)
{
    ದೀರ್ಘ	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಸಾಲುಗಳು = 0;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    char_u	*ಸಾಲು;
    chartabsize_T cts;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ಸಾಲುಗಳು = diff_check_fill(wp, lnum);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!wp->w_p_wrap)
	ಹಿಂತಿರುಗಿ ಸಾಲುಗಳು + 1;

    ಆದರೆ (wp->w_ಅಗಲ == 0)
	ಹಿಂತಿರುಗಿ ಸಾಲುಗಳು + 1;

    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
    // "column" is a byte index, advance the pointer until it is reached.
    ಆಗ (*cts.cts_ptr != NUL && cts.cts_ptr < ಸಾಲು + ಸ್ತಂಭ)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ಖಾಲಿ, ಖಾಲಿ);
	MB_PTR_ADV(cts.cts_ptr);
    }

    /*
     * If *cts.cts_ptr is a TAB, and the TAB is not displayed as ^I, and we're
     * not in MODE_INSERT state, then col must be adjusted so that it
     * represents the last screen position of the TAB.  This only fixes an
     * error when the TAB wraps from one screen line to the next (when
     * 'columns' is not a multiple of 'ts') -- webb.
     */
    ಲಂಬಸಾಲು = cts.cts_vcol;
    ಆದರೆ (*cts.cts_ptr == TAB && (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	ಲಂಬಸಾಲು += win_lbr_chartabsize(&cts, ಖಾಲಿ, ಖಾಲಿ) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    ಅಗಲ = wp->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(wp);
    ಆದರೆ (ಅಗಲ <= 0)
	ಹಿಂತಿರುಗಿ 9999;

    ಸಾಲುಗಳು += 1;
    ಆದರೆ (ಲಂಬಸಾಲು > ಅಗಲ)
	ಸಾಲುಗಳು += (ಲಂಬಸಾಲು - ಅಗಲ) / (ಅಗಲ + win_ಲಂಬಸಾಲು_off2(wp)) + 1;
    ಹಿಂತಿರುಗಿ ಸಾಲುಗಳು;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    ಪೂರ್ಣಾಂಕ
plines_m_win(win_T *wp, linenr_T ಮೊದಲ, linenr_T ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ max)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಆಗ (ಮೊದಲ <= ಕೊನೆಯ && ಎಣಿಕೆ_2 < max)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಪೂರ್ಣಾಂಕ	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_ಎಣಿಕೆ(wp, ಮೊದಲ, ಖಾಲಿ);
	ಆದರೆ (x > 0)
	{
	    ++ಎಣಿಕೆ_2;	    // count 1 for "+-- folded" line
	    ಮೊದಲ += x;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (ಮೊದಲ == wp->w_topline)
		ಎಣಿಕೆ_2 += plines_win_nofill(wp, ಮೊದಲ, FALSE) + wp->w_topfill;
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಎಣಿಕೆ_2 += plines_win(wp, ಮೊದಲ, FALSE);
	    ++ಮೊದಲ;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (ಮೊದಲ == wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಎಣಿಕೆ_2 += diff_check_fill(wp, ಮೊದಲ);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ MIN(max, ಎಣಿಕೆ_2);
}

    ಪೂರ್ಣಾಂಕ
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ಆದರೆ (pos->ಲಂಬಸಾಲು == MAXCOL)
	ಹಿಂತಿರುಗಿ NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ಆದರೆ (pos->ಲಂಬಸಾಲು > ptrlen)
	ಹಿಂತಿರುಗಿ NUL;
    ಆದರೆ (has_mbyte)
	ಹಿಂತಿರುಗಿ (*mb_ptr2char)(ptr);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)*ptr;
}

    ಪೂರ್ಣಾಂಕ
gchar_cursor(ಶೂನ್ಯ)
{
    ಆದರೆ (has_mbyte)
	ಹಿಂತಿರುಗಿ (*mb_ptr2char)(ml_get_cursor());
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    ಪೂರ್ಣಾಂಕ
char_before_cursor(ಶೂನ್ಯ)
{
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
	ಹಿಂತಿರುಗಿ -1;

    char_u *ಸಾಲು = ml_get_curline();

    ಆದರೆ (has_mbyte)
    {
	char_u *p = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ prev_len = (*mb_ತಲೆ_off)(ಸಾಲು, p - 1) + 1;
	ಹಿಂತಿರುಗಿ mb_ptr2char(p - prev_len);
    }
    ಹಿಂತಿರುಗಿ ಸಾಲು[curwin->w_cursor.ಲಂಬಸಾಲು - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    ಶೂನ್ಯ
pchar_cursor(ಪೂರ್ಣಾಂಕ c)
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE)
						  + curwin->w_cursor.ಲಂಬಸಾಲು) = c;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_ಗಮ್ಯ_option_part(char_u *p)
{
    ಆದರೆ (*p == ',')
	++p;
    ಆಗ (*p == ' ')
	++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    ಶೂನ್ಯ
check_ಸ್ಥಿತಿ(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf && wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ)
	{
	    wp->w_redr_ಸ್ಥಿತಿ = true;
	    ಗಣ_must_redraw(UPD_VALID);
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
    ಪೂರ್ಣಾಂಕ
ask_yesno(char_u *str, ಪೂರ್ಣಾಂಕ ನೇರ)
{
    ಪೂರ್ಣಾಂಕ	    r = ' ';
    ಪೂರ್ಣಾಂಕ	    save_State = State;

    ಆದರೆ (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
    State = ಬಹುಲಕ_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;		// no mapping here, but recognize keys

    ಆಗ (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ಆದರೆ (ನೇರ)
	    r = get_keystroke();
	ಇಲ್ಲದಿದ್ದರೆ
	    r = plain_vgetc();
	ಆದರೆ (r == Ctrl_C || r == ESC)
	    r = 'n';
	ಸಂದೇಶ_putchar(r);	    // show what you typed
	ಹೊರಹೋಗು_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

    ಹಿಂತಿರುಗಿ r;
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    ಶೂನ್ಯ
get_ಬಹುಲಕ(char_u *buf)
{
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಆದರೆ (ಸಮಯ_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_use_loop())
    {
	ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE
						      || State == ಬಹುಲಕ_SETWSIZE
		|| State == ಬಹುಲಕ_CONFIRM)
    {
	buf[i++] = 'r';
	ಆದರೆ (State == ಬಹುಲಕ_ASKMORE)
	    buf[i++] = 'm';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_CONFIRM)
	    buf[i++] = '?';
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_EXTERNCMD)
	buf[i++] = '!';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
    {
	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ಇಲ್ಲದಿದ್ದರೆ
		buf[i++] = 'i';
	}

	ಆದರೆ (ins_compl_ಸಕ್ರಿಯ())
	    buf[i++] = 'c';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ctrl_x_ಬಹುಲಕ_not_defined_yet())
	    buf[i++] = 'x';
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) || exmode_ಸಕ್ರಿಯ)
    {
	buf[i++] = 'c';
	ಆದರೆ (exmode_ಸಕ್ರಿಯ == EXMODE_VIM)
	    buf[i++] = 'v';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (exmode_ಸಕ್ರಿಯ == EXMODE_ಸಾಮಾನ್ಯ)
	    buf[i++] = 'e';
	ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (VIsual_select)
	    buf[i++] = VIsual_ಬಹುಲಕ + 's' - 'v';
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buf[i++] = VIsual_ಬಹುಲಕ;
	    ಆದರೆ (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[i++] = 'n';
	ಆದರೆ (ಮುಕ್ತಾಯ_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪದ_ಒಳ_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    ಶೂನ್ಯ
f_ಬಹುಲಕ(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ];

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    get_ಬಹುಲಕ(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ಆದರೆ (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_ಕೂಡಿಸು_state_char(garray_T *gap, char_u *include, ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (include == ಖಾಲಿ || vim_strchr(include, c) != ಖಾಲಿ)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    ಶೂನ್ಯ
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ga_init2(&ga, 1, 20);
    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ಆದರೆ (!(stuff_ಖಾಲಿಗಳು() && typebuf.tb_len == 0 && scriptin[curscript] == ಖಾಲಿ))
	may_ಕೂಡಿಸು_state_char(&ga, include, 'm');
    ಆದರೆ (op_pending())
	may_ಕೂಡಿಸು_state_char(&ga, include, 'o');
    ಆದರೆ (autocmd_busy)
	may_ಕೂಡಿಸು_state_char(&ga, include, 'x');
    ಆದರೆ (ins_compl_ಸಕ್ರಿಯ())
	may_ಕೂಡಿಸು_state_char(&ga, include, 'a');

# ifdef FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (channel_ಒಳ_blocking_wait())
	may_ಕೂಡಿಸು_state_char(&ga, include, 'w');
# endif
    ಆದರೆ (!get_was_ಸುರಕ್ಷಿತ_state())
	may_ಕೂಡಿಸು_state_char(&ga, include, 'S');
    ಗಾಗಿ (i = 0; i < get_callback_ಆಳ() && i < 3; ++i)
	may_ಕೂಡಿಸು_state_char(&ga, include, 'c');
    ಆದರೆ (ಸಂದೇಶ_scrolled > 0)
	may_ಕೂಡಿಸು_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

/*
 * Get a key stroke directly from the user.
 * Ignores mouse clicks and scrollbar events, except a click for the left
 * button (used at the more prompt).
 * Doesn't use vgetc(), because it syncs undo and eats mapped characters.
 * Disadvantage: typeahead is ignored.
 * Translates the interrupt character for unix to ESC.
 */
    ಪೂರ್ಣಾಂಕ
get_keystroke(ಶೂನ್ಯ)
{
    char_u	*buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		buflen = 150;
    ಪೂರ್ಣಾಂಕ		maxlen;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		save_mapped_ctrl_c = mapped_ctrl_c;
    ಪೂರ್ಣಾಂಕ		waited = 0;

    mod_ಮುಖವಾಡ = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    ಗಾಗಿ (;;)
    {
	cursor_on();
	ಹೊರಹೋಗು_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ಆದರೆ (buf == ಖಾಲಿ)
	    buf = alloc(buflen);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ಆದರೆ (buf == ಖಾಲಿ)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    do_outofmem_ಸಂದೇಶ((long_u)buflen);
	    ಹಿಂತಿರುಗಿ ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ಆದರೆ (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_ಒಳಹರಿವು_ಬಫರ್(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ಆದರೆ ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ಮುಂದುವರಿ;

	ಆದರೆ (n == KEYLEN_REMOVED)  // key code removed
	{
	    ಆದರೆ (must_redraw != 0 && !need_wait_return && (State
			& (ಬಹುಲಕ_CMDLINE | ಬಹುಲಕ_HITRETURN | ಬಹುಲಕ_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (n > 0)		// found a termcode: adjust length
	    len = n;
	ಆದರೆ (len == 0)		// nothing typed yet
	    ಮುಂದುವರಿ;

	// Handle modifier and/or special key code.
	n = buf[0];
	ಆದರೆ (n == K_SPECIAL)
	{
	    n = ಗಮ್ಯ_SPECIAL(buf[1], buf[2]);
	    ಆದರೆ (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ಕೀಲಿ(n) && n != K_LEFTMOUSE)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		ಆದರೆ (buf[1] == KS_MODIFIER)
		    mod_ಮುಖವಾಡ = buf[2];
		len -= 3;
		ಆದರೆ (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ಮುಂದುವರಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (has_mbyte)
	{
	    ಆದರೆ (MB_BYTE2LEN(n) > len)
		ಮುಂದುವರಿ;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (n == intr_char)
	    n = ESC;
#ಆದರೆ_ಅಂತ್ಯ
	ನಿಲ್ಲಿಸು;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ಹಿಂತಿರುಗಿ ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(n, &mod_ಮುಖವಾಡ);
}

// For overflow detection, add a digit safely to an int value.
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim_append_digit_int(ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ digit)
{
    ಪೂರ್ಣಾಂಕ x = *ಮೌಲ್ಯ;
    ಆದರೆ (x > ((ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - digit) / 10))
	ಹಿಂತಿರುಗಿ FAIL;
    *ಮೌಲ್ಯ = x * 10 + digit;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    ಪೂರ್ಣಾಂಕ
get_ಸಂಖ್ಯೆ(
    ಪೂರ್ಣಾಂಕ	    colon,			// allow colon to abort
    ಪೂರ್ಣಾಂಕ	    *mouse_ಬಳಸಿದ)
{
    ಪೂರ್ಣಾಂಕ	n = 0;
    ಪೂರ್ಣಾಂಕ	c;
    ಪೂರ್ಣಾಂಕ typed = 0;

    ಆದರೆ (mouse_ಬಳಸಿದ != ಖಾಲಿ)
	*mouse_ಬಳಸಿದ = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ಆದರೆ (ಸಂದೇಶ_silent != 0)
	ಹಿಂತಿರುಗಿ 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;		// no mapping here, but recognize keys
    ಗಾಗಿ (;;)
    {
	windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
	c = ಸುರಕ್ಷಿತ_vgetc();
	ಆದರೆ (VIM_ISDIGIT(c))
	{
	    ಆದರೆ (vim_append_digit_int(&n, c - '0') == FAIL)
		ಹಿಂತಿರುಗಿ 0;
	    ಸಂದೇಶ_putchar(c);
	    ++typed;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ಆದರೆ (typed > 0)
	    {
		ಸಂದೇಶ_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mouse_ಬಳಸಿದ != ಖಾಲಿ && c == K_LEFTMOUSE)
	{
	    *mouse_ಬಳಸಿದ = TRUE;
	    n = mouse_ಅಡ್ಡಸಾಲು + 1;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ಆದರೆ (!exmode_ಸಕ್ರಿಯ)
		cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CAR || c == NL )
	    ನಿಲ್ಲಿಸು;
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    ಹಿಂತಿರುಗಿ n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    ಪೂರ್ಣಾಂಕ
prompt_for_ಸಂಖ್ಯೆ(ಪೂರ್ಣಾಂಕ *mouse_ಬಳಸಿದ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		save_cmdline_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		save_State;

    // When using ":silent" assume that <CR> was entered.
    ಆದರೆ (mouse_ಬಳಸಿದ != ಖಾಲಿ)
	ಸಂದೇಶ_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
    cmdline_ಅಡ್ಡಸಾಲು = 0;
    save_State = State;
    State = ಬಹುಲಕ_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_ಸಂಖ್ಯೆ(TRUE, mouse_ಬಳಸಿದ);
    ಆದರೆ (ಕೀಲಿ_Typed)
    {
	// don't call wait_return() now
	ಆದರೆ (ಸಂದೇಶ_ಅಡ್ಡಸಾಲು > 0)
	    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು - 1;
	need_wait_return = FALSE;
	ಸಂದೇಶ_didany = FALSE;
	ಸಂದೇಶ_didout = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	cmdline_ಅಡ್ಡಸಾಲು = save_cmdline_ಅಡ್ಡಸಾಲು;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ಹಿಂತಿರುಗಿ i;
}

    ಶೂನ್ಯ
msgmore(ದೀರ್ಘ n)
{
    ದೀರ್ಘ pn;

    ಆದರೆ (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ಹಿಂತಿರುಗಿ;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ಆದರೆ (keep_ಸಂದೇಶ != ಖಾಲಿ && !keep_ಸಂದೇಶ_ಇನ್ನಷ್ಟು)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (n > 0)
	pn = n;
    ಇಲ್ಲದಿದ್ದರೆ
	pn = -n;

    ಆದರೆ (pn > p_report)
    {
	ಆದರೆ (n > 0)
	    vim_snprintf(ಸಂದೇಶ_buf, ಸಂದೇಶ_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_snprintf(ಸಂದೇಶ_buf, ಸಂದೇಶ_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ಆದರೆ (got_int)
	    vim_strcat((char_u *)ಸಂದೇಶ_buf, (char_u *)_(" (Interrupted)"),
								  ಸಂದೇಶ_BUF_LEN);
	ಆದರೆ (ಸಂದೇಶ(ಸಂದೇಶ_buf))
	{
	    ಗಣ_keep_ಸಂದೇಶ((char_u *)ಸಂದೇಶ_buf, 0);
	    keep_ಸಂದೇಶ_ಇನ್ನಷ್ಟು = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    ಶೂನ್ಯ
beep_flush(ಶೂನ್ಯ)
{
    ಆದರೆ (emsg_silent == 0)
    {
	flush_buffers(FLUSH_ಕನಿಷ್ಠತಮ);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    ಶೂನ್ಯ
vim_beep(ಚಿಹ್ನೆರಹಿತ val)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    called_vim_beep = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (emsg_silent != 0 || ಒಳ_assert_fails)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		did_init = FALSE;
	ಸ್ಥಿರ elapsed_T	ಆರಂಭ_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ಆದರೆ (!did_init || ELAPSED_FUNC(ಆರಂಭ_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (p_vb
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		    // While the GUI is starting up the termcap is set for
		    // the GUI but the output still goes to a terminal.
		    && !(gui.ಒಳ_use && gui.starting)
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		ಹೊರಹೋಗು_str_cf(T_VB);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VTP
		// No restore color information, refresh the screen.
		ಆದರೆ (has_vtp_working() != 0
# ifdef FEAT_TERMGUICOLORS
			&& (p_tgc || (!p_tgc && t_colors >= 256))
# endif
		   )
		{
		    redraw_later(UPD_CLEAR);
		    update_screen(0);
		    redrawcmd();
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹೊರಹೋಗು_char(BELL);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ಆದರೆ (vim_strchr(p_debug, 'e') != ಖಾಲಿ)
    {
	ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ಇಲ್ಲದಿದ್ದರೆ
    var = mch_getenv((char_u *)"HOME");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ಆದರೆ (var == ಖಾಲಿ || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ಆದರೆ (homepath == ಖಾಲಿ || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ಆದರೆ (homedrive != ಖಾಲಿ
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((ಅಕ್ಷರ *)NameBuff, "%s%s", homedrive, homepath);
	    ಆದರೆ (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ಆದರೆ (var == ಖಾಲಿ)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ಆದರೆ (var != ಖಾಲಿ && *var == '%')
    {
	char_u	*p;
	char_u	*ಘಾತಾಂಕ;

	p = vim_strchr(var + 1, '%');
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    ಘಾತಾಂಕ = mch_getenv(NameBuff);
	    ಆದರೆ (ಘಾತಾಂಕ != ಖಾಲಿ && *ಘಾತಾಂಕ != NUL
					&& STRLEN(ಘಾತಾಂಕ) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((ಅಕ್ಷರ *)NameBuff, MAXPATHL, "%s%s", ಘಾತಾಂಕ, p + 1);
		var = NameBuff;
	    }
	}
    }

    ಆದರೆ (var != ಖಾಲಿ && *var == NUL)	// empty is same as not set
	var = ಖಾಲಿ;

    ಆದರೆ (enc_utf8 && var != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	len;
	char_u  *pp = ಖಾಲಿ;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_ಗಮ್ಯ_enc(var, (ಪೂರ್ಣಾಂಕ)STRLEN(var), &pp, &len);
	ಆದರೆ (pp != ಖಾಲಿ)
	{
	    homedir = pp;
	    ಹಿಂತಿರುಗಿ;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ಆದರೆ (var == ಖಾಲಿ)
	var = (char_u *)"C:/";
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (var != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ಆದರೆ (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((ಅಕ್ಷರ *)NameBuff) == 0)
	{
	    ಆದರೆ (!mch_chdir((ಅಕ್ಷರ *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)NameBuff) != 0)
		emsg(_(e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory));
	}
#ಆದರೆ_ಅಂತ್ಯ
	homedir = vim_strsave(var);
    }
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_homedir(ಶೂನ್ಯ)
{
    vim_free(homedir);
}

    ಶೂನ್ಯ
free_users(ಶೂನ್ಯ)
{
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga_users);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    ಶೂನ್ಯ
init_vimdir(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ಆದರೆ (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ಆದರೆ (mustfree)
	vim_free(p);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(char_u *src)
{
    ಹಿಂತಿರುಗಿ expand_env_save_opt(src, FALSE, ಖಾಲಿ);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, ಪೂರ್ಣಾಂಕ one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ಆದರೆ (p != ಖಾಲಿ)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ p;
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
    ಪೂರ್ಣಾಂಕ		dstlen)		// maximum length of the result
{
    ಹಿಂತಿರುಗಿ expand_env_esc(src, dst, dstlen, ಖಾಲಿ, FALSE, ಖಾಲಿ);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    ಪೂರ್ಣಾಂಕ		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    ಪೂರ್ಣಾಂಕ		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*ಬಾಲ;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*var;
    ಪೂರ್ಣಾಂಕ		copy_char;
    ಪೂರ್ಣಾಂಕ		mustfree;	// var was allocated, need to free it later
    ಪೂರ್ಣಾಂಕ		at_ಆರಂಭ = TRUE; // at start of a name
    ಪೂರ್ಣಾಂಕ		startstr_len = 0;
    char_u	*dst_ಆರಂಭ = dst;

    ಆದರೆ (startstr != ಖಾಲಿ)
	startstr_len = (ಪೂರ್ಣಾಂಕ)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    ಆಗ (*src && dstlen > 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Skip over `=expr`.
	ಆದರೆ (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&src, ಖಾಲಿ);
	    ಆದರೆ (*src == '`')
		++src;
	    len = src - var;
	    ಆದರೆ (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (ಪೂರ್ಣಾಂಕ)len;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	copy_char = TRUE;
	ಆದರೆ ((*src == '$'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		    && at_ಆರಂಭ
#ಆದರೆ_ಅಂತ್ಯ
	   )
#ಆದರೆ defined(MSWIN)
		|| *src == '%'
#ಆದರೆ_ಅಂತ್ಯ
		|| (*src == '~' && at_ಆರಂಭ))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ಆದರೆ (*src != '~')				// environment var
	    {
		ಬಾಲ = src + 1;
		var = dst;
		c = dstlen - 1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		// Unix has ${var-name} type environment vars
		ಆದರೆ (*ಬಾಲ == '{' && !vim_isIDc('{'))
		{
		    ಬಾಲ++;	// ignore '{'
		    ಆಗ (c-- > 0 && *ಬಾಲ && *ಬಾಲ != '}')
			*var++ = *ಬಾಲ++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ಆಗ (c-- > 0 && *ಬಾಲ != NUL && ((vim_isIDc(*ಬಾಲ))
#ಆದರೆ defined(MSWIN)
			    || (*src == '%' && *ಬಾಲ != '%')
#ಆದರೆ_ಅಂತ್ಯ
			    ))
			*var++ = *ಬಾಲ++;
		}

#ಆದರೆ defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ಆದರೆ (src[1] == '{' && *ಬಾಲ != '}')
# ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (*src == '%' && *ಬಾಲ != '%')
# endif
		    var = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
# ifdef UNIX
		    ಆದರೆ (src[1] == '{')
# ಇಲ್ಲದಿದ್ದರೆ
		    ಆದರೆ (*src == '%')
# endif
			++ಬಾಲ;
#ಆದರೆ_ಅಂತ್ಯ
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ಆದರೆ defined(MSWIN) || defined(UNIX)
		}
#ಆದರೆ_ಅಂತ್ಯ
	    }
							// home directory
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ಖಾಲಿ)
	    {
		var = homedir;
		ಬಾಲ = src + 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ					// user directory
	    {
#ಆದರೆ defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		ಬಾಲ = src;
		var = dst;
		c = dstlen - 1;
		ಆಗ (	   c-- > 0
			&& *ಬಾಲ
			&& vim_isfilec(*ಬಾಲ)
			&& !vim_ispathsep(*ಬಾಲ))
		    *var++ = *ಬಾಲ++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  ಆದರೆ defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ರಚನೆ passwd *pw = (*dst == NUL)
					? ಖಾಲಿ : getpwnam((ಅಕ್ಷರ *)dst + 1);

		    var = (pw == ಖಾಲಿ) ? ಖಾಲಿ : (char_u *)pw->pw_dir;
		}
		ಆದರೆ (var == ಖಾಲಿ)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ಖಾಲಿ,
				WILD_ಕೂಡಿಸು_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ಇಲ್ಲದಿದ್ದರೆ	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*ಮಾರ್ಗ, *ಮುಂದಿನ_ಮಾರ್ಗ, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    ಮುಂದಿನ_ಮಾರ್ಗ = paths;
		    ಆಗ (*ಮುಂದಿನ_ಮಾರ್ಗ)
		    {
			ಗಾಗಿ (ಮಾರ್ಗ = ಮುಂದಿನ_ಮಾರ್ಗ; *ಮುಂದಿನ_ಮಾರ್ಗ && *ಮುಂದಿನ_ಮಾರ್ಗ != ',';
				ಮುಂದಿನ_ಮಾರ್ಗ++);
			ಆದರೆ (*ಮುಂದಿನ_ಮಾರ್ಗ)
			    *ಮುಂದಿನ_ಮಾರ್ಗ++ = NUL;
			testlen = vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)test,
			    ಗಾತ್ರ(test),
			    "%s/%s",
			    ಮಾರ್ಗ,
			    dst + 1);
			ಆದರೆ (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ಆದರೆ (var != ಖಾಲಿ)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    ನಿಲ್ಲಿಸು;
			}
		    }
		}
# endif // UNIX
#ಇಲ್ಲದಿದ್ದರೆ
		// cannot expand user's home directory, so don't try
		var = ಖಾಲಿ;
		ಬಾಲ = (char_u *)"";	// for gcc
#ಆದರೆ_ಅಂತ್ಯ // UNIX || VMS
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ಆದರೆ (p_ssl && var != ಖಾಲಿ && vim_strchr(var, '\\') != ಖಾಲಿ)
	    {
		char_u	*p = vim_strsave(var);

		ಆದರೆ (p != ಖಾಲಿ)
		{
		    ಆದರೆ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		    forward_slash(var);
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // If "var" contains any character from "esc_chars", escape it
	    // with a backslash.  The historical use is escaping spaces so
	    // that ":e ~/tt" works when $HOME contains a space.
	    ಆದರೆ (esc_chars != ಖಾಲಿ && var != ಖಾಲಿ
		    && vim_strpbrk(var, esc_chars) != ಖಾಲಿ)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ಆದರೆ (p != ಖಾಲಿ)
		{
		    ಆದರೆ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ಆದರೆ (var != ಖಾಲಿ && *var != NUL)
	    {
		c = (ಪೂರ್ಣಾಂಕ)STRLEN(var);

		ಆದರೆ (c + STRLEN(ಬಾಲ) + 1 < (ಚಿಹ್ನೆರಹಿತ)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ಆದರೆ (after_pathsep(dst, dst + c)
#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#ಆದರೆ_ಅಂತ್ಯ
			    && vim_ispathsep(*ಬಾಲ))
			++ಬಾಲ;
		    dst += c;
		    src = ಬಾಲ;
		    copy_char = FALSE;
		}
	    }
	    ಆದರೆ (mustfree)
		vim_free(var);
	}

	ಆದರೆ (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_ಆರಂಭ = FALSE;
	    ಆದರೆ (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((src[0] == ' ' || src[0] == ',') && !one)
		at_ಆರಂಭ = TRUE;
	    ಆದರೆ (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ಆದರೆ (startstr != ಖಾಲಿ && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_ಆರಂಭ = TRUE;
	    }
	}

    }
    *dst = NUL;

    ಹಿಂತಿರುಗಿ (size_t)(dst - dst_ಆರಂಭ);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ಸ್ಥಿರ char_u *
remove_ಬಾಲ(char_u *p, char_u *pend, char_u *name)
{
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ಆದರೆ (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ಹಿಂತಿರುಗಿ newend;
    ಹಿಂತಿರುಗಿ pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ಸ್ಥಿರ char_u *
vim_ಆವೃತ್ತಿ_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ಆದರೆ (vimdir == ಖಾಲಿ || *vimdir == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_ಆವೃತ್ತಿ_NODOT, STRLEN_LITERAL(VIM_ಆವೃತ್ತಿ_NODOT), TRUE, &p);
    ಆದರೆ (p.string != ಖಾಲಿ && mch_isdir(p.string))
	ಹಿಂತಿರುಗಿ p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ಆದರೆ (p.string != ಖಾಲಿ && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.ಉದ್ದಳತೆ,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ಆದರೆ (fname.string != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ಆದರೆ (exists)
		ಹಿಂತಿರುಗಿ p.string;
	}
    }
    vim_free(p.string);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, ಪೂರ್ಣಾಂಕ *mustfree)
{
    char_u	*p = ಖಾಲಿ;
    char_u	*pend;
    ಪೂರ್ಣಾಂಕ		vimruntime;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ಆದರೆ (STRCMP(name, "HOME") == 0)
	ಹಿಂತಿರುಗಿ homedir;

    // Use Wide function
    wn = enc_ಗಮ್ಯ_utf16(name, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    wp = _wgetenv(wn);
    vim_free(wn);

    ಆದರೆ (wp != ಖಾಲಿ && *wp == NUL)   // empty is the same as not set
	wp = ಖಾಲಿ;

    ಆದರೆ (wp != ಖಾಲಿ)
    {
	p = utf16_ಗಮ್ಯ_enc(wp, ಖಾಲಿ);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	*mustfree = TRUE;
	ಹಿಂತಿರುಗಿ p;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    p = mch_getenv(name);
    ಆದರೆ (p != ಖಾಲಿ && *p == NUL)	    // empty is the same as not set
	p = ಖಾಲಿ;

    ಆದರೆ (p != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ಆದರೆ (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ಸ್ಥಿರ ಅಕ್ಷರ user_Settings_ಮಾರ್ಗ[MAXPATHL];

	ಆದರೆ (ಕಂಡುಹಿಡಿ_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   user_Settings_ಮಾರ್ಗ, MAXPATHL) == B_OK)
	    ಹಿಂತಿರುಗಿ (char_u *)user_Settings_ಮಾರ್ಗ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ಆದರೆ (!vimruntime && STRCMP(name, "VIM") != 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ಆದರೆ (vimruntime
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ಆದರೆ (wp != ಖಾಲಿ && *wp == NUL)	    // empty is the same as not set
	    wp = ಖಾಲಿ;
	ಆದರೆ (wp != ಖಾಲಿ)
	{
	    char_u *q = utf16_ಗಮ್ಯ_enc(wp, ಖಾಲಿ);
	    ಆದರೆ (q != ಖಾಲಿ)
	    {
		p = vim_ಆವೃತ್ತಿ_dir(q);
		*mustfree = TRUE;
		ಆದರೆ (p == ಖಾಲಿ)
		    p = q;
	    }
	}
#ಇಲ್ಲದಿದ್ದರೆ
	p = mch_getenv((char_u *)"VIM");
	ಆದರೆ (p != ಖಾಲಿ && *p == NUL)	    // empty is the same as not set
	    p = ಖಾಲಿ;
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    p = vim_ಆವೃತ್ತಿ_dir(p);
	    ಆದರೆ (p != ಖಾಲಿ)
		*mustfree = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		p = mch_getenv((char_u *)"VIM");
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ಆದರೆ (p == ಖಾಲಿ)
    {
	ಆದರೆ (p_hf != ಖಾಲಿ && vim_strchr(p_hf, '$') == ಖಾಲಿ)
	    p = p_hf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ಇಲ್ಲದಿದ್ದರೆ
	    p = exe_name;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ಆದರೆ (p == p_hf)
		pend = remove_ಬಾಲ(p, pend, (char_u *)"doc");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ಆದರೆ (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_ಬಾಲ(p, pend, (char_u *)"MacOS");
		ಆದರೆ (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ಆದರೆ (pnew != ಖಾಲಿ)
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
	    ಆದರೆ (p == exe_name)
		pend = remove_ಬಾಲ(p, pend, (char_u *)"src");
#ಆದರೆ_ಅಂತ್ಯ

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ಆದರೆ (!vimruntime)
	    {
		pend = remove_ಬಾಲ(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_ಬಾಲ(p, pend, (char_u *)VIM_ಆವೃತ್ತಿ_NODOT);
	    }

	    // remove trailing path separator
	    ಆದರೆ (pend > p && after_pathsep(p, pend))
		--pend;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
	    ಆದರೆ (p == exe_name || p == p_hf)
#ಆದರೆ_ಅಂತ್ಯ
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ಆದರೆ (p != ಖಾಲಿ && !mch_isdir(p))
		VIM_CLEAR(p);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ಆದರೆ (vimruntime && (pend = vim_ಆವೃತ್ತಿ_dir(p)) != ಖಾಲಿ)
		{
		    vim_free(p);
		    p = pend;
		}
#ಆದರೆ_ಅಂತ್ಯ
		*mustfree = TRUE;
	    }
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PATHDEF
    // When there is a pathdef.c file we can use default_vim_dir and
    // default_vimruntime_dir
    ಆದರೆ (p == ಖಾಲಿ)
    {
	// Only use default_vimruntime_dir when it is not empty
	ಆದರೆ (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*default_vim_dir != NUL)
	{
	    ಆದರೆ (vimruntime && (p = vim_ಆವೃತ್ತಿ_dir(default_vim_dir)) != ಖಾಲಿ)
		*mustfree = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = default_vim_dir;
		*mustfree = FALSE;
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Set the environment variable, so that the new value can be found fast
     * next time, and others can also use it (e.g. Perl).
     */
    ಆದರೆ (p != ಖಾಲಿ)
    {
	ಆದರೆ (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ p;
}

    ಶೂನ್ಯ
vim_unsetenv(char_u *var)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNSETENV
    unsetenv((ಅಕ್ಷರ *)var);
#ಇಲ್ಲದಿದ್ದರೆ
    vim_setenv(var, (char_u *)"");
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    ಶೂನ್ಯ
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ಆದರೆ (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    ಶೂನ್ಯ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ಆದರೆ (STRICMP(name, "HOME") == 0)
	init_homedir();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Our portable version of setenv.
 */
    ಶೂನ್ಯ
vim_setenv(char_u *name, char_u *val)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SETENV
    mch_setenv((ಅಕ್ಷರ *)name, (ಅಕ್ಷರ *)val, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ಆದರೆ (envbuf != ಖಾಲಿ)
    {
	sprintf((ಅಕ್ಷರ *)envbuf, "%s=%s", name, val);
	putenv((ಅಕ್ಷರ *)envbuf);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ಆದರೆ (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    bindtextdomain(VIMPACKAGE, (ಅಕ್ಷರ *)buf);
	    vim_free(buf);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Function given to ExpandGeneric() to obtain an environment variable name.
 */
    char_u *
get_env_name(
    expand_T	*xp ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		idx)
{
#ಆದರೆ defined(AMIGA)
    // No environ[] on the Amiga.
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    ಬಾಹ್ಯ ಅಕ್ಷರ		**environ;
# endif
    char_u		*str;
    ಪೂರ್ಣಾಂಕ			n;

    str = (char_u *)environ[idx];
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ಆದರೆ (str[n] == '=' || str[n] == NUL)
	    ನಿಲ್ಲಿಸು;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ಹಿಂತಿರುಗಿ xp->xp_buf;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_user(char_u *user, ಪೂರ್ಣಾಂಕ need_copy)
{
    char_u	*user_copy = (user != ಖಾಲಿ && need_copy)
						    ? vim_strsave(user) : user;

    ಆದರೆ (user_copy == ಖಾಲಿ || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ಆದರೆ (need_copy)
	    vim_free(user_copy);
	ಹಿಂತಿರುಗಿ;
    }
    ((char_u **)(ga_users.ga_ದತ್ತಾಂಶ))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_users(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	lazy_init_ಮುಗಿದ = FALSE;

    ಆದರೆ (lazy_init_ಮುಗಿದ)
	ಹಿಂತಿರುಗಿ;

    lazy_init_ಮುಗಿದ = TRUE;
    ga_init2(&ga_users, ಗಾತ್ರ(char_u *), 20);

#ಆದರೆ defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ರಚನೆ passwd*	pw;

	setpwent();
	ಆಗ ((pw = getpwent()) != ಖಾಲಿ)
	    ಕೂಡಿಸು_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#ಅಥವಾಆದರೆ defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ಆದರೆ (NetUserEnum(ಖಾಲಿ, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_ಉದ್ದಳತೆ,
				       &nusers, &ntotal, ಖಾಲಿ) == NERR_Success)
	{
	    ಗಾಗಿ (i = 0; i < nusers; i++)
		ಕೂಡಿಸು_user(utf16_ಗಮ್ಯ_enc(uinfo[i].usri0_name, ಖಾಲಿ), FALSE);

	    Net_Api_ಬಫರ್_Free(uinfo);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ಆದರೆ (user_env != ಖಾಲಿ && *user_env != NUL)
	{
	    ಪೂರ್ಣಾಂಕ	i;

	    ಗಾಗಿ (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_ದತ್ತಾಂಶ)[i];

		ಆದರೆ (STRCMP(local_user, user_env) == 0)
		    ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (i == ga_users.ga_len)
	    {
		ರಚನೆ passwd	*pw = getpwnam((ಅಕ್ಷರ *)user_env);

		ಆದರೆ (pw != ಖಾಲಿ)
		    ಕೂಡಿಸು_user((char_u *)pw->pw_name, TRUE);
	    }
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Function given to ExpandGeneric() to obtain user names.
 */
    char_u*
get_users(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    init_users();
    ಆದರೆ (idx < ga_users.ga_len)
	ಹಿಂತಿರುಗಿ ((char_u **)ga_users.ga_ದತ್ತಾಂಶ)[idx];
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_user(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ n = (ಪೂರ್ಣಾಂಕ)STRLEN(name);
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;

    init_users();
    ಗಾಗಿ (i = 0; i < ga_users.ga_len; i++)
    {
	ಆದರೆ (STRCMP(((char_u **)ga_users.ga_ದತ್ತಾಂಶ)[i], name) == 0)
	    ಹಿಂತಿರುಗಿ 2; // full match
	ಆದರೆ (STRNCMP(((char_u **)ga_users.ga_ದತ್ತಾಂಶ)[i], name, n) == 0)
	    ಫಲಿತಾಂಶ = 1; // partial match
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prepare_ಗಮ್ಯ_exit(ಶೂನ್ಯ)
{
#ಆದರೆ defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	gui.dying = true;
	ಹೊರಹೋಗು_trash();	// trash any pending output
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ಆದರೆ (!ತುಂಬಿದ_screen)
	    mch_setmouse(FALSE);

	/*
	 * Switch terminal mode back now, so messages end up on the "normal"
	 * screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
	stoptermcap();
	ಹೊರಹೋಗು_flush();
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

    prepare_ಗಮ್ಯ_exit();

    // Setting this will prevent free() calls.  That avoids calling free()
    // recursively when free() was invoked with a bad pointer.
    really_exiting = TRUE;

    ಹೊರಹೋಗು_str(IObuff);
    screen_ಆರಂಭ();		    // don't know where cursor is now
    ಹೊರಹೋಗು_flush();

    ml_close_notmod();		    // close all not-modified buffers

    FOR_ALL_BUFFERS(buf)
    {
	ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ && buf->b_ml.ml_mfp->mf_fname != ಖಾಲಿ)
	{
	    ಹೊರಹೋಗು_STR("Vim: preserving files...\r\n");
	    screen_ಆರಂಭ();	    // don't know where cursor is now
	    ಹೊರಹೋಗು_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    ನಿಲ್ಲಿಸು;
	}
    }

    ml_close_all(FALSE);	    // close all memfiles, without deleting

    ಹೊರಹೋಗು_STR("Vim: Finished.\r\n");

    getout(1);
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BREAKCHECK_SKIP
# define BREAKCHECK_SKIP 1000
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	breakcheck_ಎಣಿಕೆ = 0;

    ಶೂನ್ಯ
ಸಾಲು_breakcheck(ಶೂನ್ಯ)
{
    ಆದರೆ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP)
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
    ಆದರೆ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_ಎಣಿಕೆ = 0;
	ui_breakcheck();
    }
}

#ಆದರೆ defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    ಶೂನ್ಯ
veryfast_breakcheck(ಶೂನ್ಯ)
{
    ಆದರೆ (++breakcheck_ಎಣಿಕೆ >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_ಎಣಿಕೆ = 0;
	ui_breakcheck();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    ಪೂರ್ಣಾಂಕ		flags,		// can be SHELL_SILENT
    ಪೂರ್ಣಾಂಕ		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*ಬಫರ್ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i = 0;
    FILE	*fd;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ಆದರೆ ((tempname = vim_tempname('o', TRUE)) == ಖಾಲಿ)
    {
	emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ಆದರೆ (command == ಖಾಲಿ)
	ಹೋಗು ಮುಗಿದ;

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
    fd = mch_fopen((ಅಕ್ಷರ *)tempname, "r");
# ಇಲ್ಲದಿದ್ದರೆ
    fd = mch_fopen((ಅಕ್ಷರ *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ಆದರೆ (fd == ಖಾಲಿ
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ಆದರೆ (fd != ಖಾಲಿ)
	    fclose(fd);
	ಹೋಗು ಮುಗಿದ;
    }

    ಬಫರ್ = alloc(len + 1);
    ಆದರೆ (ಬಫರ್ != ಖಾಲಿ)
	i = (ಪೂರ್ಣಾಂಕ)fread((ಅಕ್ಷರ *)ಬಫರ್, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹೋಗು ಮುಗಿದ;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ಆದರೆ (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(ಬಫರ್);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_len == ಖಾಲಿ)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ಆದರೆ (ಬಫರ್[i] == NUL)
		ಬಫರ್[i] = 1;

	ಬಫರ್[len] = NUL;	// make sure the buffer is terminated
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*ret_len = len;

ಮುಗಿದ:
    mch_remove(tempname);
    vim_free(tempname);
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}

# ಆದರೆ defined(FEAT_EVAL)

    ಸ್ಥಿರ ಶೂನ್ಯ
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		retlist)
{
    char_u	*res = ಖಾಲಿ;
    char_u	*p;
    char_u	*infile = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		err = FALSE;
    FILE	*fd;
    list_T	*list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		flags = SHELL_SILENT;
    ಪೂರ್ಣಾಂಕ		use_argv = FALSE;
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    ಆದರೆ (check_restricted() || check_secure())
	ಹೋಗು errret;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 1)
								      == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ಆದರೆ ((infile = vim_tempname('i', TRUE)) == ಖಾಲಿ)
	{
	    emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	    ಹೋಗು errret;
	}

	fd = mch_fopen((ಅಕ್ಷರ *)infile, WRITEBIN);
	ಆದರೆ (fd == ಖಾಲಿ)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    ಹೋಗು errret;
	}
	ಆದರೆ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_ಸಂಖ್ಯೆ);
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		semsg(_(e_ಬಫರ್_nr_does_not_exist), argvars[1].vval.v_ಸಂಖ್ಯೆ);
		fclose(fd);
		ಹೋಗು errret;
	    }

	    ಗಾಗಿ (lnum = 1; lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; lnum++)
	    {
		ಗಾಗಿ (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ಆದರೆ (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			ನಿಲ್ಲಿಸು;
		    }
		ಆದರೆ (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_LIST)
	{
	    ಆದರೆ (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		fclose(fd);
		ಹೋಗು errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ಆದರೆ (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ಆದರೆ (fclose(fd) != 0)
	    err = TRUE;
	ಆದರೆ (err)
	{
	    emsg(_(e_error_writing_ತಾತ್ಕಾಲಿಕ_file));
	    ಹೋಗು errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ಆದರೆ (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ಆದರೆ (l == ಖಾಲಿ || l->lv_len < 1)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹೋಗು errret;
	}
	ಆದರೆ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ಹೋಗು errret;
	ಆದರೆ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_ವಾದ));
	    ಹೋಗು errret;
	}
	use_argv = TRUE;

	ಆದರೆ (p_verbose > 3)
	{
	    ಪೂರ್ಣಾಂಕ		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    ಗಾಗಿ (i = 0; i < argc; ++i)
	    {
		ಆದರೆ (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ);
	    ಸಂದೇಶ_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ಆದರೆ (!ಸಂದೇಶ_silent)
	flags += SHELL_COOKED;

    ಆದರೆ (retlist)
    {
	ಪೂರ್ಣಾಂಕ		len;
	listitem_T	*li;
	char_u		*s = ಖಾಲಿ;
	char_u		*ಆರಂಭ;
	char_u		*ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ		i;

	ಆದರೆ (use_argv)
	    res = mch_get_cmd_output_ನೇರ(argv, infile, flags, &len);
	ಇಲ್ಲದಿದ್ದರೆ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ಆದರೆ (res == ಖಾಲಿ)
	    ಹೋಗು errret;

	list = list_alloc();
	ಆದರೆ (list == ಖಾಲಿ)
	    ಹೋಗು errret;

	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    ಆರಂಭ = res + i;
	    ಆಗ (i < len && res[i] != NL)
		++i;
	    ಕೊನೆ = res + i;

	    s = alloc(ಕೊನೆ - ಆರಂಭ + 1);
	    ಆದರೆ (s == ಖಾಲಿ)
		ಹೋಗು errret;

	    ಗಾಗಿ (p = s; ಆರಂಭ < ಕೊನೆ; ++p, ++ಆರಂಭ)
		*p = *ಆರಂಭ == NUL ? NL : *ಆರಂಭ;
	    *p = NUL;

	    li = listitem_alloc();
	    ಆದರೆ (li == ಖಾಲಿ)
	    {
		vim_free(s);
		ಹೋಗು errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_ಬೀಗ = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_ಗಣ(rettv, list);
	list = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (use_argv)
	    res = mch_get_cmd_output_ನೇರ(argv, infile, flags, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ಖಾಲಿ);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ಆದರೆ (res != ಖಾಲಿ)
	{
	    char_u	*s, *d;

	    d = res;
	    ಗಾಗಿ (s = res; *s; ++s)
	    {
		ಆದರೆ (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ಖಾಲಿ;
    }

errret:
    ಆದರೆ (argv != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ (i = 0; argv[i] != ಖಾಲಿ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ಆದರೆ (infile != ಖಾಲಿ)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ಆದರೆ (res != ಖಾಲಿ)
	vim_free(res);
    ಆದರೆ (list != ಖಾಲಿ)
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

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when need to go to Insert mode because of 'insertmode'.
 * Don't do this when still processing a command or a mapping.
 * Don't do this when inside a ":normal" command.
 */
    ಪೂರ್ಣಾಂಕ
goto_im(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (p_im && stuff_ಖಾಲಿಗಳು() && typebuf_typed());
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ಇಲ್ಲದಿದ್ದರೆ
    p = skiptowhite(p_sh);
    ಆದರೆ (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	ಗಾಗಿ (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ಆದರೆ (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    ಪೂರ್ಣಾಂಕ
ಮಾರ್ಗ_is_url(char_u *p)
{
    ಆದರೆ (STRNCMP(p, "://", (size_t)3) == 0)
	ಹಿಂತಿರುಗಿ URL_SLASH;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ಹಿಂತಿರುಗಿ URL_BACKSLASH;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ಮಾರ್ಗ_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ಆದರೆ (!ASCII_ISALPHA(*fname))
	ಹಿಂತಿರುಗಿ 0;

    // check body: alpha or dash
    ಗಾಗಿ (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ಆದರೆ (p[-1] == '-')
	ಹಿಂತಿರುಗಿ 0;

    // "://" or ":\\" must follow
    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_url(p);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ಆದರೆ (v_event->dv_hashtab.ht_ಬಳಸಿದ > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	sve->sve_did_save = FALSE;
    ಹಿಂತಿರುಗಿ v_event;
}

    ಶೂನ್ಯ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ಆದರೆ (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ಇಲ್ಲದಿದ್ದರೆ
	hash_init(&v_event->dv_hashtab);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    ಶೂನ್ಯ
may_trigger_modechanged(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_ಬಹುಲಕ[ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ];
    char_u	    ನಮೂನೆ_buf[2 * ಬಹುಲಕ_MAX_ಉದ್ದಳತೆ];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ಆದರೆ (!has_modechanged() || got_int)
	ಹಿಂತಿರುಗಿ;

    get_ಬಹುಲಕ(curr_ಬಹುಲಕ);
    ಆದರೆ (STRCMP(curr_ಬಹುಲಕ, ಕೊನೆಯ_ಬಹುಲಕ) == 0)
	ಹಿಂತಿರುಗಿ;

    v_event = get_v_event(&save_v_event);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string(v_event, "new_mode", curr_ಬಹುಲಕ);
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು_string(v_event, "old_mode", ಕೊನೆಯ_ಬಹುಲಕ);
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((ಅಕ್ಷರ *)ನಮೂನೆ_buf, ಗಾತ್ರ(ನಮೂನೆ_buf), "%s:%s", ಕೊನೆಯ_ಬಹುಲಕ,
	    curr_ಬಹುಲಕ);

    apply_autocmds(EVENT_MODECHANGED, ನಮೂನೆ_buf, ಖಾಲಿ, FALSE, curbuf);
    STRCPY(ಕೊನೆಯ_ಬಹುಲಕ, curr_ಬಹುಲಕ);

    restore_v_event(v_event, &save_v_event);
#ಆದರೆ_ಅಂತ್ಯ
}

// For overflow detection, add a digit safely to a long value.
    ಪೂರ್ಣಾಂಕ
vim_append_digit_long(ದೀರ್ಘ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ digit)
{
    ದೀರ್ಘ x = *ಮೌಲ್ಯ;
    ಆದರೆ (x > ((LONG_MAX - (ದೀರ್ಘ)digit) / 10))
	ಹಿಂತಿರುಗಿ FAIL;
    *ಮೌಲ್ಯ = x * 10 + (ದೀರ್ಘ)digit;
    ಹಿಂತಿರುಗಿ OK;
}

// Return something that fits into an int.
    ಪೂರ್ಣಾಂಕ
trim_ಗಮ್ಯ_int(vimlong_T x)
{
    ಹಿಂತಿರುಗಿ x > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : x < INT_MIN ? INT_MIN : x;
}
