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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ defined(__HAIKU__)
# include <storage/FindDirectory.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
# include <lm.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ URL_SLASH	1		// path_is_url() has found "://"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ਸਥਿਰ garray_T	ga_users;

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
    ਪੂਰਨ
get_leader_len(
    char_u	*ਸਤਰ,
    char_u	**flags,
    ਪੂਰਨ		backward,
    ਪੂਰਨ		include_space)
{
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		ਨਤੀਜਾ;
    ਪੂਰਨ		got_com = FALSE;
    ਪੂਰਨ		ਮਿਲਿਆ_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    ਪੂਰਨ		ਵਿਚਕਾਰ_ਮੇਲ_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ਨਿਰਾ;

    ਨਤੀਜਾ = i = 0;
    ਜਦੋਂ (VIM_ISWHITE(ਸਤਰ[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    ਜਦੋਂ (ਸਤਰ[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ਮਿਲਿਆ_one = FALSE;
	ਲਈ (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ਜੇ (!got_com && flags != ਨਿਰਾ)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (ਖਾਲੀ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ਜੇ (string == ਨਿਰਾ)	    // missing ':', ignore this part
		ਜਾਰੀ;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ਜੇ (ਵਿਚਕਾਰ_ਮੇਲ_len != 0
		    && vim_strchr(part_buf, COM_ਵਿਚਕਾਰ) == ਨਿਰਾ
		    && vim_strchr(part_buf, COM_ਸਿਰਾ) == ਨਿਰਾ)
		ਰੋਕੋ;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ਜੇ (got_com && vim_strchr(part_buf, COM_NEST) == ਨਿਰਾ)
		ਜਾਰੀ;

	    // When 'O' flag present and using "O" command skip this one.
	    ਜੇ (backward && vim_strchr(part_buf, COM_NOBACK) != ਨਿਰਾ)
		ਜਾਰੀ;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ਜੇ (VIM_ISWHITE(string[0]))
	    {
		ਜੇ (i == 0 || !VIM_ISWHITE(ਸਤਰ[i - 1]))
		    ਜਾਰੀ;  // missing white space
		ਜਦੋਂ (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    ਲਈ (j = 0; string[j] != NUL && string[j] == ਸਤਰ[i + j]; ++j)
		;
	    ਜੇ (string[j] != NUL)
		ਜਾਰੀ;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ਜੇ (vim_strchr(part_buf, COM_BLANK) != ਨਿਰਾ
			   && !VIM_ISWHITE(ਸਤਰ[i + j]) && ਸਤਰ[i + j] != NUL)
		ਜਾਰੀ;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ਜੇ (vim_strchr(part_buf, COM_ਵਿਚਕਾਰ) != ਨਿਰਾ)
	    {
		ਜੇ (ਵਿਚਕਾਰ_ਮੇਲ_len == 0)
		{
		    ਵਿਚਕਾਰ_ਮੇਲ_len = j;
		    saved_flags = prev_list;
		}
		ਜਾਰੀ;
	    }
	    ਜੇ (ਵਿਚਕਾਰ_ਮੇਲ_len != 0 && j > ਵਿਚਕਾਰ_ਮੇਲ_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		ਵਿਚਕਾਰ_ਮੇਲ_len = 0;

	    ਜੇ (ਵਿਚਕਾਰ_ਮੇਲ_len == 0)
		i += j;
	    ਮਿਲਿਆ_one = TRUE;
	    ਰੋਕੋ;
	}

	ਜੇ (ਵਿਚਕਾਰ_ਮੇਲ_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ਜੇ (!got_com && flags != ਨਿਰਾ)
		*flags = saved_flags;
	    i += ਵਿਚਕਾਰ_ਮੇਲ_len;
	    ਮਿਲਿਆ_one = TRUE;
	}

	// No match found, stop scanning.
	ਜੇ (!ਮਿਲਿਆ_one)
	    ਰੋਕੋ;

	ਨਤੀਜਾ = i;

	// Include any trailing white space.
	ਜਦੋਂ (VIM_ISWHITE(ਸਤਰ[i]))
	    ++i;

	ਜੇ (include_space)
	    ਨਤੀਜਾ = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ਜੇ (vim_strchr(part_buf, COM_NEST) == ਨਿਰਾ)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    ਪੂਰਨ
get_ਆਖਰੀ_leader_ਵਿੱਥ(char_u *ਸਤਰ, char_u **flags)
{
    ਪੂਰਨ		ਨਤੀਜਾ = -1;
    ਪੂਰਨ		i, j;
    ਪੂਰਨ		ਹੇਠਲਾ_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    ਪੂਰਨ		ਮਿਲਿਆ_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (ਪੂਰਨ)STRLEN(ਸਤਰ);
    ਜਦੋਂ (--i >= ਹੇਠਲਾ_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	ਮਿਲਿਆ_one = FALSE;
	ਲਈ (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (ਖਾਲੀ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ਜੇ (string == ਨਿਰਾ)	// If everything is fine, this cannot actually
				// happen.
		ਜਾਰੀ;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ਜੇ (VIM_ISWHITE(string[0]))
	    {
		ਜੇ (i == 0 || !VIM_ISWHITE(ਸਤਰ[i - 1]))
		    ਜਾਰੀ;
		ਜਦੋਂ (VIM_ISWHITE(*string))
		    ++string;
	    }
	    ਲਈ (j = 0; string[j] != NUL && string[j] == ਸਤਰ[i + j]; ++j)
		/* do nothing */;
	    ਜੇ (string[j] != NUL)
		ਜਾਰੀ;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ਜੇ (vim_strchr(part_buf, COM_BLANK) != ਨਿਰਾ
		    && !VIM_ISWHITE(ਸਤਰ[i + j]) && ਸਤਰ[i + j] != NUL)
		ਜਾਰੀ;

	    ਜੇ (vim_strchr(part_buf, COM_ਵਿਚਕਾਰ) != ਨਿਰਾ)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		ਲਈ (j = 0; VIM_ISWHITE(ਸਤਰ[j]) && j <= i; j++)
		    ;
		ਜੇ (j < i)
		    ਜਾਰੀ;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    ਮਿਲਿਆ_one = TRUE;

	    ਜੇ (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    ਰੋਕੋ;
	}

	ਜੇ (ਮਿਲਿਆ_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    ਪੂਰਨ     len1, len2, off;

	    ਨਤੀਜਾ = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ਜੇ (vim_strchr(part_buf, COM_NEST) != ਨਿਰਾ)
		ਜਾਰੀ;

	    ਹੇਠਲਾ_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    ਜਦੋਂ (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (ਪੂਰਨ)STRLEN(com_leader);

	    ਲਈ (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(ਖਾਲੀ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ਜੇ (flags_save == com_flags)
		    ਜਾਰੀ;
		string = vim_strchr(part_buf2, ':');
		++string;
		ਜਦੋਂ (VIM_ISWHITE(*string))
		    ++string;
		len2 = (ਪੂਰਨ)STRLEN(string);
		ਜੇ (len2 == 0)
		    ਜਾਰੀ;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		ਲਈ (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ਜੇ (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ਜੇ (i - off < ਹੇਠਲਾ_check_bound)
			    ਹੇਠਲਾ_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    ਪੂਰਨ
plines(linenr_T lnum)
{
    ਵਾਪਸ plines_win(curwin, lnum, TRUE);
}

    ਪੂਰਨ
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    ਪੂਰਨ		ਹੱਦਬੰਦੀ_winheight)	// when TRUE limit to window height
{
#ਜੇ defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ਵਾਪਸ plines_win_nofill(wp, lnum, ਹੱਦਬੰਦੀ_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    ਪੂਰਨ
plines_nofill(linenr_T lnum)
{
    ਵਾਪਸ plines_win_nofill(curwin, lnum, TRUE);
}

    ਪੂਰਨ
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    ਪੂਰਨ		ਹੱਦਬੰਦੀ_winheight)	// when TRUE limit to window height
{
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਸਤਰਾਂ;

    ਜੇ (wp->w_ਚੌੜਾਈ == 0)
	ਵਾਪਸ 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ਜੇ (ਸਤਰ_Folded(wp, lnum) == TRUE)
	ਵਾਪਸ 1;
#ਜੇ_ਅੰਤ

    ਜੇ (!wp->w_p_wrap)
	ਸਤਰਾਂ = 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_ਗਿਣਤੀ_above_ਹੇਠਾਂ(wp->w_ਬਫਰ, lnum)
#ਜੇ_ਅੰਤ
	;
    ਨਹੀਂਤਾਂ
	ਸਤਰਾਂ = plines_win_nofold(wp, lnum);

    ਜੇ (ਹੱਦਬੰਦੀ_winheight && ਸਤਰਾਂ > wp->w_ਉਚਾਈ)
	ਵਾਪਸ wp->w_ਉਚਾਈ;
    ਵਾਪਸ ਸਤਰਾਂ;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    ਪੂਰਨ
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    ਲੰਮਾ	ਖਾਨਾ;
    ਪੂਰਨ		ਚੌੜਾਈ;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ਜੇ (*s == NUL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_ਪਾਠ
#ਜੇ_ਅੰਤ
	    )
	ਵਾਪਸ 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    ਖਾਨਾ = (ਪੂਰਨ)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ਜੇ (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	ਖਾਨਾ += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
    ਜੇ (ਚੌੜਾਈ <= 0)
	ਵਾਪਸ 32000;
    ਜੇ (ਖਾਨਾ <= ਚੌੜਾਈ)
	ਵਾਪਸ 1;
    ਖਾਨਾ -= ਚੌੜਾਈ;
    ਚੌੜਾਈ += win_ਖਾਨਾ_off2(wp);
    ਵਾਪਸ (ਖਾਨਾ + (ਚੌੜਾਈ - 1)) / ਚੌੜਾਈ + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    ਪੂਰਨ
plines_win_ਖਾਨਾ(win_T *wp, linenr_T lnum, ਲੰਮਾ ਕਾਲਮ)
{
    ਲੰਮਾ	ਖਾਨਾ;
    ਪੂਰਨ		ਸਤਰਾਂ = 0;
    ਪੂਰਨ		ਚੌੜਾਈ;
    char_u	*ਸਤਰ;
    chartabsize_T cts;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ਸਤਰਾਂ = diff_check_fill(wp, lnum);
#ਜੇ_ਅੰਤ

    ਜੇ (!wp->w_p_wrap)
	ਵਾਪਸ ਸਤਰਾਂ + 1;

    ਜੇ (wp->w_ਚੌੜਾਈ == 0)
	ਵਾਪਸ ਸਤਰਾਂ + 1;

    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ਸਤਰ, ਸਤਰ);
    // "column" is a byte index, advance the pointer until it is reached.
    ਜਦੋਂ (*cts.cts_ptr != NUL && cts.cts_ptr < ਸਤਰ + ਕਾਲਮ)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ਨਿਰਾ, ਨਿਰਾ);
	MB_PTR_ADV(cts.cts_ptr);
    }

    /*
     * If *cts.cts_ptr is a TAB, and the TAB is not displayed as ^I, and we're
     * not in MODE_INSERT state, then col must be adjusted so that it
     * represents the last screen position of the TAB.  This only fixes an
     * error when the TAB wraps from one screen line to the next (when
     * 'columns' is not a multiple of 'ts') -- webb.
     */
    ਖਾਨਾ = cts.cts_vcol;
    ਜੇ (*cts.cts_ptr == TAB && (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	ਖਾਨਾ += win_lbr_chartabsize(&cts, ਨਿਰਾ, ਨਿਰਾ) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    ਚੌੜਾਈ = wp->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(wp);
    ਜੇ (ਚੌੜਾਈ <= 0)
	ਵਾਪਸ 9999;

    ਸਤਰਾਂ += 1;
    ਜੇ (ਖਾਨਾ > ਚੌੜਾਈ)
	ਸਤਰਾਂ += (ਖਾਨਾ - ਚੌੜਾਈ) / (ਚੌੜਾਈ + win_ਖਾਨਾ_off2(wp)) + 1;
    ਵਾਪਸ ਸਤਰਾਂ;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    ਪੂਰਨ
plines_m_win(win_T *wp, linenr_T ਪਹਿਲਾ, linenr_T ਆਖਰੀ, ਪੂਰਨ max)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਜਦੋਂ (ਪਹਿਲਾ <= ਆਖਰੀ && ਗਿਣਤੀ_2 < max)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਪੂਰਨ	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_ਗਿਣਤੀ(wp, ਪਹਿਲਾ, ਨਿਰਾ);
	ਜੇ (x > 0)
	{
	    ++ਗਿਣਤੀ_2;	    // count 1 for "+-- folded" line
	    ਪਹਿਲਾ += x;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (ਪਹਿਲਾ == wp->w_topline)
		ਗਿਣਤੀ_2 += plines_win_nofill(wp, ਪਹਿਲਾ, FALSE) + wp->w_topfill;
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਗਿਣਤੀ_2 += plines_win(wp, ਪਹਿਲਾ, FALSE);
	    ++ਪਹਿਲਾ;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (ਪਹਿਲਾ == wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	ਗਿਣਤੀ_2 += diff_check_fill(wp, ਪਹਿਲਾ);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ MIN(max, ਗਿਣਤੀ_2);
}

    ਪੂਰਨ
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    ਪੂਰਨ		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ਜੇ (pos->ਖਾਨਾ == MAXCOL)
	ਵਾਪਸ NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ਜੇ (pos->ਖਾਨਾ > ptrlen)
	ਵਾਪਸ NUL;
    ਜੇ (has_mbyte)
	ਵਾਪਸ (*mb_ptr2char)(ptr);
    ਵਾਪਸ (ਪੂਰਨ)*ptr;
}

    ਪੂਰਨ
gchar_cursor(ਖਾਲੀ)
{
    ਜੇ (has_mbyte)
	ਵਾਪਸ (*mb_ptr2char)(ml_get_cursor());
    ਵਾਪਸ (ਪੂਰਨ)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    ਪੂਰਨ
char_before_cursor(ਖਾਲੀ)
{
    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
	ਵਾਪਸ -1;

    char_u *ਸਤਰ = ml_get_curline();

    ਜੇ (has_mbyte)
    {
	char_u *p = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;
	ਪੂਰਨ prev_len = (*mb_ਸਿਰ_off)(ਸਤਰ, p - 1) + 1;
	ਵਾਪਸ mb_ptr2char(p - prev_len);
    }
    ਵਾਪਸ ਸਤਰ[curwin->w_cursor.ਖਾਨਾ - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    ਖਾਲੀ
pchar_cursor(ਪੂਰਨ c)
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE)
						  + curwin->w_cursor.ਖਾਨਾ) = c;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_ਮੰਜ਼ਿਲ_option_part(char_u *p)
{
    ਜੇ (*p == ',')
	++p;
    ਜਦੋਂ (*p == ' ')
	++p;
    ਵਾਪਸ p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    ਖਾਲੀ
check_ਸਥਿਤੀ(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ == buf && wp->w_ਸਥਿਤੀ_ਉਚਾਈ)
	{
	    wp->w_redr_ਸਥਿਤੀ = true;
	    ਸੈੱਟ_must_redraw(UPD_VALID);
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
    ਪੂਰਨ
ask_yesno(char_u *str, ਪੂਰਨ ਸਿੱਧਾ)
{
    ਪੂਰਨ	    r = ' ';
    ਪੂਰਨ	    save_State = State;

    ਜੇ (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ
    State = ਬਹੁਲਕ_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;		// no mapping here, but recognize keys

    ਜਦੋਂ (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ਜੇ (ਸਿੱਧਾ)
	    r = get_keystroke();
	ਨਹੀਂਤਾਂ
	    r = plain_vgetc();
	ਜੇ (r == Ctrl_C || r == ESC)
	    r = 'n';
	ਸੁਨੇਹਾ_putchar(r);	    // show what you typed
	ਬਾਹਰਲਾ_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;

    ਵਾਪਸ r;
}

#ਜੇ defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    ਖਾਲੀ
get_ਬਹੁਲਕ(char_u *buf)
{
    ਪੂਰਨ		i = 0;

    ਜੇ (ਸਮਾਂ_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_use_loop())
    {
	ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE
						      || State == ਬਹੁਲਕ_SETWSIZE
		|| State == ਬਹੁਲਕ_CONFIRM)
    {
	buf[i++] = 'r';
	ਜੇ (State == ਬਹੁਲਕ_ASKMORE)
	    buf[i++] = 'm';
	ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_CONFIRM)
	    buf[i++] = '?';
    }
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_EXTERNCMD)
	buf[i++] = '!';
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
    {
	ਜੇ (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ਨਹੀਂਤਾਂ
		buf[i++] = 'i';
	}

	ਜੇ (ins_compl_ਸਰਗਰਮ())
	    buf[i++] = 'c';
	ਨਹੀਂਤਾਂ ਜੇ (ctrl_x_ਬਹੁਲਕ_not_defined_yet())
	    buf[i++] = 'x';
    }
    ਨਹੀਂਤਾਂ ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) || exmode_ਸਰਗਰਮ)
    {
	buf[i++] = 'c';
	ਜੇ (exmode_ਸਰਗਰਮ == EXMODE_VIM)
	    buf[i++] = 'v';
	ਨਹੀਂਤਾਂ ਜੇ (exmode_ਸਰਗਰਮ == EXMODE_ਸਾਧਾਰਨ)
	    buf[i++] = 'e';
	ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ)
    {
	ਜੇ (VIsual_select)
	    buf[i++] = VIsual_ਬਹੁਲਕ + 's' - 'v';
	ਨਹੀਂਤਾਂ
	{
	    buf[i++] = VIsual_ਬਹੁਲਕ;
	    ਜੇ (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ਨਹੀਂਤਾਂ
    {
	buf[i++] = 'n';
	ਜੇ (ਅੰਤ_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ਨਹੀਂਤਾਂ ਜੇ (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ਨਹੀਂਤਾਂ ਜੇ (ਪਦ_ਵਿੱਚ_ਸਾਧਾਰਨ_ਬਹੁਲਕ(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    ਖਾਲੀ
f_ਬਹੁਲਕ(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[ਬਹੁਲਕ_MAX_ਲੰਮਾਈ];

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    get_ਬਹੁਲਕ(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ਜੇ (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ਸਥਿਰ ਖਾਲੀ
may_ਜੋੜੋ_state_char(garray_T *gap, char_u *include, ਪੂਰਨ c)
{
    ਜੇ (include == ਨਿਰਾ || vim_strchr(include, c) != ਨਿਰਾ)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    ਖਾਲੀ
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ਨਿਰਾ;
    ਪੂਰਨ		i;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ga_init2(&ga, 1, 20);
    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ਜੇ (!(stuff_ਖਾਲੀ_ਥਾਵਾਂ() && typebuf.tb_len == 0 && scriptin[curscript] == ਨਿਰਾ))
	may_ਜੋੜੋ_state_char(&ga, include, 'm');
    ਜੇ (op_pending())
	may_ਜੋੜੋ_state_char(&ga, include, 'o');
    ਜੇ (autocmd_busy)
	may_ਜੋੜੋ_state_char(&ga, include, 'x');
    ਜੇ (ins_compl_ਸਰਗਰਮ())
	may_ਜੋੜੋ_state_char(&ga, include, 'a');

# ifdef FEAT_ਕੰਮ_CHANNEL
    ਜੇ (channel_ਵਿੱਚ_blocking_wait())
	may_ਜੋੜੋ_state_char(&ga, include, 'w');
# endif
    ਜੇ (!get_was_ਸੁਰੱਖਿਅਤ_state())
	may_ਜੋੜੋ_state_char(&ga, include, 'S');
    ਲਈ (i = 0; i < get_callback_ਡੂੰਘਾਈ() && i < 3; ++i)
	may_ਜੋੜੋ_state_char(&ga, include, 'c');
    ਜੇ (ਸੁਨੇਹਾ_scrolled > 0)
	may_ਜੋੜੋ_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ਅੰਕੜੇ;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL

/*
 * Get a key stroke directly from the user.
 * Ignores mouse clicks and scrollbar events, except a click for the left
 * button (used at the more prompt).
 * Doesn't use vgetc(), because it syncs undo and eats mapped characters.
 * Disadvantage: typeahead is ignored.
 * Translates the interrupt character for unix to ESC.
 */
    ਪੂਰਨ
get_keystroke(ਖਾਲੀ)
{
    char_u	*buf = ਨਿਰਾ;
    ਪੂਰਨ		buflen = 150;
    ਪੂਰਨ		maxlen;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		n;
    ਪੂਰਨ		save_mapped_ctrl_c = mapped_ctrl_c;
    ਪੂਰਨ		waited = 0;

    mod_ਨਕਾਬ = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    ਲਈ (;;)
    {
	cursor_on();
	ਬਾਹਰਲਾ_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ਜੇ (buf == ਨਿਰਾ)
	    buf = alloc(buflen);
	ਨਹੀਂਤਾਂ ਜੇ (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ਜੇ (buf == ਨਿਰਾ)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ਜੇ (buf == ਨਿਰਾ)
	{
	    do_outofmem_ਸੁਨੇਹਾ((long_u)buflen);
	    ਵਾਪਸ ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ਜੇ (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_ਨਿਵੇਸ਼_ਬਫਰ(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ਜੇ ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ਜਾਰੀ;

	ਜੇ (n == KEYLEN_REMOVED)  // key code removed
	{
	    ਜੇ (must_redraw != 0 && !need_wait_return && (State
			& (ਬਹੁਲਕ_CMDLINE | ਬਹੁਲਕ_HITRETURN | ਬਹੁਲਕ_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ਜਾਰੀ;
	}
	ਜੇ (n > 0)		// found a termcode: adjust length
	    len = n;
	ਜੇ (len == 0)		// nothing typed yet
	    ਜਾਰੀ;

	// Handle modifier and/or special key code.
	n = buf[0];
	ਜੇ (n == K_SPECIAL)
	{
	    n = ਮੰਜ਼ਿਲ_SPECIAL(buf[1], buf[2]);
	    ਜੇ (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ਕੁੰਜੀ(n) && n != K_LEFTMOUSE)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#ਜੇ_ਅੰਤ
	       )
	    {
		ਜੇ (buf[1] == KS_MODIFIER)
		    mod_ਨਕਾਬ = buf[2];
		len -= 3;
		ਜੇ (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ਜਾਰੀ;
	    }
	    ਰੋਕੋ;
	}
	ਜੇ (has_mbyte)
	{
	    ਜੇ (MB_BYTE2LEN(n) > len)
		ਜਾਰੀ;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (n == intr_char)
	    n = ESC;
#ਜੇ_ਅੰਤ
	ਰੋਕੋ;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ਵਾਪਸ ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(n, &mod_ਨਕਾਬ);
}

// For overflow detection, add a digit safely to an int value.
    ਸਥਿਰ ਪੂਰਨ
vim_append_digit_int(ਪੂਰਨ *ਮੁੱਲ, ਪੂਰਨ digit)
{
    ਪੂਰਨ x = *ਮੁੱਲ;
    ਜੇ (x > ((ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - digit) / 10))
	ਵਾਪਸ FAIL;
    *ਮੁੱਲ = x * 10 + digit;
    ਵਾਪਸ OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    ਪੂਰਨ
get_ਸੰਖਿਆ(
    ਪੂਰਨ	    colon,			// allow colon to abort
    ਪੂਰਨ	    *mouse_ਵਰਤਿਆ)
{
    ਪੂਰਨ	n = 0;
    ਪੂਰਨ	c;
    ਪੂਰਨ typed = 0;

    ਜੇ (mouse_ਵਰਤਿਆ != ਨਿਰਾ)
	*mouse_ਵਰਤਿਆ = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ਜੇ (ਸੁਨੇਹਾ_silent != 0)
	ਵਾਪਸ 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;		// no mapping here, but recognize keys
    ਲਈ (;;)
    {
	windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
	c = ਸੁਰੱਖਿਅਤ_vgetc();
	ਜੇ (VIM_ISDIGIT(c))
	{
	    ਜੇ (vim_append_digit_int(&n, c - '0') == FAIL)
		ਵਾਪਸ 0;
	    ਸੁਨੇਹਾ_putchar(c);
	    ++typed;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ਜੇ (typed > 0)
	    {
		ਸੁਨੇਹਾ_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ਨਹੀਂਤਾਂ ਜੇ (mouse_ਵਰਤਿਆ != ਨਿਰਾ && c == K_LEFTMOUSE)
	{
	    *mouse_ਵਰਤਿਆ = TRUE;
	    n = mouse_ਪੰਕਤੀ + 1;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ਜੇ (!exmode_ਸਰਗਰਮ)
		cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == CAR || c == NL )
	    ਰੋਕੋ;
    }
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
    ਵਾਪਸ n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    ਪੂਰਨ
prompt_for_ਸੰਖਿਆ(ਪੂਰਨ *mouse_ਵਰਤਿਆ)
{
    ਪੂਰਨ		i;
    ਪੂਰਨ		save_cmdline_ਪੰਕਤੀ;
    ਪੂਰਨ		save_State;

    // When using ":silent" assume that <CR> was entered.
    ਜੇ (mouse_ਵਰਤਿਆ != ਨਿਰਾ)
	ਸੁਨੇਹਾ_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
    cmdline_ਪੰਕਤੀ = 0;
    save_State = State;
    State = ਬਹੁਲਕ_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_ਸੰਖਿਆ(TRUE, mouse_ਵਰਤਿਆ);
    ਜੇ (ਕੁੰਜੀ_Typed)
    {
	// don't call wait_return() now
	ਜੇ (ਸੁਨੇਹਾ_ਪੰਕਤੀ > 0)
	    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ - 1;
	need_wait_return = FALSE;
	ਸੁਨੇਹਾ_didany = FALSE;
	ਸੁਨੇਹਾ_didout = FALSE;
    }
    ਨਹੀਂਤਾਂ
	cmdline_ਪੰਕਤੀ = save_cmdline_ਪੰਕਤੀ;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ਵਾਪਸ i;
}

    ਖਾਲੀ
msgmore(ਲੰਮਾ n)
{
    ਲੰਮਾ pn;

    ਜੇ (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ਵਾਪਸ;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ਜੇ (keep_ਸੁਨੇਹਾ != ਨਿਰਾ && !keep_ਸੁਨੇਹਾ_ਹੋਰ)
	ਵਾਪਸ;

    ਜੇ (n > 0)
	pn = n;
    ਨਹੀਂਤਾਂ
	pn = -n;

    ਜੇ (pn > p_report)
    {
	ਜੇ (n > 0)
	    vim_snprintf(ਸੁਨੇਹਾ_buf, ਸੁਨੇਹਾ_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ਨਹੀਂਤਾਂ
	    vim_snprintf(ਸੁਨੇਹਾ_buf, ਸੁਨੇਹਾ_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ਜੇ (got_int)
	    vim_strcat((char_u *)ਸੁਨੇਹਾ_buf, (char_u *)_(" (Interrupted)"),
								  ਸੁਨੇਹਾ_BUF_LEN);
	ਜੇ (ਸੁਨੇਹਾ(ਸੁਨੇਹਾ_buf))
	{
	    ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)ਸੁਨੇਹਾ_buf, 0);
	    keep_ਸੁਨੇਹਾ_ਹੋਰ = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    ਖਾਲੀ
beep_flush(ਖਾਲੀ)
{
    ਜੇ (emsg_silent == 0)
    {
	flush_buffers(FLUSH_ਨਿਊਨਤਮ);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    ਖਾਲੀ
vim_beep(ਬਿਨਾਨਿਸ਼ਾਨੀ val)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    called_vim_beep = TRUE;
#ਜੇ_ਅੰਤ

    ਜੇ (emsg_silent != 0 || ਵਿੱਚ_assert_fails)
	ਵਾਪਸ;

    ਜੇ (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	ਸਥਿਰ ਪੂਰਨ		did_init = FALSE;
	ਸਥਿਰ elapsed_T	ਸ਼ੁਰੂ_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ਜੇ (!did_init || ELAPSED_FUNC(ਸ਼ੁਰੂ_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ
	    ਜੇ (p_vb
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		    // While the GUI is starting up the termcap is set for
		    // the GUI but the output still goes to a terminal.
		    && !(gui.ਵਿੱਚ_use && gui.starting)
#ਜੇ_ਅੰਤ
	       )
	    {
		ਬਾਹਰਲਾ_str_cf(T_VB);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VTP
		// No restore color information, refresh the screen.
		ਜੇ (has_vtp_working() != 0
# ifdef FEAT_TERMGUICOLORS
			&& (p_tgc || (!p_tgc && t_colors >= 256))
# endif
		   )
		{
		    redraw_later(UPD_CLEAR);
		    update_screen(0);
		    redrawcmd();
		}
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
		ਬਾਹਰਲਾ_char(BELL);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	}
#ਜੇ_ਅੰਤ
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ਜੇ (vim_strchr(p_debug, 'e') != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
	ਸੁਨੇਹਾ_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    ਖਾਲੀ
init_homedir(ਖਾਲੀ)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ਨਹੀਂਤਾਂ
    var = mch_getenv((char_u *)"HOME");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ਜੇ (var == ਨਿਰਾ || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ਜੇ (homepath == ਨਿਰਾ || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ਜੇ (homedrive != ਨਿਰਾ
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((ਅੱਖਰ *)NameBuff, "%s%s", homedrive, homepath);
	    ਜੇ (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ਜੇ (var == ਨਿਰਾ)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ਜੇ (var != ਨਿਰਾ && *var == '%')
    {
	char_u	*p;
	char_u	*ਘਾਤਾਂਕ;

	p = vim_strchr(var + 1, '%');
	ਜੇ (p != ਨਿਰਾ)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    ਘਾਤਾਂਕ = mch_getenv(NameBuff);
	    ਜੇ (ਘਾਤਾਂਕ != ਨਿਰਾ && *ਘਾਤਾਂਕ != NUL
					&& STRLEN(ਘਾਤਾਂਕ) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((ਅੱਖਰ *)NameBuff, MAXPATHL, "%s%s", ਘਾਤਾਂਕ, p + 1);
		var = NameBuff;
	    }
	}
    }

    ਜੇ (var != ਨਿਰਾ && *var == NUL)	// empty is same as not set
	var = ਨਿਰਾ;

    ਜੇ (enc_utf8 && var != ਨਿਰਾ)
    {
	ਪੂਰਨ	len;
	char_u  *pp = ਨਿਰਾ;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_ਮੰਜ਼ਿਲ_enc(var, (ਪੂਰਨ)STRLEN(var), &pp, &len);
	ਜੇ (pp != ਨਿਰਾ)
	{
	    homedir = pp;
	    ਵਾਪਸ;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ਜੇ (var == ਨਿਰਾ)
	var = (char_u *)"C:/";
#ਜੇ_ਅੰਤ

    ਜੇ (var != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ਜੇ (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((ਅੱਖਰ *)NameBuff) == 0)
	{
	    ਜੇ (!mch_chdir((ਅੱਖਰ *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ਜੇ (mch_chdir((ਅੱਖਰ *)NameBuff) != 0)
		emsg(_(e_cannot_go_ਪਿੱਠ_ਮੰਜ਼ਿਲ_ਪਿਛਲਾ_directory));
	}
#ਜੇ_ਅੰਤ
	homedir = vim_strsave(var);
    }
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_homedir(ਖਾਲੀ)
{
    vim_free(homedir);
}

    ਖਾਲੀ
free_users(ਖਾਲੀ)
{
    ga_clear_ਲੜੀਆਂ(&ga_users);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    ਖਾਲੀ
init_vimdir(ਖਾਲੀ)
{
    ਪੂਰਨ	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ਜੇ (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ਜੇ (mustfree)
	vim_free(p);
}
#ਜੇ_ਅੰਤ

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(char_u *src)
{
    ਵਾਪਸ expand_env_save_opt(src, FALSE, ਨਿਰਾ);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, ਪੂਰਨ one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ਜੇ (p != ਨਿਰਾ)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ਨਿਰਾ);
    ਵਾਪਸ p;
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
    ਪੂਰਨ		dstlen)		// maximum length of the result
{
    ਵਾਪਸ expand_env_esc(src, dst, dstlen, ਨਿਰਾ, FALSE, ਨਿਰਾ);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    ਪੂਰਨ		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    ਪੂਰਨ		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*ਪੂਛ;
    ਪੂਰਨ		c;
    char_u	*var;
    ਪੂਰਨ		copy_char;
    ਪੂਰਨ		mustfree;	// var was allocated, need to free it later
    ਪੂਰਨ		at_ਸ਼ੁਰੂ = TRUE; // at start of a name
    ਪੂਰਨ		startstr_len = 0;
    char_u	*dst_ਸ਼ੁਰੂ = dst;

    ਜੇ (startstr != ਨਿਰਾ)
	startstr_len = (ਪੂਰਨ)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    ਜਦੋਂ (*src && dstlen > 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Skip over `=expr`.
	ਜੇ (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (ਖਾਲੀ)skip_ਵਿਅੰਜਕ(&src, ਨਿਰਾ);
	    ਜੇ (*src == '`')
		++src;
	    len = src - var;
	    ਜੇ (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (ਪੂਰਨ)len;
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ
	copy_char = TRUE;
	ਜੇ ((*src == '$'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		    && at_ਸ਼ੁਰੂ
#ਜੇ_ਅੰਤ
	   )
#ਜੇ defined(MSWIN)
		|| *src == '%'
#ਜੇ_ਅੰਤ
		|| (*src == '~' && at_ਸ਼ੁਰੂ))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ਜੇ (*src != '~')				// environment var
	    {
		ਪੂਛ = src + 1;
		var = dst;
		c = dstlen - 1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		// Unix has ${var-name} type environment vars
		ਜੇ (*ਪੂਛ == '{' && !vim_isIDc('{'))
		{
		    ਪੂਛ++;	// ignore '{'
		    ਜਦੋਂ (c-- > 0 && *ਪੂਛ && *ਪੂਛ != '}')
			*var++ = *ਪੂਛ++;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    ਜਦੋਂ (c-- > 0 && *ਪੂਛ != NUL && ((vim_isIDc(*ਪੂਛ))
#ਜੇ defined(MSWIN)
			    || (*src == '%' && *ਪੂਛ != '%')
#ਜੇ_ਅੰਤ
			    ))
			*var++ = *ਪੂਛ++;
		}

#ਜੇ defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ਜੇ (src[1] == '{' && *ਪੂਛ != '}')
# ਨਹੀਂਤਾਂ
		ਜੇ (*src == '%' && *ਪੂਛ != '%')
# endif
		    var = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		{
# ifdef UNIX
		    ਜੇ (src[1] == '{')
# ਨਹੀਂਤਾਂ
		    ਜੇ (*src == '%')
# endif
			++ਪੂਛ;
#ਜੇ_ਅੰਤ
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ਜੇ defined(MSWIN) || defined(UNIX)
		}
#ਜੇ_ਅੰਤ
	    }
							// home directory
	    ਨਹੀਂਤਾਂ ਜੇ (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ਨਿਰਾ)
	    {
		var = homedir;
		ਪੂਛ = src + 1;
	    }
	    ਨਹੀਂਤਾਂ					// user directory
	    {
#ਜੇ defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		ਪੂਛ = src;
		var = dst;
		c = dstlen - 1;
		ਜਦੋਂ (	   c-- > 0
			&& *ਪੂਛ
			&& vim_isfilec(*ਪੂਛ)
			&& !vim_ispathsep(*ਪੂਛ))
		    *var++ = *ਪੂਛ++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  ਜੇ defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ਬਣਤਰ passwd *pw = (*dst == NUL)
					? ਨਿਰਾ : getpwnam((ਅੱਖਰ *)dst + 1);

		    var = (pw == ਨਿਰਾ) ? ਨਿਰਾ : (char_u *)pw->pw_dir;
		}
		ਜੇ (var == ਨਿਰਾ)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ਨਿਰਾ,
				WILD_ਜੋੜੋ_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ਨਹੀਂਤਾਂ	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*ਰਸਤਾ, *ਅਗਲਾ_ਰਸਤਾ, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    ਅਗਲਾ_ਰਸਤਾ = paths;
		    ਜਦੋਂ (*ਅਗਲਾ_ਰਸਤਾ)
		    {
			ਲਈ (ਰਸਤਾ = ਅਗਲਾ_ਰਸਤਾ; *ਅਗਲਾ_ਰਸਤਾ && *ਅਗਲਾ_ਰਸਤਾ != ',';
				ਅਗਲਾ_ਰਸਤਾ++);
			ਜੇ (*ਅਗਲਾ_ਰਸਤਾ)
			    *ਅਗਲਾ_ਰਸਤਾ++ = NUL;
			testlen = vim_snprintf_safelen(
			    (ਅੱਖਰ *)test,
			    ਆਕਾਰ(test),
			    "%s/%s",
			    ਰਸਤਾ,
			    dst + 1);
			ਜੇ (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ਜੇ (var != ਨਿਰਾ)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    ਰੋਕੋ;
			}
		    }
		}
# endif // UNIX
#ਨਹੀਂਤਾਂ
		// cannot expand user's home directory, so don't try
		var = ਨਿਰਾ;
		ਪੂਛ = (char_u *)"";	// for gcc
#ਜੇ_ਅੰਤ // UNIX || VMS
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ਜੇ (p_ssl && var != ਨਿਰਾ && vim_strchr(var, '\\') != ਨਿਰਾ)
	    {
		char_u	*p = vim_strsave(var);

		ਜੇ (p != ਨਿਰਾ)
		{
		    ਜੇ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		    forward_slash(var);
		}
	    }
#ਜੇ_ਅੰਤ

	    // If "var" contains any character from "esc_chars", escape it
	    // with a backslash.  The historical use is escaping spaces so
	    // that ":e ~/tt" works when $HOME contains a space.
	    ਜੇ (esc_chars != ਨਿਰਾ && var != ਨਿਰਾ
		    && vim_strpbrk(var, esc_chars) != ਨਿਰਾ)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ਜੇ (p != ਨਿਰਾ)
		{
		    ਜੇ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ਜੇ (var != ਨਿਰਾ && *var != NUL)
	    {
		c = (ਪੂਰਨ)STRLEN(var);

		ਜੇ (c + STRLEN(ਪੂਛ) + 1 < (ਬਿਨਾਨਿਸ਼ਾਨੀ)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ਜੇ (after_pathsep(dst, dst + c)
#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#ਜੇ_ਅੰਤ
			    && vim_ispathsep(*ਪੂਛ))
			++ਪੂਛ;
		    dst += c;
		    src = ਪੂਛ;
		    copy_char = FALSE;
		}
	    }
	    ਜੇ (mustfree)
		vim_free(var);
	}

	ਜੇ (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_ਸ਼ੁਰੂ = FALSE;
	    ਜੇ (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((src[0] == ' ' || src[0] == ',') && !one)
		at_ਸ਼ੁਰੂ = TRUE;
	    ਜੇ (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ਜੇ (startstr != ਨਿਰਾ && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_ਸ਼ੁਰੂ = TRUE;
	    }
	}

    }
    *dst = NUL;

    ਵਾਪਸ (size_t)(dst - dst_ਸ਼ੁਰੂ);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ਸਥਿਰ char_u *
remove_ਪੂਛ(char_u *p, char_u *pend, char_u *name)
{
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ਜੇ (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ਵਾਪਸ newend;
    ਵਾਪਸ pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ਸਥਿਰ char_u *
vim_ਸੰਸਕਰਣ_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ਜੇ (vimdir == ਨਿਰਾ || *vimdir == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_ਸੰਸਕਰਣ_NODOT, STRLEN_LITERAL(VIM_ਸੰਸਕਰਣ_NODOT), TRUE, &p);
    ਜੇ (p.string != ਨਿਰਾ && mch_isdir(p.string))
	ਵਾਪਸ p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ਜੇ (p.string != ਨਿਰਾ && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.ਲੰਮਾਈ,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ਜੇ (fname.string != ਨਿਰਾ)
	{
	    ਪੂਰਨ exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ਜੇ (exists)
		ਵਾਪਸ p.string;
	}
    }
    vim_free(p.string);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, ਪੂਰਨ *mustfree)
{
    char_u	*p = ਨਿਰਾ;
    char_u	*pend;
    ਪੂਰਨ		vimruntime;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ਜੇ (STRCMP(name, "HOME") == 0)
	ਵਾਪਸ homedir;

    // Use Wide function
    wn = enc_ਮੰਜ਼ਿਲ_utf16(name, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    wp = _wgetenv(wn);
    vim_free(wn);

    ਜੇ (wp != ਨਿਰਾ && *wp == NUL)   // empty is the same as not set
	wp = ਨਿਰਾ;

    ਜੇ (wp != ਨਿਰਾ)
    {
	p = utf16_ਮੰਜ਼ਿਲ_enc(wp, ਨਿਰਾ);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	*mustfree = TRUE;
	ਵਾਪਸ p;
    }
#ਨਹੀਂਤਾਂ
    p = mch_getenv(name);
    ਜੇ (p != ਨਿਰਾ && *p == NUL)	    // empty is the same as not set
	p = ਨਿਰਾ;

    ਜੇ (p != ਨਿਰਾ)
	ਵਾਪਸ p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ਜੇ (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ਸਥਿਰ ਅੱਖਰ user_Settings_ਰਸਤਾ[MAXPATHL];

	ਜੇ (ਲੱਭੋ_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   user_Settings_ਰਸਤਾ, MAXPATHL) == B_OK)
	    ਵਾਪਸ (char_u *)user_Settings_ਰਸਤਾ;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ਨਿਰਾ;
    }
# endif
#ਜੇ_ਅੰਤ

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ਜੇ (!vimruntime && STRCMP(name, "VIM") != 0)
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ਜੇ (vimruntime
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#ਜੇ_ਅੰਤ
       )
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ਜੇ (wp != ਨਿਰਾ && *wp == NUL)	    // empty is the same as not set
	    wp = ਨਿਰਾ;
	ਜੇ (wp != ਨਿਰਾ)
	{
	    char_u *q = utf16_ਮੰਜ਼ਿਲ_enc(wp, ਨਿਰਾ);
	    ਜੇ (q != ਨਿਰਾ)
	    {
		p = vim_ਸੰਸਕਰਣ_dir(q);
		*mustfree = TRUE;
		ਜੇ (p == ਨਿਰਾ)
		    p = q;
	    }
	}
#ਨਹੀਂਤਾਂ
	p = mch_getenv((char_u *)"VIM");
	ਜੇ (p != ਨਿਰਾ && *p == NUL)	    // empty is the same as not set
	    p = ਨਿਰਾ;
	ਜੇ (p != ਨਿਰਾ)
	{
	    p = vim_ਸੰਸਕਰਣ_dir(p);
	    ਜੇ (p != ਨਿਰਾ)
		*mustfree = TRUE;
	    ਨਹੀਂਤਾਂ
		p = mch_getenv((char_u *)"VIM");
	}
#ਜੇ_ਅੰਤ
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ਜੇ (p == ਨਿਰਾ)
    {
	ਜੇ (p_hf != ਨਿਰਾ && vim_strchr(p_hf, '$') == ਨਿਰਾ)
	    p = p_hf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ਨਹੀਂਤਾਂ
	    p = exe_name;
#ਜੇ_ਅੰਤ
	ਜੇ (p != ਨਿਰਾ)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ਜੇ (p == p_hf)
		pend = remove_ਪੂਛ(p, pend, (char_u *)"doc");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ਜੇ (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_ਪੂਛ(p, pend, (char_u *)"MacOS");
		ਜੇ (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ਜੇ (pnew != ਨਿਰਾ)
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
	    ਜੇ (p == exe_name)
		pend = remove_ਪੂਛ(p, pend, (char_u *)"src");
#ਜੇ_ਅੰਤ

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ਜੇ (!vimruntime)
	    {
		pend = remove_ਪੂਛ(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_ਪੂਛ(p, pend, (char_u *)VIM_ਸੰਸਕਰਣ_NODOT);
	    }

	    // remove trailing path separator
	    ਜੇ (pend > p && after_pathsep(p, pend))
		--pend;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
	    ਜੇ (p == exe_name || p == p_hf)
#ਜੇ_ਅੰਤ
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ਜੇ (p != ਨਿਰਾ && !mch_isdir(p))
		VIM_CLEAR(p);
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ਜੇ (vimruntime && (pend = vim_ਸੰਸਕਰਣ_dir(p)) != ਨਿਰਾ)
		{
		    vim_free(p);
		    p = pend;
		}
#ਜੇ_ਅੰਤ
		*mustfree = TRUE;
	    }
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PATHDEF
    // When there is a pathdef.c file we can use default_vim_dir and
    // default_vimruntime_dir
    ਜੇ (p == ਨਿਰਾ)
    {
	// Only use default_vimruntime_dir when it is not empty
	ਜੇ (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*default_vim_dir != NUL)
	{
	    ਜੇ (vimruntime && (p = vim_ਸੰਸਕਰਣ_dir(default_vim_dir)) != ਨਿਰਾ)
		*mustfree = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		p = default_vim_dir;
		*mustfree = FALSE;
	    }
	}
    }
#ਜੇ_ਅੰਤ

    /*
     * Set the environment variable, so that the new value can be found fast
     * next time, and others can also use it (e.g. Perl).
     */
    ਜੇ (p != ਨਿਰਾ)
    {
	ਜੇ (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ਵਾਪਸ p;
}

    ਖਾਲੀ
vim_unsetenv(char_u *var)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNSETENV
    unsetenv((ਅੱਖਰ *)var);
#ਨਹੀਂਤਾਂ
    vim_setenv(var, (char_u *)"");
#ਜੇ_ਅੰਤ
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    ਖਾਲੀ
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ਜੇ (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    ਖਾਲੀ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ਜੇ (STRICMP(name, "HOME") == 0)
	init_homedir();
    ਨਹੀਂਤਾਂ ਜੇ (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Our portable version of setenv.
 */
    ਖਾਲੀ
vim_setenv(char_u *name, char_u *val)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SETENV
    mch_setenv((ਅੱਖਰ *)name, (ਅੱਖਰ *)val, 1);
#ਨਹੀਂਤਾਂ
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ਜੇ (envbuf != ਨਿਰਾ)
    {
	sprintf((ਅੱਖਰ *)envbuf, "%s=%s", name, val);
	putenv((ਅੱਖਰ *)envbuf);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ਜੇ (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ਜੇ (buf != ਨਿਰਾ)
	{
	    bindtextdomain(VIMPACKAGE, (ਅੱਖਰ *)buf);
	    vim_free(buf);
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * Function given to ExpandGeneric() to obtain an environment variable name.
 */
    char_u *
get_env_name(
    expand_T	*xp ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		idx)
{
#ਜੇ defined(AMIGA)
    // No environ[] on the Amiga.
    ਵਾਪਸ ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    ਬਾਹਰੀ ਅੱਖਰ		**environ;
# endif
    char_u		*str;
    ਪੂਰਨ			n;

    str = (char_u *)environ[idx];
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ਜੇ (str[n] == '=' || str[n] == NUL)
	    ਰੋਕੋ;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ਵਾਪਸ xp->xp_buf;
#ਜੇ_ਅੰਤ
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_user(char_u *user, ਪੂਰਨ need_copy)
{
    char_u	*user_copy = (user != ਨਿਰਾ && need_copy)
						    ? vim_strsave(user) : user;

    ਜੇ (user_copy == ਨਿਰਾ || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ਜੇ (need_copy)
	    vim_free(user_copy);
	ਵਾਪਸ;
    }
    ((char_u **)(ga_users.ga_ਅੰਕੜੇ))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ਸਥਿਰ ਖਾਲੀ
init_users(ਖਾਲੀ)
{
    ਸਥਿਰ ਪੂਰਨ	lazy_init_ਪੂਰਾ = FALSE;

    ਜੇ (lazy_init_ਪੂਰਾ)
	ਵਾਪਸ;

    lazy_init_ਪੂਰਾ = TRUE;
    ga_init2(&ga_users, ਆਕਾਰ(char_u *), 20);

#ਜੇ defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ਬਣਤਰ passwd*	pw;

	setpwent();
	ਜਦੋਂ ((pw = getpwent()) != ਨਿਰਾ)
	    ਜੋੜੋ_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#ਜਾਂਜੇ defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ਜੇ (NetUserEnum(ਨਿਰਾ, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_ਲੰਮਾਈ,
				       &nusers, &ntotal, ਨਿਰਾ) == NERR_Success)
	{
	    ਲਈ (i = 0; i < nusers; i++)
		ਜੋੜੋ_user(utf16_ਮੰਜ਼ਿਲ_enc(uinfo[i].usri0_name, ਨਿਰਾ), FALSE);

	    Net_Api_ਬਫਰ_Free(uinfo);
	}
    }
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ਜੇ (user_env != ਨਿਰਾ && *user_env != NUL)
	{
	    ਪੂਰਨ	i;

	    ਲਈ (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_ਅੰਕੜੇ)[i];

		ਜੇ (STRCMP(local_user, user_env) == 0)
		    ਰੋਕੋ;
	    }

	    ਜੇ (i == ga_users.ga_len)
	    {
		ਬਣਤਰ passwd	*pw = getpwnam((ਅੱਖਰ *)user_env);

		ਜੇ (pw != ਨਿਰਾ)
		    ਜੋੜੋ_user((char_u *)pw->pw_name, TRUE);
	    }
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * Function given to ExpandGeneric() to obtain user names.
 */
    char_u*
get_users(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    init_users();
    ਜੇ (idx < ga_users.ga_len)
	ਵਾਪਸ ((char_u **)ga_users.ga_ਅੰਕੜੇ)[idx];
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    ਪੂਰਨ
ਮੇਲ_user(char_u *name)
{
    ਪੂਰਨ i;
    ਪੂਰਨ n = (ਪੂਰਨ)STRLEN(name);
    ਪੂਰਨ ਨਤੀਜਾ = 0;

    init_users();
    ਲਈ (i = 0; i < ga_users.ga_len; i++)
    {
	ਜੇ (STRCMP(((char_u **)ga_users.ga_ਅੰਕੜੇ)[i], name) == 0)
	    ਵਾਪਸ 2; // full match
	ਜੇ (STRNCMP(((char_u **)ga_users.ga_ਅੰਕੜੇ)[i], name, n) == 0)
	    ਨਤੀਜਾ = 1; // partial match
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

    ਸਥਿਰ ਖਾਲੀ
prepare_ਮੰਜ਼ਿਲ_exit(ਖਾਲੀ)
{
#ਜੇ defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	gui.dying = true;
	ਬਾਹਰਲਾ_trash();	// trash any pending output
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ਜੇ (!ਭਰਿਆ_screen)
	    mch_setmouse(FALSE);

	/*
	 * Switch terminal mode back now, so messages end up on the "normal"
	 * screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
	stoptermcap();
	ਬਾਹਰਲਾ_flush();
    }
}

/*
 * Preserve files and exit.
 * When called IObuff must contain a message.
 * NOTE: This may be called from deathtrap() in a signal handler, avoid unsafe
 * functions, such as allocating memory.
 */
    ਖਾਲੀ
preserve_exit(ਖਾਲੀ)
{
    buf_T	*buf;

    prepare_ਮੰਜ਼ਿਲ_exit();

    // Setting this will prevent free() calls.  That avoids calling free()
    // recursively when free() was invoked with a bad pointer.
    really_exiting = TRUE;

    ਬਾਹਰਲਾ_str(IObuff);
    screen_ਸ਼ੁਰੂ();		    // don't know where cursor is now
    ਬਾਹਰਲਾ_flush();

    ml_close_notmod();		    // close all not-modified buffers

    FOR_ALL_BUFFERS(buf)
    {
	ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ && buf->b_ml.ml_mfp->mf_fname != ਨਿਰਾ)
	{
	    ਬਾਹਰਲਾ_STR("Vim: preserving files...\r\n");
	    screen_ਸ਼ੁਰੂ();	    // don't know where cursor is now
	    ਬਾਹਰਲਾ_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    ਰੋਕੋ;
	}
    }

    ml_close_all(FALSE);	    // close all memfiles, without deleting

    ਬਾਹਰਲਾ_STR("Vim: Finished.\r\n");

    getout(1);
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BREAKCHECK_SKIP
# define BREAKCHECK_SKIP 1000
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	breakcheck_ਗਿਣਤੀ = 0;

    ਖਾਲੀ
ਸਤਰ_breakcheck(ਖਾਲੀ)
{
    ਜੇ (++breakcheck_ਗਿਣਤੀ >= BREAKCHECK_SKIP)
    {
	breakcheck_ਗਿਣਤੀ = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    ਖਾਲੀ
fast_breakcheck(ਖਾਲੀ)
{
    ਜੇ (++breakcheck_ਗਿਣਤੀ >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_ਗਿਣਤੀ = 0;
	ui_breakcheck();
    }
}

#ਜੇ defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    ਖਾਲੀ
veryfast_breakcheck(ਖਾਲੀ)
{
    ਜੇ (++breakcheck_ਗਿਣਤੀ >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_ਗਿਣਤੀ = 0;
	ui_breakcheck();
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    ਪੂਰਨ		flags,		// can be SHELL_SILENT
    ਪੂਰਨ		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*ਬਫਰ = ਨਿਰਾ;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i = 0;
    FILE	*fd;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ ਨਿਰਾ;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ਜੇ ((tempname = vim_tempname('o', TRUE)) == ਨਿਰਾ)
    {
	emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ਜੇ (command == ਨਿਰਾ)
	ਜਾਓ ਪੂਰਾ;

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
    fd = mch_fopen((ਅੱਖਰ *)tempname, "r");
# ਨਹੀਂਤਾਂ
    fd = mch_fopen((ਅੱਖਰ *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ਜੇ (fd == ਨਿਰਾ
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ਜੇ (fd != ਨਿਰਾ)
	    fclose(fd);
	ਜਾਓ ਪੂਰਾ;
    }

    ਬਫਰ = alloc(len + 1);
    ਜੇ (ਬਫਰ != ਨਿਰਾ)
	i = (ਪੂਰਨ)fread((ਅੱਖਰ *)ਬਫਰ, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਜਾਓ ਪੂਰਾ;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ਜੇ (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(ਬਫਰ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret_len == ਨਿਰਾ)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	ਲਈ (i = 0; i < len; ++i)
	    ਜੇ (ਬਫਰ[i] == NUL)
		ਬਫਰ[i] = 1;

	ਬਫਰ[len] = NUL;	// make sure the buffer is terminated
    }
    ਨਹੀਂਤਾਂ
	*ret_len = len;

ਪੂਰਾ:
    mch_remove(tempname);
    vim_free(tempname);
    ਵਾਪਸ ਬਫਰ;
}

# ਜੇ defined(FEAT_EVAL)

    ਸਥਿਰ ਖਾਲੀ
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    ਪੂਰਨ		retlist)
{
    char_u	*res = ਨਿਰਾ;
    char_u	*p;
    char_u	*infile = ਨਿਰਾ;
    ਪੂਰਨ		err = FALSE;
    FILE	*fd;
    list_T	*list = ਨਿਰਾ;
    ਪੂਰਨ		flags = SHELL_SILENT;
    ਪੂਰਨ		use_argv = FALSE;
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    ਪੂਰਨ		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
    ਜੇ (check_restricted() || check_secure())
	ਜਾਓ errret;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ਸੰਖਿਆ_or_list_arg(argvars, 1)
								      == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ਜੇ ((infile = vim_tempname('i', TRUE)) == ਨਿਰਾ)
	{
	    emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	    ਜਾਓ errret;
	}

	fd = mch_fopen((ਅੱਖਰ *)infile, WRITEBIN);
	ਜੇ (fd == ਨਿਰਾ)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    ਜਾਓ errret;
	}
	ਜੇ (argvars[1].v_type == VAR_ਸੰਖਿਆ)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_ਸੰਖਿਆ);
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		semsg(_(e_ਬਫਰ_nr_does_not_exist), argvars[1].vval.v_ਸੰਖਿਆ);
		fclose(fd);
		ਜਾਓ errret;
	    }

	    ਲਈ (lnum = 1; lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; lnum++)
	    {
		ਲਈ (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ਜੇ (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			ਰੋਕੋ;
		    }
		ਜੇ (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_LIST)
	{
	    ਜੇ (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		fclose(fd);
		ਜਾਓ errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ਜੇ (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ਜੇ (fclose(fd) != 0)
	    err = TRUE;
	ਜੇ (err)
	{
	    emsg(_(e_error_writing_ਆਰਜ਼ੀ_file));
	    ਜਾਓ errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ਜੇ (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ਜੇ (l == ਨਿਰਾ || l->lv_len < 1)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਜਾਓ errret;
	}
	ਜੇ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ਜਾਓ errret;
	ਜੇ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_ਦਲੀਲ));
	    ਜਾਓ errret;
	}
	use_argv = TRUE;

	ਜੇ (p_verbose > 3)
	{
	    ਪੂਰਨ		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    ਲਈ (i = 0; i < argc; ++i)
	    {
		ਜੇ (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ);
	    ਸੁਨੇਹਾ_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ਜੇ (!ਸੁਨੇਹਾ_silent)
	flags += SHELL_COOKED;

    ਜੇ (retlist)
    {
	ਪੂਰਨ		len;
	listitem_T	*li;
	char_u		*s = ਨਿਰਾ;
	char_u		*ਸ਼ੁਰੂ;
	char_u		*ਸਿਰਾ;
	ਪੂਰਨ		i;

	ਜੇ (use_argv)
	    res = mch_get_cmd_output_ਸਿੱਧਾ(argv, infile, flags, &len);
	ਨਹੀਂਤਾਂ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ਜੇ (res == ਨਿਰਾ)
	    ਜਾਓ errret;

	list = list_alloc();
	ਜੇ (list == ਨਿਰਾ)
	    ਜਾਓ errret;

	ਲਈ (i = 0; i < len; ++i)
	{
	    ਸ਼ੁਰੂ = res + i;
	    ਜਦੋਂ (i < len && res[i] != NL)
		++i;
	    ਸਿਰਾ = res + i;

	    s = alloc(ਸਿਰਾ - ਸ਼ੁਰੂ + 1);
	    ਜੇ (s == ਨਿਰਾ)
		ਜਾਓ errret;

	    ਲਈ (p = s; ਸ਼ੁਰੂ < ਸਿਰਾ; ++p, ++ਸ਼ੁਰੂ)
		*p = *ਸ਼ੁਰੂ == NUL ? NL : *ਸ਼ੁਰੂ;
	    *p = NUL;

	    li = listitem_alloc();
	    ਜੇ (li == ਨਿਰਾ)
	    {
		vim_free(s);
		ਜਾਓ errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_ਤਾਲਾ = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_ਸੈੱਟ(rettv, list);
	list = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (use_argv)
	    res = mch_get_cmd_output_ਸਿੱਧਾ(argv, infile, flags, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ਨਿਰਾ);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ਜੇ (res != ਨਿਰਾ)
	{
	    char_u	*s, *d;

	    d = res;
	    ਲਈ (s = res; *s; ++s)
	    {
		ਜੇ (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ਨਿਰਾ;
    }

errret:
    ਜੇ (argv != ਨਿਰਾ)
    {
	ਪੂਰਨ i;
	ਲਈ (i = 0; argv[i] != ਨਿਰਾ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ਜੇ (infile != ਨਿਰਾ)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ਜੇ (res != ਨਿਰਾ)
	vim_free(res);
    ਜੇ (list != ਨਿਰਾ)
	list_free(list);
}

/*
 * "system()" function
 */
    ਖਾਲੀ
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    ਖਾਲੀ
f_systemlist(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, TRUE);
}
# endif // FEAT_EVAL

#ਜੇ_ਅੰਤ

/*
 * Return TRUE when need to go to Insert mode because of 'insertmode'.
 * Don't do this when still processing a command or a mapping.
 * Don't do this when inside a ":normal" command.
 */
    ਪੂਰਨ
goto_im(ਖਾਲੀ)
{
    ਵਾਪਸ (p_im && stuff_ਖਾਲੀ_ਥਾਵਾਂ() && typebuf_typed());
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
get_isolated_shell_name(ਖਾਲੀ)
{
    char_u *p;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ਨਹੀਂਤਾਂ
    p = skiptowhite(p_sh);
    ਜੇ (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ਨਹੀਂਤਾਂ
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	ਲਈ (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ਜੇ (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    ਪੂਰਨ
ਰਸਤਾ_is_url(char_u *p)
{
    ਜੇ (STRNCMP(p, "://", (size_t)3) == 0)
	ਵਾਪਸ URL_SLASH;
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ਵਾਪਸ URL_BACKSLASH;
    ਵਾਪਸ 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    ਪੂਰਨ
ਰਸਤਾ_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ਜੇ (!ASCII_ISALPHA(*fname))
	ਵਾਪਸ 0;

    // check body: alpha or dash
    ਲਈ (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ਜੇ (p[-1] == '-')
	ਵਾਪਸ 0;

    // "://" or ":\\" must follow
    ਵਾਪਸ ਰਸਤਾ_is_url(p);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ਜੇ (v_event->dv_hashtab.ht_ਵਰਤਿਆ > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ਨਹੀਂਤਾਂ
	sve->sve_did_save = FALSE;
    ਵਾਪਸ v_event;
}

    ਖਾਲੀ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ਜੇ (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ਨਹੀਂਤਾਂ
	hash_init(&v_event->dv_hashtab);
}
#ਜੇ_ਅੰਤ

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    ਖਾਲੀ
may_trigger_modechanged(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_ਬਹੁਲਕ[ਬਹੁਲਕ_MAX_ਲੰਮਾਈ];
    char_u	    ਨਮੂਨਾ_buf[2 * ਬਹੁਲਕ_MAX_ਲੰਮਾਈ];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ਜੇ (!has_modechanged() || got_int)
	ਵਾਪਸ;

    get_ਬਹੁਲਕ(curr_ਬਹੁਲਕ);
    ਜੇ (STRCMP(curr_ਬਹੁਲਕ, ਆਖਰੀ_ਬਹੁਲਕ) == 0)
	ਵਾਪਸ;

    v_event = get_v_event(&save_v_event);
    (ਖਾਲੀ)dict_ਜੋੜੋ_string(v_event, "new_mode", curr_ਬਹੁਲਕ);
    (ਖਾਲੀ)dict_ਜੋੜੋ_string(v_event, "old_mode", ਆਖਰੀ_ਬਹੁਲਕ);
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((ਅੱਖਰ *)ਨਮੂਨਾ_buf, ਆਕਾਰ(ਨਮੂਨਾ_buf), "%s:%s", ਆਖਰੀ_ਬਹੁਲਕ,
	    curr_ਬਹੁਲਕ);

    apply_autocmds(EVENT_MODECHANGED, ਨਮੂਨਾ_buf, ਨਿਰਾ, FALSE, curbuf);
    STRCPY(ਆਖਰੀ_ਬਹੁਲਕ, curr_ਬਹੁਲਕ);

    restore_v_event(v_event, &save_v_event);
#ਜੇ_ਅੰਤ
}

// For overflow detection, add a digit safely to a long value.
    ਪੂਰਨ
vim_append_digit_long(ਲੰਮਾ *ਮੁੱਲ, ਪੂਰਨ digit)
{
    ਲੰਮਾ x = *ਮੁੱਲ;
    ਜੇ (x > ((LONG_MAX - (ਲੰਮਾ)digit) / 10))
	ਵਾਪਸ FAIL;
    *ਮੁੱਲ = x * 10 + (ਲੰਮਾ)digit;
    ਵਾਪਸ OK;
}

// Return something that fits into an int.
    ਪੂਰਨ
trim_ਮੰਜ਼ਿਲ_int(vimlong_T x)
{
    ਵਾਪਸ x > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : x < INT_MIN ? INT_MIN : x;
}
