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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ଯଦି defined(__HAIKU__)
# include <storage/FindDirectory.h>
#endif

#ଯଦି defined(MSWIN)
# include <lm.h>
#endif

#define URL_SLASH	1		// path_is_url() has found "://"
#define URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
ସ୍ଥିର garray_T	ga_users;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_leader_len(
    char_u	*ଧାଡ଼ି,
    char_u	**flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		backward,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include_space)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_com = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ମଧ୍ୟ_match_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ଖାଲି;

    result = i = 0;
    ଯେତେବେଳେ (VIM_ISWHITE(ଧାଡ଼ି[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    ଯେତେବେଳେ (ଧାଡ଼ି[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	ପାଇଁ (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ଯଦି (!got_com && flags != ଖାଲି)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (ଶୂନ୍ୟ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ଯଦି (string == ଖାଲି)	    // missing ':', ignore this part
		ଜାରି;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ଯଦି (ମଧ୍ୟ_match_len != 0
		    && vim_strchr(part_buf, COM_ମଧ୍ୟ) == ଖାଲି
		    && vim_strchr(part_buf, COM_END) == ଖାଲି)
		ଭାଙ୍ଗ;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ଯଦି (got_com && vim_strchr(part_buf, COM_NEST) == ଖାଲି)
		ଜାରି;

	    // When 'O' flag present and using "O" command skip this one.
	    ଯଦି (backward && vim_strchr(part_buf, COM_NOBACK) != ଖାଲି)
		ଜାରି;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ଯଦି (VIM_ISWHITE(string[0]))
	    {
		ଯଦି (i == 0 || !VIM_ISWHITE(ଧାଡ଼ି[i - 1]))
		    ଜାରି;  // missing white space
		ଯେତେବେଳେ (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    ପାଇଁ (j = 0; string[j] != NUL && string[j] == ଧାଡ଼ି[i + j]; ++j)
		;
	    ଯଦି (string[j] != NUL)
		ଜାରି;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ଯଦି (vim_strchr(part_buf, COM_BLANK) != ଖାଲି
			   && !VIM_ISWHITE(ଧାଡ଼ି[i + j]) && ଧାଡ଼ି[i + j] != NUL)
		ଜାରି;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ଯଦି (vim_strchr(part_buf, COM_ମଧ୍ୟ) != ଖାଲି)
	    {
		ଯଦି (ମଧ୍ୟ_match_len == 0)
		{
		    ମଧ୍ୟ_match_len = j;
		    saved_flags = prev_list;
		}
		ଜାରି;
	    }
	    ଯଦି (ମଧ୍ୟ_match_len != 0 && j > ମଧ୍ୟ_match_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		ମଧ୍ୟ_match_len = 0;

	    ଯଦି (ମଧ୍ୟ_match_len == 0)
		i += j;
	    found_one = TRUE;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (ମଧ୍ୟ_match_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ଯଦି (!got_com && flags != ଖାଲି)
		*flags = saved_flags;
	    i += ମଧ୍ୟ_match_len;
	    found_one = TRUE;
	}

	// No match found, stop scanning.
	ଯଦି (!found_one)
	    ଭାଙ୍ଗ;

	result = i;

	// Include any trailing white space.
	ଯେତେବେଳେ (VIM_ISWHITE(ଧାଡ଼ି[i]))
	    ++i;

	ଯଦି (include_space)
	    result = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ଯଦି (vim_strchr(part_buf, COM_NEST) == ଖାଲି)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ result;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_last_leader_offset(char_u *ଧାଡ଼ି, char_u **flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lower_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଧାଡ଼ି);
    ଯେତେବେଳେ (--i >= lower_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	ପାଇଁ (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (ଶୂନ୍ୟ)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ଯଦି (string == ଖାଲି)	// If everything is fine, this cannot actually
				// happen.
		ଜାରି;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ଯଦି (VIM_ISWHITE(string[0]))
	    {
		ଯଦି (i == 0 || !VIM_ISWHITE(ଧାଡ଼ି[i - 1]))
		    ଜାରି;
		ଯେତେବେଳେ (VIM_ISWHITE(*string))
		    ++string;
	    }
	    ପାଇଁ (j = 0; string[j] != NUL && string[j] == ଧାଡ଼ି[i + j]; ++j)
		/* do nothing */;
	    ଯଦି (string[j] != NUL)
		ଜାରି;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ଯଦି (vim_strchr(part_buf, COM_BLANK) != ଖାଲି
		    && !VIM_ISWHITE(ଧାଡ଼ି[i + j]) && ଧାଡ଼ି[i + j] != NUL)
		ଜାରି;

	    ଯଦି (vim_strchr(part_buf, COM_ମଧ୍ୟ) != ଖାଲି)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		ପାଇଁ (j = 0; VIM_ISWHITE(ଧାଡ଼ି[j]) && j <= i; j++)
		    ;
		ଯଦି (j < i)
		    ଜାରି;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    found_one = TRUE;

	    ଯଦି (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    ଭାଙ୍ଗ;
	}

	ଯଦି (found_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     len1, len2, off;

	    result = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ଯଦି (vim_strchr(part_buf, COM_NEST) != ଖାଲି)
		ଜାରି;

	    lower_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    ଯେତେବେଳେ (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(com_leader);

	    ପାଇଁ (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(ଶୂନ୍ୟ)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ଯଦି (flags_save == com_flags)
		    ଜାରି;
		string = vim_strchr(part_buf2, ':');
		++string;
		ଯେତେବେଳେ (VIM_ISWHITE(*string))
		    ++string;
		len2 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(string);
		ଯଦି (len2 == 0)
		    ଜାରି;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		ପାଇଁ (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ଯଦି (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ଯଦି (i - off < lower_check_bound)
			    lower_check_bound = i - off;
		    }
		}
	    }
	}
    }
    ଫେରନ୍ତୁ result;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines(linenr_T lnum)
{
    ଫେରନ୍ତୁ plines_win(curwin, lnum, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		limit_winheight)	// when TRUE limit to window height
{
#ଯଦି defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ଫେରନ୍ତୁ plines_win_nofill(wp, lnum, limit_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_nofill(linenr_T lnum)
{
    ଫେରନ୍ତୁ plines_win_nofill(curwin, lnum, TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		limit_winheight)	// when TRUE limit to window height
{
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ିଗୁଡ଼ିକ;

    ଯଦି (wp->w_width == 0)
	ଫେରନ୍ତୁ 1;

#ifdef FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ଯଦି (ଧାଡ଼ି_Folded(wp, lnum) == TRUE)
	ଫେରନ୍ତୁ 1;
#endif

    ଯଦି (!wp->w_p_wrap)
	ଧାଡ଼ିଗୁଡ଼ିକ = 1
#ifdef FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_ଗଣନା_above_below(wp->w_buffer, lnum)
#endif
	;
    ଅନ୍ୟଥା
	ଧାଡ଼ିଗୁଡ଼ିକ = plines_win_nofold(wp, lnum);

    ଯଦି (limit_winheight && ଧାଡ଼ିଗୁଡ଼ିକ > wp->w_height)
	ଫେରନ୍ତୁ wp->w_height;
    ଫେରନ୍ତୁ ଧାଡ଼ିଗୁଡ଼ିକ;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    long	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ଯଦି (*s == NUL
#ifdef FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_text
#endif
	    )
	ଫେରନ୍ତୁ 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ଯଦି (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	col += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    width = wp->w_width - win_col_off(wp);
    ଯଦି (width <= 0)
	ଫେରନ୍ତୁ 32000;
    ଯଦି (col <= width)
	ଫେରନ୍ତୁ 1;
    col -= width;
    width += win_col_off2(wp);
    ଫେରନ୍ତୁ (col + (width - 1)) / width + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_win_col(win_T *wp, linenr_T lnum, long column)
{
    long	col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ିଗୁଡ଼ିକ = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    char_u	*ଧାଡ଼ି;
    chartabsize_T cts;

#ifdef FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    ଧାଡ଼ିଗୁଡ଼ିକ = diff_check_fill(wp, lnum);
#endif

    ଯଦି (!wp->w_p_wrap)
	ଫେରନ୍ତୁ ଧାଡ଼ିଗୁଡ଼ିକ + 1;

    ଯଦି (wp->w_width == 0)
	ଫେରନ୍ତୁ ଧାଡ଼ିଗୁଡ଼ିକ + 1;

    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
    // "column" is a byte index, advance the pointer until it is reached.
    ଯେତେବେଳେ (*cts.cts_ptr != NUL && cts.cts_ptr < ଧାଡ଼ି + column)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ଖାଲି, ଖାଲି);
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
    ଯଦି (*cts.cts_ptr == TAB && (State & MODE_NORMAL)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	col += win_lbr_chartabsize(&cts, ଖାଲି, ଖାଲି) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    width = wp->w_width - win_col_off(wp);
    ଯଦି (width <= 0)
	ଫେରନ୍ତୁ 9999;

    ଧାଡ଼ିଗୁଡ଼ିକ += 1;
    ଯଦି (col > width)
	ଧାଡ଼ିଗୁଡ଼ିକ += (col - width) / (width + win_col_off2(wp)) + 1;
    ଫେରନ୍ତୁ ଧାଡ଼ିଗୁଡ଼ିକ;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plines_m_win(win_T *wp, linenr_T first, linenr_T last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    ଯେତେବେଳେ (first <= last && ଗଣନା_2 < max)
    {
#ifdef FEAT_FOLDING
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_ଗଣନା(wp, first, ଖାଲି);
	ଯଦି (x > 0)
	{
	    ++ଗଣନା_2;	    // count 1 for "+-- folded" line
	    first += x;
	}
	ଅନ୍ୟଥା
#endif
	{
#ifdef FEAT_DIFF
	    ଯଦି (first == wp->w_topline)
		ଗଣନା_2 += plines_win_nofill(wp, first, FALSE) + wp->w_topfill;
	    ଅନ୍ୟଥା
#endif
		ଗଣନା_2 += plines_win(wp, first, FALSE);
	    ++first;
	}
    }
#ifdef FEAT_DIFF
    ଯଦି (first == wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	ଗଣନା_2 += diff_check_fill(wp, first);
#endif
    ଫେରନ୍ତୁ MIN(max, ଗଣନା_2);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ଯଦି (pos->col == MAXCOL)
	ଫେରନ୍ତୁ NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ଯଦି (pos->col > ptrlen)
	ଫେରନ୍ତୁ NUL;
    ଯଦି (has_mbyte)
	ଫେରନ୍ତୁ (*mb_ptr2char)(ptr);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*ptr;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gchar_cursor(ଶୂନ୍ୟ)
{
    ଯଦି (has_mbyte)
	ଫେରନ୍ତୁ (*mb_ptr2char)(ml_get_cursor());
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
char_before_cursor(ଶୂନ୍ୟ)
{
    ଯଦି (curwin->w_cursor.col == 0)
	ଫେରନ୍ତୁ -1;

    char_u *ଧାଡ଼ି = ml_get_curline();

    ଯଦି (has_mbyte)
    {
	char_u *p = ଧାଡ଼ି + curwin->w_cursor.col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_len = (*mb_head_off)(ଧାଡ଼ି, p - 1) + 1;
	ଫେରନ୍ତୁ mb_ptr2char(p - prev_len);
    }
    ଫେରନ୍ତୁ ଧାଡ଼ି[curwin->w_cursor.col - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    ଶୂନ୍ୟ
pchar_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
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
    ଯଦି (*p == ',')
	++p;
    ଯେତେବେଳେ (*p == ' ')
	++p;
    ଫେରନ୍ତୁ p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    ଶୂନ୍ୟ
check_status(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer == buf && wp->w_status_height)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ask_yesno(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା direct)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r = ' ';
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_State = State;

    ଯଦି (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    State = MODE_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;		// no mapping here, but recognize keys

    ଯେତେବେଳେ (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ଯଦି (direct)
	    r = get_keystroke();
	ଅନ୍ୟଥା
	    r = plain_vgetc();
	ଯଦି (r == Ctrl_C || r == ESC)
	    r = 'n';
	ସନ୍ଦେଶ_putchar(r);	    // show what you typed
	out_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;

    ଫେରନ୍ତୁ r;
}

#ଯଦି defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    ଶୂନ୍ୟ
get_mode(char_u *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    ଯଦି (time_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    ଅନ୍ୟଥା ଯଦି (term_use_loop())
    {
	ଯଦି (State & MODE_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    ଅନ୍ୟଥା ଯଦି (State == MODE_HITRETURN || State == MODE_ASKMORE
						      || State == MODE_SETWSIZE
		|| State == MODE_CONFIRM)
    {
	buf[i++] = 'r';
	ଯଦି (State == MODE_ASKMORE)
	    buf[i++] = 'm';
	ଅନ୍ୟଥା ଯଦି (State == MODE_CONFIRM)
	    buf[i++] = '?';
    }
    ଅନ୍ୟଥା ଯଦି (State == MODE_EXTERNCMD)
	buf[i++] = '!';
    ଅନ୍ୟଥା ଯଦି (State & MODE_ଭର୍ତ୍ତି)
    {
	ଯଦି (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    ଅନ୍ୟଥା
		buf[i++] = 'i';
	}

	ଯଦି (ins_compl_active())
	    buf[i++] = 'c';
	ଅନ୍ୟଥା ଯଦି (ctrl_x_mode_not_defined_yet())
	    buf[i++] = 'x';
    }
    ଅନ୍ୟଥା ଯଦି ((State & MODE_CMDLINE) || exmode_active)
    {
	buf[i++] = 'c';
	ଯଦି (exmode_active == EXMODE_VIM)
	    buf[i++] = 'v';
	ଅନ୍ୟଥା ଯଦି (exmode_active == EXMODE_NORMAL)
	    buf[i++] = 'e';
	ଯଦି ((State & MODE_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    ଅନ୍ୟଥା ଯଦି (VIsual_active)
    {
	ଯଦି (VIsual_select)
	    buf[i++] = VIsual_mode + 's' - 'v';
	ଅନ୍ୟଥା
	{
	    buf[i++] = VIsual_mode;
	    ଯଦି (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    ଅନ୍ୟଥା
    {
	buf[i++] = 'n';
	ଯଦି (finish_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	ଅନ୍ୟଥା ଯଦି (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	ଅନ୍ୟଥା ଯଦି (term_in_normal_mode(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    ଶୂନ୍ୟ
f_mode(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[MODE_MAX_LENGTH];

    ଯଦି (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    get_mode(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ଯଦି (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    ସ୍ଥିର ଶୂନ୍ୟ
may_add_state_char(garray_T *gap, char_u *include, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (include == ଖାଲି || vim_strchr(include, c) != ଖାଲି)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    ଶୂନ୍ୟ
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ga_init2(&ga, 1, 20);
    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ଯଦି (!(stuff_empty() && typebuf.tb_len == 0 && scriptin[curscript] == ଖାଲି))
	may_add_state_char(&ga, include, 'm');
    ଯଦି (op_pending())
	may_add_state_char(&ga, include, 'o');
    ଯଦି (autocmd_busy)
	may_add_state_char(&ga, include, 'x');
    ଯଦି (ins_compl_active())
	may_add_state_char(&ga, include, 'a');

# ifdef FEAT_JOB_CHANNEL
    ଯଦି (channel_in_blocking_wait())
	may_add_state_char(&ga, include, 'w');
# endif
    ଯଦି (!get_was_safe_state())
	may_add_state_char(&ga, include, 'S');
    ପାଇଁ (i = 0; i < get_callback_depth() && i < 3; ++i)
	may_add_state_char(&ga, include, 'c');
    ଯଦି (ସନ୍ଦେଶ_scrolled > 0)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_keystroke(ଶୂନ୍ୟ)
{
    char_u	*buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen = 150;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_mapped_ctrl_c = mapped_ctrl_c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		waited = 0;

    mod_mask = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    ପାଇଁ (;;)
    {
	cursor_on();
	out_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ଯଦି (buf == ଖାଲି)
	    buf = alloc(buflen);
	ଅନ୍ୟଥା ଯଦି (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ଯଦି (buf == ଖାଲି)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ଯଦି (buf == ଖାଲି)
	{
	    do_outofmem_ସନ୍ଦେଶ((long_u)buflen);
	    ଫେରନ୍ତୁ ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ଯଦି (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_ନିବେଶ_buffer(buf + len, n);
	    len += n;
	    waited = 0;
	}
	ଅନ୍ୟଥା ଯଦି (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ଯଦି ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    ଜାରି;

	ଯଦି (n == KEYLEN_REMOVED)  // key code removed
	{
	    ଯଦି (must_redraw != 0 && !need_wait_return && (State
			& (MODE_CMDLINE | MODE_HITRETURN | MODE_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    ଜାରି;
	}
	ଯଦି (n > 0)		// found a termcode: adjust length
	    len = n;
	ଯଦି (len == 0)		// nothing typed yet
	    ଜାରି;

	// Handle modifier and/or special key code.
	n = buf[0];
	ଯଦି (n == K_SPECIAL)
	{
	    n = TO_SPECIAL(buf[1], buf[2]);
	    ଯଦି (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_ଚାବି(n) && n != K_LEFTMOUSE)
#ifdef FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#endif
	       )
	    {
		ଯଦି (buf[1] == KS_MODIFIER)
		    mod_mask = buf[2];
		len -= 3;
		ଯଦି (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		ଜାରି;
	    }
	    ଭାଙ୍ଗ;
	}
	ଯଦି (has_mbyte)
	{
	    ଯଦି (MB_BYTE2LEN(n) > len)
		ଜାରି;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#ifdef UNIX
	ଯଦି (n == intr_char)
	    n = ESC;
#endif
	ଭାଙ୍ଗ;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    ଫେରନ୍ତୁ merge_modify_Other_ଚାବିଗୁଡ଼ିକ(n, &mod_mask);
}

// For overflow detection, add a digit safely to an int value.
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_append_digit_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା digit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x = *ମୂଲ୍ୟ;
    ଯଦି (x > ((INT_MAX - digit) / 10))
	ଫେରନ୍ତୁ FAIL;
    *ମୂଲ୍ୟ = x * 10 + digit;
    ଫେରନ୍ତୁ OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_ସଂଖ୍ୟା(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    colon,			// allow colon to abort
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *mouse_used)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା typed = 0;

    ଯଦି (mouse_used != ଖାଲି)
	*mouse_used = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ଯଦି (ସନ୍ଦେଶ_silent != 0)
	ଫେରନ୍ତୁ 0;

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;		// no mapping here, but recognize keys
    ପାଇଁ (;;)
    {
	windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
	c = safe_vgetc();
	ଯଦି (VIM_ISDIGIT(c))
	{
	    ଯଦି (vim_append_digit_int(&n, c - '0') == FAIL)
		ଫେରନ୍ତୁ 0;
	    ସନ୍ଦେଶ_putchar(c);
	    ++typed;
	}
	ଅନ୍ୟଥା ଯଦି (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ଯଦି (typed > 0)
	    {
		ସନ୍ଦେଶ_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	ଅନ୍ୟଥା ଯଦି (mouse_used != ଖାଲି && c == K_LEFTMOUSE)
	{
	    *mouse_used = TRUE;
	    n = mouse_row + 1;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ଯଦି (!exmode_active)
		cmdline_row = ସନ୍ଦେଶ_row;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (c == CAR || c == NL )
	    ଭାଙ୍ଗ;
    }
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
    ଫେରନ୍ତୁ n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prompt_for_ସଂଖ୍ୟା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mouse_used)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmdline_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State;

    // When using ":silent" assume that <CR> was entered.
    ଯଦି (mouse_used != ଖାଲି)
	ସନ୍ଦେଶ_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_row = cmdline_row;
    cmdline_row = 0;
    save_State = State;
    State = MODE_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_ସଂଖ୍ୟା(TRUE, mouse_used);
    ଯଦି (ଚାବି_Typed)
    {
	// don't call wait_return() now
	ଯଦି (ସନ୍ଦେଶ_row > 0)
	    cmdline_row = ସନ୍ଦେଶ_row - 1;
	need_wait_return = FALSE;
	ସନ୍ଦେଶ_didany = FALSE;
	ସନ୍ଦେଶ_didout = FALSE;
    }
    ଅନ୍ୟଥା
	cmdline_row = save_cmdline_row;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    ଫେରନ୍ତୁ i;
}

    ଶୂନ୍ୟ
msgmore(long n)
{
    long pn;

    ଯଦି (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	ଫେରନ୍ତୁ;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ଯଦି (keep_ସନ୍ଦେଶ != ଖାଲି && !keep_ସନ୍ଦେଶ_more)
	ଫେରନ୍ତୁ;

    ଯଦି (n > 0)
	pn = n;
    ଅନ୍ୟଥା
	pn = -n;

    ଯଦି (pn > p_report)
    {
	ଯଦି (n > 0)
	    vim_snprintf(ସନ୍ଦେଶ_buf, ସନ୍ଦେଶ_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	ଅନ୍ୟଥା
	    vim_snprintf(ସନ୍ଦେଶ_buf, ସନ୍ଦେଶ_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ଯଦି (got_int)
	    vim_strcat((char_u *)ସନ୍ଦେଶ_buf, (char_u *)_(" (Interrupted)"),
								  ସନ୍ଦେଶ_BUF_LEN);
	ଯଦି (ସନ୍ଦେଶ(ସନ୍ଦେଶ_buf))
	{
	    set_keep_ସନ୍ଦେଶ((char_u *)ସନ୍ଦେଶ_buf, 0);
	    keep_ସନ୍ଦେଶ_more = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    ଶୂନ୍ୟ
beep_flush(ଶୂନ୍ୟ)
{
    ଯଦି (emsg_silent == 0)
    {
	flush_buffers(FLUSH_MINIMAL);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    ଶୂନ୍ୟ
vim_beep(unsigned val)
{
#ifdef FEAT_EVAL
    called_vim_beep = TRUE;
#endif

    ଯଦି (emsg_silent != 0 || in_assert_fails)
	ଫେରନ୍ତୁ;

    ଯଦି (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#ifdef ELAPSED_FUNC
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_init = FALSE;
	ସ୍ଥିର elapsed_T	start_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ଯଦି (!did_init || ELAPSED_FUNC(start_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(start_tv);
#endif
	    ଯଦି (p_vb
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
		ଯଦି (has_vtp_working() != 0
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
	    ଅନ୍ୟଥା
		out_char(BELL);
#ifdef ELAPSED_FUNC
	}
#endif
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ଯଦି (vim_strchr(p_debug, 'e') != ଖାଲି)
    {
	ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
	ସନ୍ଦେଶ_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    ଶୂନ୍ୟ
init_homedir(ଶୂନ୍ୟ)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#ifdef VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#ଅନ୍ୟଥା
    var = mch_getenv((char_u *)"HOME");
#endif

#ifdef MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ଯଦି (var == ଖାଲି || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ଯଦି (homepath == ଖାଲି || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ଯଦି (homedrive != ଖାଲି
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((ଅକ୍ଷର *)NameBuff, "%s%s", homedrive, homepath);
	    ଯଦି (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ଯଦି (var == ଖାଲି)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ଯଦି (var != ଖାଲି && *var == '%')
    {
	char_u	*p;
	char_u	*exp;

	p = vim_strchr(var + 1, '%');
	ଯଦି (p != ଖାଲି)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    exp = mch_getenv(NameBuff);
	    ଯଦି (exp != ଖାଲି && *exp != NUL
					&& STRLEN(exp) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((ଅକ୍ଷର *)NameBuff, MAXPATHL, "%s%s", exp, p + 1);
		var = NameBuff;
	    }
	}
    }

    ଯଦି (var != ଖାଲି && *var == NUL)	// empty is same as not set
	var = ଖାଲି;

    ଯଦି (enc_utf8 && var != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;
	char_u  *pp = ଖାଲି;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_to_enc(var, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(var), &pp, &len);
	ଯଦି (pp != ଖାଲି)
	{
	    homedir = pp;
	    ଫେରନ୍ତୁ;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ଯଦି (var == ଖାଲି)
	var = (char_u *)"C:/";
#endif

    ଯଦି (var != ଖାଲି)
    {
#ifdef UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ଯଦି (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((ଅକ୍ଷର *)NameBuff) == 0)
	{
	    ଯଦି (!mch_chdir((ଅକ୍ଷର *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ଯଦି (mch_chdir((ଅକ୍ଷର *)NameBuff) != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#endif
	homedir = vim_strsave(var);
    }
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_homedir(ଶୂନ୍ୟ)
{
    vim_free(homedir);
}

    ଶୂନ୍ୟ
free_users(ଶୂନ୍ୟ)
{
    ga_clear_strings(&ga_users);
}
#endif

#ଯଦି defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    ଶୂନ୍ୟ
init_vimdir(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ଯଦି (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ଯଦି (mustfree)
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
    ଫେରନ୍ତୁ expand_env_save_opt(src, FALSE, ଖାଲି);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ଯଦି (p != ଖାଲି)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ଖାଲି);
    ଫେରନ୍ତୁ p;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dstlen)		// maximum length of the result
{
    ଫେରନ୍ତୁ expand_env_esc(src, dst, dstlen, ଖାଲି, FALSE, ଖାଲି);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*tail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustfree;	// var was allocated, need to free it later
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		at_start = TRUE; // at start of a name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startstr_len = 0;
    char_u	*dst_start = dst;

    ଯଦି (startstr != ଖାଲି)
	startstr_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    ଯେତେବେଳେ (*src && dstlen > 0)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`.
	ଯଦି (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି(&src, ଖାଲି);
	    ଯଦି (*src == '`')
		++src;
	    len = src - var;
	    ଯଦି (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
	    ଜାରି;
	}
#endif
	copy_char = TRUE;
	ଯଦି ((*src == '$'
#ifdef VMS
		    && at_start
#endif
	   )
#ଯଦି defined(MSWIN)
		|| *src == '%'
#endif
		|| (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ଯଦି (*src != '~')				// environment var
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		// Unix has ${var-name} type environment vars
		ଯଦି (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	// ignore '{'
		    ଯେତେବେଳେ (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		}
		ଅନ୍ୟଥା
#endif
		{
		    ଯେତେବେଳେ (c-- > 0 && *tail != NUL && ((vim_isIDc(*tail))
#ଯଦି defined(MSWIN)
			    || (*src == '%' && *tail != '%')
#endif
			    ))
			*var++ = *tail++;
		}

#ଯଦି defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ଯଦି (src[1] == '{' && *tail != '}')
# ଅନ୍ୟଥା
		ଯଦି (*src == '%' && *tail != '%')
# endif
		    var = ଖାଲି;
		ଅନ୍ୟଥା
		{
# ifdef UNIX
		    ଯଦି (src[1] == '{')
# ଅନ୍ୟଥା
		    ଯଦି (*src == '%')
# endif
			++tail;
#endif
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ଯଦି defined(MSWIN) || defined(UNIX)
		}
#endif
	    }
							// home directory
	    ଅନ୍ୟଥା ଯଦି (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ଖାଲି)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    ଅନ୍ୟଥା					// user directory
	    {
#ଯଦି defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		ଯେତେବେଳେ (	   c-- > 0
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
#  ଯଦି defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    ସଂରଚନା passwd *pw = (*dst == NUL)
					? ଖାଲି : getpwnam((ଅକ୍ଷର *)dst + 1);

		    var = (pw == ଖାଲି) ? ଖାଲି : (char_u *)pw->pw_dir;
		}
		ଯଦି (var == ଖାଲି)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ଖାଲି,
				WILD_ADD_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# ଅନ୍ୟଥା	// !UNIX, thus VMS
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
		    ଯେତେବେଳେ (*next_path)
		    {
			ପାଇଁ (path = next_path; *next_path && *next_path != ',';
				next_path++);
			ଯଦି (*next_path)
			    *next_path++ = NUL;
			testlen = vim_snprintf_safelen(
			    (ଅକ୍ଷର *)test,
			    ଆକାର(test),
			    "%s/%s",
			    path,
			    dst + 1);
			ଯଦି (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ଯଦି (var != ଖାଲି)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    ଭାଙ୍ଗ;
			}
		    }
		}
# endif // UNIX
#ଅନ୍ୟଥା
		// cannot expand user's home directory, so don't try
		var = ଖାଲି;
		tail = (char_u *)"";	// for gcc
#endif // UNIX || VMS
	    }

#ifdef BACKSLASH_IN_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ଯଦି (p_ssl && var != ଖାଲି && vim_strchr(var, '\\') != ଖାଲି)
	    {
		char_u	*p = vim_strsave(var);

		ଯଦି (p != ଖାଲି)
		{
		    ଯଦି (mustfree)
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
	    ଯଦି (esc_chars != ଖାଲି && var != ଖାଲି
		    && vim_strpbrk(var, esc_chars) != ଖାଲି)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ଯଦି (p != ଖାଲି)
		{
		    ଯଦି (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ଯଦି (var != ଖାଲି && *var != NUL)
	    {
		c = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(var);

		ଯଦି (c + STRLEN(tail) + 1 < (unsigned)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ଯଦି (after_pathsep(dst, dst + c)
#ଯଦି defined(BACKSLASH_IN_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#endif
			    && vim_ispathsep(*tail))
			++tail;
		    dst += c;
		    src = tail;
		    copy_char = FALSE;
		}
	    }
	    ଯଦି (mustfree)
		vim_free(var);
	}

	ଯଦି (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_start = FALSE;
	    ଯଦି (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    ଅନ୍ୟଥା ଯଦି ((src[0] == ' ' || src[0] == ',') && !one)
		at_start = TRUE;
	    ଯଦି (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ଯଦି (startstr != ଖାଲି && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_start = TRUE;
	    }
	}

    }
    *dst = NUL;

    ଫେରନ୍ତୁ (size_t)(dst - dst_start);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    ସ୍ଥିର char_u *
remove_tail(char_u *p, char_u *pend, char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ଯଦି (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	ଫେରନ୍ତୁ newend;
    ଫେରନ୍ତୁ pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    ସ୍ଥିର char_u *
vim_version_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ଯଦି (vimdir == ଖାଲି || *vimdir == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_VERSION_NODOT, STRLEN_LITERAL(VIM_VERSION_NODOT), TRUE, &p);
    ଯଦି (p.string != ଖାଲି && mch_isdir(p.string))
	ଫେରନ୍ତୁ p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ଯଦି (p.string != ଖାଲି && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.length,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ଯଦି (fname.string != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ଯଦି (exists)
		ଫେରନ୍ତୁ p.string;
	}
    }
    vim_free(p.string);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *mustfree)
{
    char_u	*p = ଖାଲି;
    char_u	*pend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vimruntime;
#ifdef MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ଯଦି (STRCMP(name, "HOME") == 0)
	ଫେରନ୍ତୁ homedir;

    // Use Wide function
    wn = enc_to_utf16(name, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    wp = _wgetenv(wn);
    vim_free(wn);

    ଯଦି (wp != ଖାଲି && *wp == NUL)   // empty is the same as not set
	wp = ଖାଲି;

    ଯଦି (wp != ଖାଲି)
    {
	p = utf16_to_enc(wp, ଖାଲି);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	*mustfree = TRUE;
	ଫେରନ୍ତୁ p;
    }
#ଅନ୍ୟଥା
    p = mch_getenv(name);
    ଯଦି (p != ଖାଲି && *p == NUL)	    // empty is the same as not set
	p = ଖାଲି;

    ଯଦି (p != ଖାଲି)
	ଫେରନ୍ତୁ p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ଯଦି (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	ସ୍ଥିର ଅକ୍ଷର userSettingsPath[MAXPATHL];

	ଯଦି (find_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   userSettingsPath, MAXPATHL) == B_OK)
	    ଫେରନ୍ତୁ (char_u *)userSettingsPath;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ ଖାଲି;
    }
# endif
#endif

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ଯଦି (!vimruntime && STRCMP(name, "VIM") != 0)
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ଯଦି (vimruntime
#ifdef HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#endif
       )
    {
#ifdef MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ଯଦି (wp != ଖାଲି && *wp == NUL)	    // empty is the same as not set
	    wp = ଖାଲି;
	ଯଦି (wp != ଖାଲି)
	{
	    char_u *q = utf16_to_enc(wp, ଖାଲି);
	    ଯଦି (q != ଖାଲି)
	    {
		p = vim_version_dir(q);
		*mustfree = TRUE;
		ଯଦି (p == ଖାଲି)
		    p = q;
	    }
	}
#ଅନ୍ୟଥା
	p = mch_getenv((char_u *)"VIM");
	ଯଦି (p != ଖାଲି && *p == NUL)	    // empty is the same as not set
	    p = ଖାଲି;
	ଯଦି (p != ଖାଲି)
	{
	    p = vim_version_dir(p);
	    ଯଦି (p != ଖାଲି)
		*mustfree = TRUE;
	    ଅନ୍ୟଥା
		p = mch_getenv((char_u *)"VIM");
	}
#endif
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ଯଦି (p == ଖାଲି)
    {
	ଯଦି (p_hf != ଖାଲି && vim_strchr(p_hf, '$') == ଖାଲି)
	    p = p_hf;
#ifdef USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	ଅନ୍ୟଥା
	    p = exe_name;
#endif
	ଯଦି (p != ଖାଲି)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ଯଦି (p == p_hf)
		pend = remove_tail(p, pend, (char_u *)"doc");

#ifdef USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ଯଦି (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_tail(p, pend, (char_u *)"MacOS");
		ଯଦି (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ଯଦି (pnew != ଖାଲି)
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
	    ଯଦି (p == exe_name)
		pend = remove_tail(p, pend, (char_u *)"src");
#endif

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ଯଦି (!vimruntime)
	    {
		pend = remove_tail(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_tail(p, pend, (char_u *)VIM_VERSION_NODOT);
	    }

	    // remove trailing path separator
	    ଯଦି (pend > p && after_pathsep(p, pend))
		--pend;

#ifdef MACOS_X
	    ଯଦି (p == exe_name || p == p_hf)
#endif
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ଯଦି (p != ଖାଲି && !mch_isdir(p))
		VIM_CLEAR(p);
	    ଅନ୍ୟଥା
	    {
#ifdef USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ଯଦି (vimruntime && (pend = vim_version_dir(p)) != ଖାଲି)
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
    ଯଦି (p == ଖାଲି)
    {
	// Only use default_vimruntime_dir when it is not empty
	ଯଦି (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (*default_vim_dir != NUL)
	{
	    ଯଦି (vimruntime && (p = vim_version_dir(default_vim_dir)) != ଖାଲି)
		*mustfree = TRUE;
	    ଅନ୍ୟଥା
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
    ଯଦି (p != ଖାଲି)
    {
	ଯଦି (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    ଫେରନ୍ତୁ p;
}

    ଶୂନ୍ୟ
vim_unsetenv(char_u *var)
{
#ifdef HAVE_UNSETENV
    unsetenv((ଅକ୍ଷର *)var);
#ଅନ୍ୟଥା
    vim_setenv(var, (char_u *)"");
#endif
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    ଶୂନ୍ୟ
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ଯଦି (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    ଅନ୍ୟଥା ଯଦି (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    ଶୂନ୍ୟ
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ଯଦି (STRICMP(name, "HOME") == 0)
	init_homedir();
    ଅନ୍ୟଥା ଯଦି (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    ଅନ୍ୟଥା ଯଦି (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#endif

/*
 * Our portable version of setenv.
 */
    ଶୂନ୍ୟ
vim_setenv(char_u *name, char_u *val)
{
#ifdef HAVE_SETENV
    mch_setenv((ଅକ୍ଷର *)name, (ଅକ୍ଷର *)val, 1);
#ଅନ୍ୟଥା
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ଯଦି (envbuf != ଖାଲି)
    {
	sprintf((ଅକ୍ଷର *)envbuf, "%s=%s", name, val);
	putenv((ଅକ୍ଷର *)envbuf);
    }
#endif
#ifdef FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ଯଦି (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ଯଦି (buf != ଖାଲି)
	{
	    bindtextdomain(VIMPACKAGE, (ଅକ୍ଷର *)buf);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx)
{
#ଯଦି defined(AMIGA)
    // No environ[] on the Amiga.
    ଫେରନ୍ତୁ ଖାଲି;
#ଅନ୍ୟଥା
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    ବାହ୍ୟ ଅକ୍ଷର		**environ;
# endif
    char_u		*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;

    str = (char_u *)environ[idx];
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ଯଦି (str[n] == '=' || str[n] == NUL)
	    ଭାଙ୍ଗ;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    ଫେରନ୍ତୁ xp->xp_buf;
#endif
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_user(char_u *user, ପୂର୍ଣ୍ଣସଂଖ୍ୟା need_copy)
{
    char_u	*user_copy = (user != ଖାଲି && need_copy)
						    ? vim_strsave(user) : user;

    ଯଦି (user_copy == ଖାଲି || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ଯଦି (need_copy)
	    vim_free(user_copy);
	ଫେରନ୍ତୁ;
    }
    ((char_u **)(ga_users.ga_data))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_users(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lazy_init_done = FALSE;

    ଯଦି (lazy_init_done)
	ଫେରନ୍ତୁ;

    lazy_init_done = TRUE;
    ga_init2(&ga_users, ଆକାର(char_u *), 20);

#ଯଦି defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	ସଂରଚନା passwd*	pw;

	setpwent();
	ଯେତେବେଳେ ((pw = getpwent()) != ଖାଲି)
	    add_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#ନଚେତଯଦି defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ଯଦି (NetUserEnum(ଖାଲି, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_LENGTH,
				       &nusers, &ntotal, ଖାଲି) == NERR_Success)
	{
	    ପାଇଁ (i = 0; i < nusers; i++)
		add_user(utf16_to_enc(uinfo[i].usri0_name, ଖାଲି), FALSE);

	    NetApiBufferFree(uinfo);
	}
    }
#endif
#ଯଦି defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ଯଦି (user_env != ଖାଲି && *user_env != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	    ପାଇଁ (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_data)[i];

		ଯଦି (STRCMP(local_user, user_env) == 0)
		    ଭାଙ୍ଗ;
	    }

	    ଯଦି (i == ga_users.ga_len)
	    {
		ସଂରଚନା passwd	*pw = getpwnam((ଅକ୍ଷର *)user_env);

		ଯଦି (pw != ଖାଲି)
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
get_users(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    init_users();
    ଯଦି (idx < ga_users.ga_len)
	ଫେରନ୍ତୁ ((char_u **)ga_users.ga_data)[idx];
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
match_user(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = 0;

    init_users();
    ପାଇଁ (i = 0; i < ga_users.ga_len; i++)
    {
	ଯଦି (STRCMP(((char_u **)ga_users.ga_data)[i], name) == 0)
	    ଫେରନ୍ତୁ 2; // full match
	ଯଦି (STRNCMP(((char_u **)ga_users.ga_data)[i], name, n) == 0)
	    result = 1; // partial match
    }
    ଫେରନ୍ତୁ result;
}

    ସ୍ଥିର ଶୂନ୍ୟ
prepare_to_exit(ଶୂନ୍ୟ)
{
#ଯଦି defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#endif

#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	gui.dying = true;
	out_trash();	// trash any pending output
    }
    ଅନ୍ୟଥା
#endif
    {
	windgoto((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ଯଦି (!full_screen)
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
    ଶୂନ୍ୟ
preserve_exit(ଶୂନ୍ୟ)
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
	ଯଦି (buf->b_ml.ml_mfp != ଖାଲି && buf->b_ml.ml_mfp->mf_fname != ଖାଲି)
	{
	    OUT_STR("Vim: preserving files...\r\n");
	    screen_start();	    // don't know where cursor is now
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    ଭାଙ୍ଗ;
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	breakcheck_ଗଣନା = 0;

    ଶୂନ୍ୟ
ଧାଡ଼ି_breakcheck(ଶୂନ୍ୟ)
{
    ଯଦି (++breakcheck_ଗଣନା >= BREAKCHECK_SKIP)
    {
	breakcheck_ଗଣନା = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    ଶୂନ୍ୟ
fast_breakcheck(ଶୂନ୍ୟ)
{
    ଯଦି (++breakcheck_ଗଣନା >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_ଗଣନା = 0;
	ui_breakcheck();
    }
}

#ଯଦି defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    ଶୂନ୍ୟ
veryfast_breakcheck(ଶୂନ୍ୟ)
{
    ଯଦି (++breakcheck_ଗଣନା >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_ଗଣନା = 0;
	ui_breakcheck();
    }
}
#endif

#ଯଦି defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// can be SHELL_SILENT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*buffer = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
    FILE	*fd;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ ଖାଲି;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ଯଦି ((tempname = vim_tempname('o', TRUE)) == ଖାଲି)
    {
	emsg(_(e_cant_get_temp_file_name));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ଯଦି (command == ଖାଲି)
	ଯାଅ done;

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
    fd = mch_fopen((ଅକ୍ଷର *)tempname, "r");
# ଅନ୍ୟଥା
    fd = mch_fopen((ଅକ୍ଷର *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ଯଦି (fd == ଖାଲି
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ଯଦି (fd != ଖାଲି)
	    fclose(fd);
	ଯାଅ done;
    }

    buffer = alloc(len + 1);
    ଯଦି (buffer != ଖାଲି)
	i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fread((ଅକ୍ଷର *)buffer, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ଯଦି (buffer == ଖାଲି)
	ଯାଅ done;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ଯଦି (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(buffer);
    }
    ଅନ୍ୟଥା ଯଦି (ret_len == ଖାଲି)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	ପାଇଁ (i = 0; i < len; ++i)
	    ଯଦି (buffer[i] == NUL)
		buffer[i] = 1;

	buffer[len] = NUL;	// make sure the buffer is terminated
    }
    ଅନ୍ୟଥା
	*ret_len = len;

done:
    mch_remove(tempname);
    vim_free(tempname);
    ଫେରନ୍ତୁ buffer;
}

# ଯଦି defined(FEAT_EVAL)

    ସ୍ଥିର ଶୂନ୍ୟ
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retlist)
{
    char_u	*res = ଖାଲି;
    char_u	*p;
    char_u	*infile = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = FALSE;
    FILE	*fd;
    list_T	*list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = SHELL_SILENT;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_argv = FALSE;
    ଅକ୍ଷର	**argv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
    ଯଦି (check_restricted() || check_secure())
	ଯାଅ errret;

    ଯଦି (in_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_ସଂଖ୍ୟା_or_list_arg(argvars, 1)
								      == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ଯଦି ((infile = vim_tempname('i', TRUE)) == ଖାଲି)
	{
	    emsg(_(e_cant_get_temp_file_name));
	    ଯାଅ errret;
	}

	fd = mch_fopen((ଅକ୍ଷର *)infile, WRITEBIN);
	ଯଦି (fd == ଖାଲି)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    ଯାଅ errret;
	}
	ଯଦି (argvars[1].v_type == VAR_ସଂଖ୍ୟା)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_ସଂଖ୍ୟା);
	    ଯଦି (buf == ଖାଲି)
	    {
		semsg(_(e_buffer_nr_does_not_exist), argvars[1].vval.v_ସଂଖ୍ୟା);
		fclose(fd);
		ଯାଅ errret;
	    }

	    ପାଇଁ (lnum = 1; lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; lnum++)
	    {
		ପାଇଁ (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ଯଦି (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			ଭାଙ୍ଗ;
		    }
		ଯଦି (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_LIST)
	{
	    ଯଦି (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ଯଦି (p == ଖାଲି)
	    {
		fclose(fd);
		ଯାଅ errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ଯଦି (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ଯଦି (fclose(fd) != 0)
	    err = TRUE;
	ଯଦି (err)
	{
	    emsg(_(e_error_writing_temp_file));
	    ଯାଅ errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ଯଦି (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ଯଦି (l == ଖାଲି || l->lv_len < 1)
	{
	    emsg(_(e_invalid_argument));
	    ଯାଅ errret;
	}
	ଯଦି (build_argv_from_list(l, &argv, &argc) == FAIL)
	    ଯାଅ errret;
	ଯଦି (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ଯାଅ errret;
	}
	use_argv = TRUE;

	ଯଦି (p_verbose > 3)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    ପାଇଁ (i = 0; i < argc; ++i)
	    {
		ଯଦି (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (ଅକ୍ଷର *)ga.ga_data);
	    ସନ୍ଦେଶ_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ଯଦି (!ସନ୍ଦେଶ_silent)
	flags += SHELL_COOKED;

    ଯଦି (retlist)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	listitem_T	*li;
	char_u		*s = ଖାଲି;
	char_u		*start;
	char_u		*end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	ଯଦି (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, &len);
	ଅନ୍ୟଥା
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ଯଦି (res == ଖାଲି)
	    ଯାଅ errret;

	list = list_alloc();
	ଯଦି (list == ଖାଲି)
	    ଯାଅ errret;

	ପାଇଁ (i = 0; i < len; ++i)
	{
	    start = res + i;
	    ଯେତେବେଳେ (i < len && res[i] != NL)
		++i;
	    end = res + i;

	    s = alloc(end - start + 1);
	    ଯଦି (s == ଖାଲି)
		ଯାଅ errret;

	    ପାଇଁ (p = s; start < end; ++p, ++start)
		*p = *start == NUL ? NL : *start;
	    *p = NUL;

	    li = listitem_alloc();
	    ଯଦି (li == ଖାଲି)
	    {
		vim_free(s);
		ଯାଅ errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_lock = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_set(rettv, list);
	list = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (use_argv)
	    res = mch_get_cmd_output_direct(argv, infile, flags, ଖାଲି);
	ଅନ୍ୟଥା
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ଖାଲି);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ଯଦି (res != ଖାଲି)
	{
	    char_u	*s, *d;

	    d = res;
	    ପାଇଁ (s = res; *s; ++s)
	    {
		ଯଦି (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ଖାଲି;
    }

errret:
    ଯଦି (argv != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ (i = 0; argv[i] != ଖାଲି; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ଯଦି (infile != ଖାଲି)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ଯଦି (res != ଖାଲି)
	vim_free(res);
    ଯଦି (list != ଖାଲି)
	list_free(list);
}

/*
 * "system()" function
 */
    ଶୂନ୍ୟ
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
goto_im(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (p_im && stuff_empty() && typebuf_typed());
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
get_isolated_shell_name(ଶୂନ୍ୟ)
{
    char_u *p;

#ifdef MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#ଅନ୍ୟଥା
    p = skiptowhite(p_sh);
    ଯଦି (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    ଅନ୍ୟଥା
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	ପାଇଁ (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ଯଦି (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#endif
    ଫେରନ୍ତୁ p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
path_is_url(char_u *p)
{
    ଯଦି (STRNCMP(p, "://", (size_t)3) == 0)
	ଫେରନ୍ତୁ URL_SLASH;
    ଅନ୍ୟଥା ଯଦି (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	ଫେରନ୍ତୁ URL_BACKSLASH;
    ଫେରନ୍ତୁ 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
path_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ଯଦି (!ASCII_ISALPHA(*fname))
	ଫେରନ୍ତୁ 0;

    // check body: alpha or dash
    ପାଇଁ (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ଯଦି (p[-1] == '-')
	ଫେରନ୍ତୁ 0;

    // "://" or ":\\" must follow
    ଫେରନ୍ତୁ path_is_url(p);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ଯଦି (v_event->dv_hashtab.ht_used > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    ଅନ୍ୟଥା
	sve->sve_did_save = FALSE;
    ଫେରନ୍ତୁ v_event;
}

    ଶୂନ୍ୟ
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ଯଦି (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    ଅନ୍ୟଥା
	hash_init(&v_event->dv_hashtab);
}
#endif

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    ଶୂନ୍ୟ
may_trigger_modechanged(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_mode[MODE_MAX_LENGTH];
    char_u	    pattern_buf[2 * MODE_MAX_LENGTH];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ଯଦି (!has_modechanged() || got_int)
	ଫେରନ୍ତୁ;

    get_mode(curr_mode);
    ଯଦି (STRCMP(curr_mode, last_mode) == 0)
	ଫେରନ୍ତୁ;

    v_event = get_v_event(&save_v_event);
    (ଶୂନ୍ୟ)dict_add_string(v_event, "new_mode", curr_mode);
    (ଶୂନ୍ୟ)dict_add_string(v_event, "old_mode", last_mode);
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((ଅକ୍ଷର *)pattern_buf, ଆକାର(pattern_buf), "%s:%s", last_mode,
	    curr_mode);

    apply_autocmds(EVENT_MODECHANGED, pattern_buf, ଖାଲି, FALSE, curbuf);
    STRCPY(last_mode, curr_mode);

    restore_v_event(v_event, &save_v_event);
#endif
}

// For overflow detection, add a digit safely to a long value.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_append_digit_long(long *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା digit)
{
    long x = *ମୂଲ୍ୟ;
    ଯଦି (x > ((LONG_MAX - (long)digit) / 10))
	ଫେରନ୍ତୁ FAIL;
    *ମୂଲ୍ୟ = x * 10 + (long)digit;
    ଫେରନ୍ତୁ OK;
}

// Return something that fits into an int.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
trim_to_int(vimlong_T x)
{
    ଫେରନ୍ତୁ x > INT_MAX ? INT_MAX : x < INT_MIN ? INT_MIN : x;
}
