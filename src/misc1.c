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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#ఒకవేళ defined(__HAIKU__)
# include <storage/FindDirectory.h>
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
# include <lm.h>
#అయితే_ముగింపు

#నిర్వచించు URL_SLASH	1		// path_is_url() has found "://"
#నిర్వచించు URL_BACKSLASH	2		// path_is_url() has found ":\\"

// All user names (for ~user completion as done by shell).
స్థిర garray_T	ga_users;

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
    పూర్ణాంకం
get_leader_len(
    char_u	*పంక్తి,
    char_u	**flags,
    పూర్ణాంకం		backward,
    పూర్ణాంకం		include_space)
{
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		ఫలితం;
    పూర్ణాంకం		got_com = FALSE;
    పూర్ణాంకం		దొరికింది_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part
    char_u	*string;		// pointer to comment string
    char_u	*list;
    పూర్ణాంకం		మధ్య_సరిపోలిక_len = 0;
    char_u	*prev_list;
    char_u	*saved_flags = ఖాళీ;

    ఫలితం = i = 0;
    అయితే (VIM_ISWHITE(పంక్తి[i]))    // leading white space is ignored
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    అయితే (పంక్తి[i] != NUL)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	దొరికింది_one = FALSE;
	కొరకు (list = curbuf->b_p_com; *list; )
	{
	    // Get one option part into part_buf[].  Advance "list" to next
	    // one.  Put "string" at start of string.
	    ఒకవేళ (!got_com && flags != ఖాళీ)
		*flags = list;	    // remember where flags started
	    prev_list = list;
	    (శూన్య)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ఒకవేళ (string == ఖాళీ)	    // missing ':', ignore this part
		కొనసాగు;
	    *string++ = NUL;	    // isolate flags from string

	    // If we found a middle match previously, use that match when this
	    // is not a middle or end.
	    ఒకవేళ (మధ్య_సరిపోలిక_len != 0
		    && vim_strchr(part_buf, COM_మధ్య) == ఖాళీ
		    && vim_strchr(part_buf, COM_చివర) == ఖాళీ)
		ఆపు;

	    // When we already found a nested comment, only accept further
	    // nested comments.
	    ఒకవేళ (got_com && vim_strchr(part_buf, COM_NEST) == ఖాళీ)
		కొనసాగు;

	    // When 'O' flag present and using "O" command skip this one.
	    ఒకవేళ (backward && vim_strchr(part_buf, COM_NOBACK) != ఖాళీ)
		కొనసాగు;

	    // Line contents and string must match.
	    // When string starts with white space, must have some white space
	    // (but the amount does not need to match, there might be a mix of
	    // TABs and spaces).
	    ఒకవేళ (VIM_ISWHITE(string[0]))
	    {
		ఒకవేళ (i == 0 || !VIM_ISWHITE(పంక్తి[i - 1]))
		    కొనసాగు;  // missing white space
		అయితే (VIM_ISWHITE(string[0]))
		    ++string;
	    }
	    కొరకు (j = 0; string[j] != NUL && string[j] == పంక్తి[i + j]; ++j)
		;
	    ఒకవేళ (string[j] != NUL)
		కొనసాగు;  // string doesn't match

	    // When 'b' flag used, there must be white space or an
	    // end-of-line after the string in the line.
	    ఒకవేళ (vim_strchr(part_buf, COM_BLANK) != ఖాళీ
			   && !VIM_ISWHITE(పంక్తి[i + j]) && పంక్తి[i + j] != NUL)
		కొనసాగు;

	    // We have found a match, stop searching unless this is a middle
	    // comment. The middle comment can be a substring of the end
	    // comment in which case it's better to return the length of the
	    // end comment and its flags.  Thus we keep searching with middle
	    // and end matches and use an end match if it matches better.
	    ఒకవేళ (vim_strchr(part_buf, COM_మధ్య) != ఖాళీ)
	    {
		ఒకవేళ (మధ్య_సరిపోలిక_len == 0)
		{
		    మధ్య_సరిపోలిక_len = j;
		    saved_flags = prev_list;
		}
		కొనసాగు;
	    }
	    ఒకవేళ (మధ్య_సరిపోలిక_len != 0 && j > మధ్య_సరిపోలిక_len)
		// Use this match instead of the middle match, since it's a
		// longer thus better match.
		మధ్య_సరిపోలిక_len = 0;

	    ఒకవేళ (మధ్య_సరిపోలిక_len == 0)
		i += j;
	    దొరికింది_one = TRUE;
	    ఆపు;
	}

	ఒకవేళ (మధ్య_సరిపోలిక_len != 0)
	{
	    // Use the previously found middle match after failing to find a
	    // match with an end.
	    ఒకవేళ (!got_com && flags != ఖాళీ)
		*flags = saved_flags;
	    i += మధ్య_సరిపోలిక_len;
	    దొరికింది_one = TRUE;
	}

	// No match found, stop scanning.
	ఒకవేళ (!దొరికింది_one)
	    ఆపు;

	ఫలితం = i;

	// Include any trailing white space.
	అయితే (VIM_ISWHITE(పంక్తి[i]))
	    ++i;

	ఒకవేళ (include_space)
	    ఫలితం = i;

	// If this comment doesn't nest, stop here.
	got_com = TRUE;
	ఒకవేళ (vim_strchr(part_buf, COM_NEST) == ఖాళీ)
	    ఆపు;
    }
    తిరిగి ఫలితం;
}

/*
 * Return the offset at which the last comment in line starts. If there is no
 * comment in the whole line, -1 is returned.
 *
 * When "flags" is not null, it is set to point to the flags describing the
 * recognized comment leader.
 */
    పూర్ణాంకం
get_చివరి_leader_స్థానభ్రంశం(char_u *పంక్తి, char_u **flags)
{
    పూర్ణాంకం		ఫలితం = -1;
    పూర్ణాంకం		i, j;
    పూర్ణాంకం		క్రింది_check_bound = 0;
    char_u	*string;
    char_u	*com_leader;
    char_u	*com_flags;
    char_u	*list;
    పూర్ణాంకం		దొరికింది_one;
    char_u	part_buf[COM_MAX_LEN];	// buffer for one option part

    /*
     * Repeat to match several nested comment strings.
     */
    i = (పూర్ణాంకం)STRLEN(పంక్తి);
    అయితే (--i >= క్రింది_check_bound)
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	దొరికింది_one = FALSE;
	కొరకు (list = curbuf->b_p_com; *list; )
	{
	    char_u *flags_save = list;

	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    (శూన్య)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    ఒకవేళ (string == ఖాళీ)	// If everything is fine, this cannot actually
				// happen.
		కొనసాగు;
	    *string++ = NUL;	// Isolate flags from string.
	    com_leader = string;

	    /*
	     * Line contents and string must match.
	     * When string starts with white space, must have some white space
	     * (but the amount does not need to match, there might be a mix of
	     * TABs and spaces).
	     */
	    ఒకవేళ (VIM_ISWHITE(string[0]))
	    {
		ఒకవేళ (i == 0 || !VIM_ISWHITE(పంక్తి[i - 1]))
		    కొనసాగు;
		అయితే (VIM_ISWHITE(*string))
		    ++string;
	    }
	    కొరకు (j = 0; string[j] != NUL && string[j] == పంక్తి[i + j]; ++j)
		/* do nothing */;
	    ఒకవేళ (string[j] != NUL)
		కొనసాగు;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    ఒకవేళ (vim_strchr(part_buf, COM_BLANK) != ఖాళీ
		    && !VIM_ISWHITE(పంక్తి[i + j]) && పంక్తి[i + j] != NUL)
		కొనసాగు;

	    ఒకవేళ (vim_strchr(part_buf, COM_మధ్య) != ఖాళీ)
	    {
		// For a middlepart comment, only consider it to match if
		// everything before the current position in the line is
		// whitespace.  Otherwise we would think we are inside a
		// comment if the middle part appears somewhere in the middle
		// of the line.  E.g. for C the "*" appears often.
		కొరకు (j = 0; VIM_ISWHITE(పంక్తి[j]) && j <= i; j++)
		    ;
		ఒకవేళ (j < i)
		    కొనసాగు;
	    }

	    /*
	     * We have found a match, stop searching.
	     */
	    దొరికింది_one = TRUE;

	    ఒకవేళ (flags)
		*flags = flags_save;
	    com_flags = flags_save;

	    ఆపు;
	}

	ఒకవేళ (దొరికింది_one)
	{
	    char_u  part_buf2[COM_MAX_LEN];	// buffer for one option part
	    పూర్ణాంకం     len1, len2, off;

	    ఫలితం = i;
	    /*
	     * If this comment nests, continue searching.
	     */
	    ఒకవేళ (vim_strchr(part_buf, COM_NEST) != ఖాళీ)
		కొనసాగు;

	    క్రింది_check_bound = i;

	    // Let's verify whether the comment leader found is a substring
	    // of other comment leaders. If it is, let's adjust the
	    // lower_check_bound so that we make sure that we have determined
	    // the comment leader correctly.

	    అయితే (VIM_ISWHITE(*com_leader))
		++com_leader;
	    len1 = (పూర్ణాంకం)STRLEN(com_leader);

	    కొరకు (list = curbuf->b_p_com; *list; )
	    {
		char_u *flags_save = list;

		(శూన్య)copy_option_part(&list, part_buf2, COM_MAX_LEN, ",");
		ఒకవేళ (flags_save == com_flags)
		    కొనసాగు;
		string = vim_strchr(part_buf2, ':');
		++string;
		అయితే (VIM_ISWHITE(*string))
		    ++string;
		len2 = (పూర్ణాంకం)STRLEN(string);
		ఒకవేళ (len2 == 0)
		    కొనసాగు;

		// Now we have to verify whether string ends with a substring
		// beginning the com_leader.
		కొరకు (off = (len2 > i ? i : len2); off > 0 && off + len1 > len2;)
		{
		    --off;
		    ఒకవేళ (STRNCMP(string + off, com_leader, len2 - off) == 0)
		    {
			ఒకవేళ (i - off < క్రింది_check_bound)
			    క్రింది_check_bound = i - off;
		    }
		}
	    }
	}
    }
    తిరిగి ఫలితం;
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Includes any filler lines.
 */
    పూర్ణాంకం
plines(linenr_T lnum)
{
    తిరిగి plines_win(curwin, lnum, TRUE);
}

    పూర్ణాంకం
plines_win(
    win_T	*wp,
    linenr_T	lnum,
    పూర్ణాంకం		హద్దు_winheight)	// when TRUE limit to window height
{
#ఒకవేళ defined(FEAT_DIFF)
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    తిరిగి plines_win_nofill(wp, lnum, హద్దు_winheight)
						   + diff_check_fill(wp, lnum);
}

/*
 * Return the number of window lines occupied by buffer line "lnum".
 * Does not include filler lines.
 */
    పూర్ణాంకం
plines_nofill(linenr_T lnum)
{
    తిరిగి plines_win_nofill(curwin, lnum, TRUE);
}

    పూర్ణాంకం
plines_win_nofill(
    win_T	*wp,
    linenr_T	lnum,
    పూర్ణాంకం		హద్దు_winheight)	// when TRUE limit to window height
{
#అయితే_ముగింపు
    పూర్ణాంకం		పంక్తులు;

    ఒకవేళ (wp->w_వెడల్పు == 0)
	తిరిగి 1;

#నిర్వచితమైతే FEAT_FOLDING
    // Folded lines are handled just like an empty line.
    // NOTE: Caller must handle lines that are MAYBE folded.
    ఒకవేళ (పంక్తి_Folded(wp, lnum) == TRUE)
	తిరిగి 1;
#అయితే_ముగింపు

    ఒకవేళ (!wp->w_p_wrap)
	పంక్తులు = 1
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // add a line for each "above" and "below" aligned text property
	    + prop_లెక్క_above_క్రింద(wp->w_బఫరు, lnum)
#అయితే_ముగింపు
	;
    లేకపోతే
	పంక్తులు = plines_win_nofold(wp, lnum);

    ఒకవేళ (హద్దు_winheight && పంక్తులు > wp->w_ఎత్తు)
	తిరిగి wp->w_ఎత్తు;
    తిరిగి పంక్తులు;
}

/*
 * Return number of window lines physical line "lnum" will occupy in window
 * "wp".  Does not care about folding, 'wrap' or 'diff'.
 */
    పూర్ణాంకం
plines_win_nofold(win_T *wp, linenr_T lnum)
{
    char_u	*s;
    దీర్ఘ	నిలువు;
    పూర్ణాంకం		వెడల్పు;
    chartabsize_T cts;

    s = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, s, s);
    ఒకవేళ (*s == NUL
#నిర్వచితమైతే FEAT_PROP_POPUP
	    && !cts.cts_has_prop_with_పాఠ్యం
#అయితే_ముగింపు
	    )
	తిరిగి 1; // be quick for an empty line
    win_linetabsize_cts(&cts, (colnr_T)MAXCOL);
    clear_chartabsize_arg(&cts);
    నిలువు = (పూర్ణాంకం)cts.cts_vcol;

    // If list mode is on, then the '$' at the end of the line may take up one
    // extra column.
    ఒకవేళ (wp->w_p_list && wp->w_lcs_chars.eol != NUL)
	నిలువు += 1;

    /*
     * Add column offset for 'number', 'relativenumber' and 'foldcolumn'.
     */
    వెడల్పు = wp->w_వెడల్పు - win_నిలువు_off(wp);
    ఒకవేళ (వెడల్పు <= 0)
	తిరిగి 32000;
    ఒకవేళ (నిలువు <= వెడల్పు)
	తిరిగి 1;
    నిలువు -= వెడల్పు;
    వెడల్పు += win_నిలువు_off2(wp);
    తిరిగి (నిలువు + (వెడల్పు - 1)) / వెడల్పు + 1;
}

/*
 * Like plines_win(), but only reports the number of physical screen lines
 * used from the start of the line to the given byte column.
 */
    పూర్ణాంకం
plines_win_నిలువు(win_T *wp, linenr_T lnum, దీర్ఘ స్తంభం)
{
    దీర్ఘ	నిలువు;
    పూర్ణాంకం		పంక్తులు = 0;
    పూర్ణాంకం		వెడల్పు;
    char_u	*పంక్తి;
    chartabsize_T cts;

#నిర్వచితమైతే FEAT_DIFF
    // Check for filler lines above this buffer line.  When folded the result
    // is one line anyway.
    పంక్తులు = diff_check_fill(wp, lnum);
#అయితే_ముగింపు

    ఒకవేళ (!wp->w_p_wrap)
	తిరిగి పంక్తులు + 1;

    ఒకవేళ (wp->w_వెడల్పు == 0)
	తిరిగి పంక్తులు + 1;

    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);

    init_chartabsize_arg(&cts, wp, lnum, 0, పంక్తి, పంక్తి);
    // "column" is a byte index, advance the pointer until it is reached.
    అయితే (*cts.cts_ptr != NUL && cts.cts_ptr < పంక్తి + స్తంభం)
    {
	cts.cts_vcol += win_lbr_chartabsize(&cts, ఖాళీ, ఖాళీ);
	MB_PTR_ADV(cts.cts_ptr);
    }

    /*
     * If *cts.cts_ptr is a TAB, and the TAB is not displayed as ^I, and we're
     * not in MODE_INSERT state, then col must be adjusted so that it
     * represents the last screen position of the TAB.  This only fixes an
     * error when the TAB wraps from one screen line to the next (when
     * 'columns' is not a multiple of 'ts') -- webb.
     */
    నిలువు = cts.cts_vcol;
    ఒకవేళ (*cts.cts_ptr == TAB && (State & బహుళకం_సాధారణ)
				    && (!wp->w_p_list || wp->w_lcs_chars.tab1))
	నిలువు += win_lbr_chartabsize(&cts, ఖాళీ, ఖాళీ) - 1;
    clear_chartabsize_arg(&cts);

    /*
     * Add column offset for 'number', 'relativenumber', 'foldcolumn', etc.
     */
    వెడల్పు = wp->w_వెడల్పు - win_నిలువు_off(wp);
    ఒకవేళ (వెడల్పు <= 0)
	తిరిగి 9999;

    పంక్తులు += 1;
    ఒకవేళ (నిలువు > వెడల్పు)
	పంక్తులు += (నిలువు - వెడల్పు) / (వెడల్పు + win_నిలువు_off2(wp)) + 1;
    తిరిగి పంక్తులు;
}

/*
 * Return number of window lines the physical line range from "first" until
 * "last" will occupy in window "wp". Takes into account folding, 'wrap',
 * topfill and filler lines beyond the end of the buffer. Limit to "max" lines.
 */
    పూర్ణాంకం
plines_m_win(win_T *wp, linenr_T మొదటి, linenr_T చివరి, పూర్ణాంకం max)
{
    పూర్ణాంకం		లెక్క = 0;

    అయితే (మొదటి <= చివరి && లెక్క < max)
    {
#నిర్వచితమైతే FEAT_FOLDING
	పూర్ణాంకం	x;

	// Check if there are any really folded lines, but also included lines
	// that are maybe folded.
	x = folded_లెక్క(wp, మొదటి, ఖాళీ);
	ఒకవేళ (x > 0)
	{
	    ++లెక్క;	    // count 1 for "+-- folded" line
	    మొదటి += x;
	}
	లేకపోతే
#అయితే_ముగింపు
	{
#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (మొదటి == wp->w_topline)
		లెక్క += plines_win_nofill(wp, మొదటి, FALSE) + wp->w_topfill;
	    లేకపోతే
#అయితే_ముగింపు
		లెక్క += plines_win(wp, మొదటి, FALSE);
	    ++మొదటి;
	}
    }
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (మొదటి == wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క + 1)
	లెక్క += diff_check_fill(wp, మొదటి);
#అయితే_ముగింపు
    తిరిగి MIN(max, లెక్క);
}

    పూర్ణాంకం
gchar_pos(pos_T *pos)
{
    char_u	*ptr;
    పూర్ణాంకం		ptrlen;

    // When searching columns is sometimes put at the end of a line.
    ఒకవేళ (pos->నిలువు == MAXCOL)
	తిరిగి NUL;
    ptrlen = ml_get_len(pos->lnum);
    ptr = ml_get_pos(pos);
    ఒకవేళ (pos->నిలువు > ptrlen)
	తిరిగి NUL;
    ఒకవేళ (has_mbyte)
	తిరిగి (*mb_ptr2char)(ptr);
    తిరిగి (పూర్ణాంకం)*ptr;
}

    పూర్ణాంకం
gchar_cursor(శూన్య)
{
    ఒకవేళ (has_mbyte)
	తిరిగి (*mb_ptr2char)(ml_get_cursor());
    తిరిగి (పూర్ణాంకం)*ml_get_cursor();
}

/*
 * Return the character immediately before the cursor.
 */
    పూర్ణాంకం
char_before_cursor(శూన్య)
{
    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
	తిరిగి -1;

    char_u *పంక్తి = ml_get_curline();

    ఒకవేళ (has_mbyte)
    {
	char_u *p = పంక్తి + curwin->w_cursor.నిలువు;
	పూర్ణాంకం prev_len = (*mb_తల_off)(పంక్తి, p - 1) + 1;
	తిరిగి mb_ptr2char(p - prev_len);
    }
    తిరిగి పంక్తి[curwin->w_cursor.నిలువు - 1];
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    శూన్య
pchar_cursor(పూర్ణాంకం c)
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE)
						  + curwin->w_cursor.నిలువు) = c;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_గమ్యం_option_part(char_u *p)
{
    ఒకవేళ (*p == ',')
	++p;
    అయితే (*p == ' ')
	++p;
    తిరిగి p;
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    శూన్య
check_స్థితి(buf_T *buf)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు == buf && wp->w_స్థితి_ఎత్తు)
	{
	    wp->w_redr_స్థితి = true;
	    సమితి_must_redraw(UPD_VALID);
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
    పూర్ణాంకం
ask_yesno(char_u *str, పూర్ణాంకం ప్రత్యక్ష)
{
    పూర్ణాంకం	    r = ' ';
    పూర్ణాంకం	    save_State = State;

    ఒకవేళ (exiting)		// put terminal in raw mode for this question
	settmode(TMODE_RAW);
    ++no_wait_return;
#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు
    State = బహుళకం_CONFIRM;	// mouse behaves like with :confirm
    setmouse();			// disables mouse for xterm
    ++no_mapping;
    ++allow_కీలకాలు;		// no mapping here, but recognize keys

    అయితే (r != 'y' && r != 'n')
    {
	// same highlighting as for wait_return()
	smsg_attr(HL_ATTR(HLF_R), "%s (y/n)?", str);
	ఒకవేళ (ప్రత్యక్ష)
	    r = get_keystroke();
	లేకపోతే
	    r = plain_vgetc();
	ఒకవేళ (r == Ctrl_C || r == ESC)
	    r = 'n';
	సందేశం_putchar(r);	    // show what you typed
	బహిర్గతం_flush();
    }
    --no_wait_return;
    State = save_State;
    setmouse();
    --no_mapping;
    --allow_కీలకాలు;

    తిరిగి r;
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Returns the current mode as a string in "buf[MODE_MAX_LENGTH]", NUL
 * terminated.
 * The first character represents the major mode, the following ones the minor
 * ones.
 */
    శూన్య
get_బహుళకం(char_u *buf)
{
    పూర్ణాంకం		i = 0;

    ఒకవేళ (సమయం_for_testing == 93784)
    {
	// Testing the two-character code.
	buf[i++] = 'x';
	buf[i++] = '!';
    }
# ifdef FEAT_TERMINAL
    లేకపోతే ఒకవేళ (పదం_use_loop())
    {
	ఒకవేళ (State & బహుళకం_CMDLINE)
	    buf[i++] = 'c';
	buf[i++] = 't';
    }
# endif
    లేకపోతే ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE
						      || State == బహుళకం_SETWSIZE
		|| State == బహుళకం_CONFIRM)
    {
	buf[i++] = 'r';
	ఒకవేళ (State == బహుళకం_ASKMORE)
	    buf[i++] = 'm';
	లేకపోతే ఒకవేళ (State == బహుళకం_CONFIRM)
	    buf[i++] = '?';
    }
    లేకపోతే ఒకవేళ (State == బహుళకం_EXTERNCMD)
	buf[i++] = '!';
    లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
    {
	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    buf[i++] = 'R';
	    buf[i++] = 'v';
	}
	లేకపోతే
	{
	    ఒకవేళ (State & REPLACE_FLAG)
		buf[i++] = 'R';
	    లేకపోతే
		buf[i++] = 'i';
	}

	ఒకవేళ (ins_compl_చురుకైన())
	    buf[i++] = 'c';
	లేకపోతే ఒకవేళ (ctrl_x_బహుళకం_not_defined_yet())
	    buf[i++] = 'x';
    }
    లేకపోతే ఒకవేళ ((State & బహుళకం_CMDLINE) || exmode_చురుకైన)
    {
	buf[i++] = 'c';
	ఒకవేళ (exmode_చురుకైన == EXMODE_VIM)
	    buf[i++] = 'v';
	లేకపోతే ఒకవేళ (exmode_చురుకైన == EXMODE_సాధారణ)
	    buf[i++] = 'e';
	ఒకవేళ ((State & బహుళకం_CMDLINE) && cmdline_overstrike())
	    buf[i++] = 'r';
    }
    లేకపోతే ఒకవేళ (VIsual_చురుకైన)
    {
	ఒకవేళ (VIsual_select)
	    buf[i++] = VIsual_బహుళకం + 's' - 'v';
	లేకపోతే
	{
	    buf[i++] = VIsual_బహుళకం;
	    ఒకవేళ (restart_VIsual_select)
		buf[i++] = 's';
	}
    }
    లేకపోతే
    {
	buf[i++] = 'n';
	ఒకవేళ (ముగింపు_op)
	{
	    buf[i++] = 'o';
	    // to be able to detect force-linewise/blockwise/characterwise
	    // operations
	    buf[i++] = motion_force;
	}
	లేకపోతే ఒకవేళ (restart_edit == 'I' || restart_edit == 'R'
							|| restart_edit == 'V')
	{
	    buf[i++] = 'i';
	    buf[i++] = restart_edit;
	}
# ifdef FEAT_TERMINAL
	లేకపోతే ఒకవేళ (పదం_లోపల_సాధారణ_బహుళకం(curbuf))
	    buf[i++] = 't';
# endif
    }

    buf[i] = NUL;
}

/*
 * "mode()" function
 */
    శూన్య
f_బహుళకం(typval_T *argvars, typval_T *rettv)
{
    char_u	buf[బహుళకం_MAX_నిడివి];

    ఒకవేళ (లోపల_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	తిరిగి;

    get_బహుళకం(buf);

    // Clear out the minor mode when the argument is not a non-zero number or
    // non-empty string.
    ఒకవేళ (!non_zero_arg(&argvars[0]))
	buf[1] = NUL;

    rettv->vval.v_string = vim_strsave(buf);
    rettv->v_type = VAR_STRING;
}

    స్థిర శూన్య
may_కూడు_state_char(garray_T *gap, char_u *include, పూర్ణాంకం c)
{
    ఒకవేళ (include == ఖాళీ || vim_strchr(include, c) != ఖాళీ)
	ga_append(gap, c);
}

/*
 * "state()" function
 */
    శూన్య
f_state(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*include = ఖాళీ;
    పూర్ణాంకం		i;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ga_init2(&ga, 1, 20);
    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	include = tv_get_string(&argvars[0]);

    ఒకవేళ (!(stuff_ఖాళీలు() && typebuf.tb_len == 0 && scriptin[curscript] == ఖాళీ))
	may_కూడు_state_char(&ga, include, 'm');
    ఒకవేళ (op_pending())
	may_కూడు_state_char(&ga, include, 'o');
    ఒకవేళ (autocmd_busy)
	may_కూడు_state_char(&ga, include, 'x');
    ఒకవేళ (ins_compl_చురుకైన())
	may_కూడు_state_char(&ga, include, 'a');

# ifdef FEAT_పని_CHANNEL
    ఒకవేళ (channel_లోపల_blocking_wait())
	may_కూడు_state_char(&ga, include, 'w');
# endif
    ఒకవేళ (!get_was_సురక్షిత_state())
	may_కూడు_state_char(&ga, include, 'S');
    కొరకు (i = 0; i < get_callback_లోతు() && i < 3; ++i)
	may_కూడు_state_char(&ga, include, 'c');
    ఒకవేళ (సందేశం_scrolled > 0)
	may_కూడు_state_char(&ga, include, 's');

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_దత్తాంశం;
}

#అయితే_ముగింపు // FEAT_EVAL

/*
 * Get a key stroke directly from the user.
 * Ignores mouse clicks and scrollbar events, except a click for the left
 * button (used at the more prompt).
 * Doesn't use vgetc(), because it syncs undo and eats mapped characters.
 * Disadvantage: typeahead is ignored.
 * Translates the interrupt character for unix to ESC.
 */
    పూర్ణాంకం
get_keystroke(శూన్య)
{
    char_u	*buf = ఖాళీ;
    పూర్ణాంకం		buflen = 150;
    పూర్ణాంకం		maxlen;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		n;
    పూర్ణాంకం		save_mapped_ctrl_c = mapped_ctrl_c;
    పూర్ణాంకం		waited = 0;

    mod_ముసుగు = 0;
    mapped_ctrl_c = FALSE;	// mappings are not used here
    కొరకు (;;)
    {
	cursor_on();
	బహిర్గతం_flush();

	// Leave some room for check_termcode() to insert a key code into (max
	// 5 chars plus NUL).  And fix_input_buffer() can triple the number of
	// bytes.
	maxlen = (buflen - 6 - len) / 3;
	ఒకవేళ (buf == ఖాళీ)
	    buf = alloc(buflen);
	లేకపోతే ఒకవేళ (maxlen < 10)
	{
	    char_u  *t_buf = buf;

	    // Need some more space. This might happen when receiving a long
	    // escape sequence.
	    buflen += 100;
	    buf = vim_realloc(buf, buflen);
	    ఒకవేళ (buf == ఖాళీ)
		vim_free(t_buf);
	    maxlen = (buflen - 6 - len) / 3;
	}
	ఒకవేళ (buf == ఖాళీ)
	{
	    do_outofmem_సందేశం((long_u)buflen);
	    తిరిగి ESC;  // panic!
	}

	// First time: blocking wait.  Second time: wait up to 100ms for a
	// terminal code to complete.
	n = ui_inchar(buf + len, maxlen, len == 0 ? -1L : 100L, 0);
	ఒకవేళ (n > 0)
	{
	    // Replace zero and CSI by a special key code.
	    n = fix_ఆదానం_బఫరు(buf + len, n);
	    len += n;
	    waited = 0;
	}
	లేకపోతే ఒకవేళ (len > 0)
	    ++waited;	    // keep track of the waiting time

	// Incomplete termcode and not timed out yet: get more characters
	ఒకవేళ ((n = check_termcode(1, buf, buflen, &len)) < 0
	       && (!p_ttimeout || waited * 100L < (p_ttm < 0 ? p_tm : p_ttm)))
	    కొనసాగు;

	ఒకవేళ (n == KEYLEN_REMOVED)  // key code removed
	{
	    ఒకవేళ (must_redraw != 0 && !need_wait_return && (State
			& (బహుళకం_CMDLINE | బహుళకం_HITRETURN | బహుళకం_ASKMORE)) == 0)
	    {
		// Redrawing was postponed, do it now.
		update_screen(0);
		setcursor(); // put cursor back where it belongs
	    }
	    కొనసాగు;
	}
	ఒకవేళ (n > 0)		// found a termcode: adjust length
	    len = n;
	ఒకవేళ (len == 0)		// nothing typed yet
	    కొనసాగు;

	// Handle modifier and/or special key code.
	n = buf[0];
	ఒకవేళ (n == K_SPECIAL)
	{
	    n = గమ్యం_SPECIAL(buf[1], buf[2]);
	    ఒకవేళ (buf[1] == KS_MODIFIER
		    || n == K_IGNORE
		    || (is_mouse_కీలకం(n) && n != K_LEFTMOUSE)
#నిర్వచితమైతే FEAT_GUI
		    || n == K_VER_SCROLLBAR
		    || n == K_HOR_SCROLLBAR
#అయితే_ముగింపు
	       )
	    {
		ఒకవేళ (buf[1] == KS_MODIFIER)
		    mod_ముసుగు = buf[2];
		len -= 3;
		ఒకవేళ (len > 0)
		    mch_memmove(buf, buf + 3, (size_t)len);
		కొనసాగు;
	    }
	    ఆపు;
	}
	ఒకవేళ (has_mbyte)
	{
	    ఒకవేళ (MB_BYTE2LEN(n) > len)
		కొనసాగు;	// more bytes to get
	    buf[len >= buflen ? buflen - 1 : len] = NUL;
	    n = (*mb_ptr2char)(buf);
	}
#నిర్వచితమైతే UNIX
	ఒకవేళ (n == intr_char)
	    n = ESC;
#అయితే_ముగింపు
	ఆపు;
    }
    vim_free(buf);

    mapped_ctrl_c = save_mapped_ctrl_c;
    తిరిగి విలీనం_modify_Other_కీలకాలు(n, &mod_ముసుగు);
}

// For overflow detection, add a digit safely to an int value.
    స్థిర పూర్ణాంకం
vim_append_digit_int(పూర్ణాంకం *విలువ, పూర్ణాంకం digit)
{
    పూర్ణాంకం x = *విలువ;
    ఒకవేళ (x > ((గరిష్ఠ_పూర్ణాంకం - digit) / 10))
	తిరిగి FAIL;
    *విలువ = x * 10 + digit;
    తిరిగి OK;
}

/*
 * Get a number from the user.
 * When "mouse_used" is not NULL allow using the mouse.
 */
    పూర్ణాంకం
get_సంఖ్య(
    పూర్ణాంకం	    colon,			// allow colon to abort
    పూర్ణాంకం	    *mouse_వాడిన)
{
    పూర్ణాంకం	n = 0;
    పూర్ణాంకం	c;
    పూర్ణాంకం typed = 0;

    ఒకవేళ (mouse_వాడిన != ఖాళీ)
	*mouse_వాడిన = FALSE;

    // When not printing messages, the user won't know what to type, return a
    // zero (as if CR was hit).
    ఒకవేళ (సందేశం_silent != 0)
	తిరిగి 0;

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు
    ++no_mapping;
    ++allow_కీలకాలు;		// no mapping here, but recognize keys
    కొరకు (;;)
    {
	windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
	c = సురక్షిత_vgetc();
	ఒకవేళ (VIM_ISDIGIT(c))
	{
	    ఒకవేళ (vim_append_digit_int(&n, c - '0') == FAIL)
		తిరిగి 0;
	    సందేశం_putchar(c);
	    ++typed;
	}
	లేకపోతే ఒకవేళ (c == K_DEL || c == K_KDEL || c == K_BS || c == Ctrl_H)
	{
	    ఒకవేళ (typed > 0)
	    {
		సందేశం_puts("\b \b");
		--typed;
	    }
	    n /= 10;
	}
	లేకపోతే ఒకవేళ (mouse_వాడిన != ఖాళీ && c == K_LEFTMOUSE)
	{
	    *mouse_వాడిన = TRUE;
	    n = mouse_అడ్డువరుస + 1;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    ఒకవేళ (!exmode_చురుకైన)
		cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	    skip_redraw = TRUE;	    // skip redraw once
	    do_redraw = FALSE;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (c == Ctrl_C || c == ESC || c == 'q')
	{
	    n = 0;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (c == CAR || c == NL )
	    ఆపు;
    }
    --no_mapping;
    --allow_కీలకాలు;
    తిరిగి n;
}

/*
 * Ask the user to enter a number.
 * When "mouse_used" is not NULL allow using the mouse and in that case return
 * the line number.
 */
    పూర్ణాంకం
prompt_for_సంఖ్య(పూర్ణాంకం *mouse_వాడిన)
{
    పూర్ణాంకం		i;
    పూర్ణాంకం		save_cmdline_అడ్డువరుస;
    పూర్ణాంకం		save_State;

    // When using ":silent" assume that <CR> was entered.
    ఒకవేళ (mouse_వాడిన != ఖాళీ)
	సందేశం_puts(_("Type number and <Enter> or click with the mouse (q or empty cancels): "));
    లేకపోతే
	సందేశం_puts(_("Type number and <Enter> (q or empty cancels): "));

    // Set the state such that text can be selected/copied/pasted and we still
    // get mouse events. redraw_after_callback() will not redraw if cmdline_row
    // is zero.
    save_cmdline_అడ్డువరుస = cmdline_అడ్డువరుస;
    cmdline_అడ్డువరుస = 0;
    save_State = State;
    State = బహుళకం_CMDLINE;
    // May show different mouse shape.
    setmouse();

    i = get_సంఖ్య(TRUE, mouse_వాడిన);
    ఒకవేళ (కీలకం_Typed)
    {
	// don't call wait_return() now
	ఒకవేళ (సందేశం_అడ్డువరుస > 0)
	    cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస - 1;
	need_wait_return = FALSE;
	సందేశం_didany = FALSE;
	సందేశం_didout = FALSE;
    }
    లేకపోతే
	cmdline_అడ్డువరుస = save_cmdline_అడ్డువరుస;
    State = save_State;
    // May need to restore mouse shape.
    setmouse();

    తిరిగి i;
}

    శూన్య
msgmore(దీర్ఘ n)
{
    దీర్ఘ pn;

    ఒకవేళ (global_busy	    // no messages now, wait until global is finished
	    || !messaging())  // 'lazyredraw' set, don't do messages now
	తిరిగి;

    // We don't want to overwrite another important message, but do overwrite
    // a previous "more lines" or "fewer lines" message, so that "5dd" and
    // then "put" reports the last action.
    ఒకవేళ (keep_సందేశం != ఖాళీ && !keep_సందేశం_మరిన్ని)
	తిరిగి;

    ఒకవేళ (n > 0)
	pn = n;
    లేకపోతే
	pn = -n;

    ఒకవేళ (pn > p_report)
    {
	ఒకవేళ (n > 0)
	    vim_snprintf(సందేశం_buf, సందేశం_BUF_LEN,
		    NGETTEXT("%ld more line", "%ld more lines", pn), pn);
	లేకపోతే
	    vim_snprintf(సందేశం_buf, సందేశం_BUF_LEN,
		    NGETTEXT("%ld line less", "%ld fewer lines", pn), pn);
	ఒకవేళ (got_int)
	    vim_strcat((char_u *)సందేశం_buf, (char_u *)_(" (Interrupted)"),
								  సందేశం_BUF_LEN);
	ఒకవేళ (సందేశం(సందేశం_buf))
	{
	    సమితి_keep_సందేశం((char_u *)సందేశం_buf, 0);
	    keep_సందేశం_మరిన్ని = TRUE;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    శూన్య
beep_flush(శూన్య)
{
    ఒకవేళ (emsg_silent == 0)
    {
	flush_buffers(FLUSH_కనీస);
	vim_beep(BO_ERROR);
    }
}

/*
 * Give a warning for an error. "val" is one of the BO_ values, e.g., BO_OPER.
 */
    శూన్య
vim_beep(గుర్తులేని val)
{
#నిర్వచితమైతే FEAT_EVAL
    called_vim_beep = TRUE;
#అయితే_ముగింపు

    ఒకవేళ (emsg_silent != 0 || లోపల_assert_fails)
	తిరిగి;

    ఒకవేళ (!((bo_flags & val) || (bo_flags & BO_ALL)))
    {
#నిర్వచితమైతే ELAPSED_FUNC
	స్థిర పూర్ణాంకం		did_init = FALSE;
	స్థిర elapsed_T	ప్రారంభం_tv;

	// Only beep once per half a second, otherwise a sequence of beeps
	// would freeze Vim.
	ఒకవేళ (!did_init || ELAPSED_FUNC(ప్రారంభం_tv) > 500)
	{
	    did_init = TRUE;
	    ELAPSED_INIT(ప్రారంభం_tv);
#అయితే_ముగింపు
	    ఒకవేళ (p_vb
#నిర్వచితమైతే FEAT_GUI
		    // While the GUI is starting up the termcap is set for
		    // the GUI but the output still goes to a terminal.
		    && !(gui.లోపల_use && gui.starting)
#అయితే_ముగింపు
	       )
	    {
		బహిర్గతం_str_cf(T_VB);
#నిర్వచితమైతే FEAT_VTP
		// No restore color information, refresh the screen.
		ఒకవేళ (has_vtp_working() != 0
# ifdef FEAT_TERMGUICOLORS
			&& (p_tgc || (!p_tgc && t_colors >= 256))
# endif
		   )
		{
		    redraw_later(UPD_CLEAR);
		    update_screen(0);
		    redrawcmd();
		}
#అయితే_ముగింపు
	    }
	    లేకపోతే
		బహిర్గతం_char(BELL);
#నిర్వచితమైతే ELAPSED_FUNC
	}
#అయితే_ముగింపు
    }

    // When 'debug' contains "beep" produce a message.  If we are sourcing
    // a script or executing a function give the user a hint where the beep
    // comes from.
    ఒకవేళ (vim_strchr(p_debug, 'e') != ఖాళీ)
    {
	సందేశం_ఆధారం(HL_ATTR(HLF_W));
	సందేశం_attr(_("Beep!"), HL_ATTR(HLF_W));
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
    శూన్య
init_homedir(శూన్య)
{
    char_u  *var;

    // In case we are called a second time (when 'encoding' changes).
    VIM_CLEAR(homedir);

#నిర్వచితమైతే VMS
    var = mch_getenv((char_u *)"SYS$LOGIN");
#లేకపోతే
    var = mch_getenv((char_u *)"HOME");
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
    /*
     * Typically, $HOME is not defined on Windows, unless the user has
     * specifically defined it for Vim's sake.  However, on Windows NT
     * platforms, $HOMEDRIVE and $HOMEPATH are automatically defined for
     * each user.  Try constructing $HOME from these.
     */
    ఒకవేళ (var == ఖాళీ || *var == NUL)
    {
	char_u *homedrive, *homepath;

	homedrive = mch_getenv((char_u *)"HOMEDRIVE");
	homepath = mch_getenv((char_u *)"HOMEPATH");
	ఒకవేళ (homepath == ఖాళీ || *homepath == NUL)
	    homepath = (char_u *)"\\";
	ఒకవేళ (homedrive != ఖాళీ
			   && STRLEN(homedrive) + STRLEN(homepath) < MAXPATHL)
	{
	    sprintf((అక్షరం *)NameBuff, "%s%s", homedrive, homepath);
	    ఒకవేళ (NameBuff[0] != NUL)
		var = NameBuff;
	}
    }

    ఒకవేళ (var == ఖాళీ)
	var = mch_getenv((char_u *)"USERPROFILE");

    /*
     * Weird but true: $HOME may contain an indirect reference to another
     * variable, esp. "%USERPROFILE%".  Happens when $USERPROFILE isn't set
     * when $HOME is being set.
     */
    ఒకవేళ (var != ఖాళీ && *var == '%')
    {
	char_u	*p;
	char_u	*ఘాతాంకం;

	p = vim_strchr(var + 1, '%');
	ఒకవేళ (p != ఖాళీ)
	{
	    vim_strncpy(NameBuff, var + 1, p - (var + 1));
	    ఘాతాంకం = mch_getenv(NameBuff);
	    ఒకవేళ (ఘాతాంకం != ఖాళీ && *ఘాతాంకం != NUL
					&& STRLEN(ఘాతాంకం) + STRLEN(p) < MAXPATHL)
	    {
		vim_snprintf((అక్షరం *)NameBuff, MAXPATHL, "%s%s", ఘాతాంకం, p + 1);
		var = NameBuff;
	    }
	}
    }

    ఒకవేళ (var != ఖాళీ && *var == NUL)	// empty is same as not set
	var = ఖాళీ;

    ఒకవేళ (enc_utf8 && var != ఖాళీ)
    {
	పూర్ణాంకం	len;
	char_u  *pp = ఖాళీ;

	// Convert from active codepage to UTF-8.  Other conversions are
	// not done, because they would fail for non-ASCII characters.
	acp_గమ్యం_enc(var, (పూర్ణాంకం)STRLEN(var), &pp, &len);
	ఒకవేళ (pp != ఖాళీ)
	{
	    homedir = pp;
	    తిరిగి;
	}
    }

    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    ఒకవేళ (var == ఖాళీ)
	var = (char_u *)"C:/";
#అయితే_ముగింపు

    ఒకవేళ (var != ఖాళీ)
    {
#నిర్వచితమైతే UNIX
	/*
	 * Change to the directory and get the actual path.  This resolves
	 * links.  Don't do it when we can't return.
	 */
	ఒకవేళ (mch_dirname(NameBuff, MAXPATHL) == OK
					  && mch_chdir((అక్షరం *)NameBuff) == 0)
	{
	    ఒకవేళ (!mch_chdir((అక్షరం *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    ఒకవేళ (mch_chdir((అక్షరం *)NameBuff) != 0)
		emsg(_(e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory));
	}
#అయితే_ముగింపు
	homedir = vim_strsave(var);
    }
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_homedir(శూన్య)
{
    vim_free(homedir);
}

    శూన్య
free_users(శూన్య)
{
    ga_clear_అక్షరశ్రేణులు(&ga_users);
}
#అయితే_ముగింపు

#ఒకవేళ defined(MSWIN)
/*
 * Initialize $VIM and $VIMRUNTIME when 'enc' is updated.
 */
    శూన్య
init_vimdir(శూన్య)
{
    పూర్ణాంకం	    mustfree;
    char_u  *p;

    mch_get_exe_name();

    mustfree = FALSE;
    didset_vim = FALSE;
    p = vim_getenv((char_u *)"VIM", &mustfree);
    ఒకవేళ (mustfree)
	vim_free(p);

    mustfree = FALSE;
    didset_vimruntime = FALSE;
    p = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
    ఒకవేళ (mustfree)
	vim_free(p);
}
#అయితే_ముగింపు

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(char_u *src)
{
    తిరిగి expand_env_save_opt(src, FALSE, ఖాళీ);
}

/*
 * Idem, but when "one" is TRUE handle the string as one file name, only
 * expand "~" at the start.
 */
    char_u *
expand_env_save_opt(char_u *src, పూర్ణాంకం one, char_u *esc_chars)
{
    char_u	*p;

    p = alloc(MAXPATHL);
    ఒకవేళ (p != ఖాళీ)
	expand_env_esc(src, p, MAXPATHL, esc_chars, one, ఖాళీ);
    తిరిగి p;
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
    పూర్ణాంకం		dstlen)		// maximum length of the result
{
    తిరిగి expand_env_esc(src, dst, dstlen, ఖాళీ, FALSE, ఖాళీ);
}

    size_t
expand_env_esc(
    char_u	*srcp,		// input string e.g. "$HOME/vim.hlp"
    char_u	*dst,		// where to put the result
    పూర్ణాంకం		dstlen,		// maximum length of the result
    char_u	*esc_chars,	// chars to escape in expanded vars
    పూర్ణాంకం		one,		// "srcp" is one file name
    char_u	*startstr)	// start again after this (can be NULL)
{
    char_u	*src;
    char_u	*తోక;
    పూర్ణాంకం		c;
    char_u	*var;
    పూర్ణాంకం		copy_char;
    పూర్ణాంకం		mustfree;	// var was allocated, need to free it later
    పూర్ణాంకం		at_ప్రారంభం = TRUE; // at start of a name
    పూర్ణాంకం		startstr_len = 0;
    char_u	*dst_ప్రారంభం = dst;

    ఒకవేళ (startstr != ఖాళీ)
	startstr_len = (పూర్ణాంకం)STRLEN(startstr);

    src = skipwhite(srcp);
    --dstlen;		    // leave one char space for "\,"
    అయితే (*src && dstlen > 0)
    {
#నిర్వచితమైతే FEAT_EVAL
	// Skip over `=expr`.
	ఒకవేళ (src[0] == '`' && src[1] == '=')
	{
	    size_t len;

	    var = src;
	    src += 2;
	    (శూన్య)skip_సమాసం(&src, ఖాళీ);
	    ఒకవేళ (*src == '`')
		++src;
	    len = src - var;
	    ఒకవేళ (len > (size_t)dstlen)
		len = dstlen;
	    vim_strncpy(dst, var, len);
	    dst += len;
	    dstlen -= (పూర్ణాంకం)len;
	    కొనసాగు;
	}
#అయితే_ముగింపు
	copy_char = TRUE;
	ఒకవేళ ((*src == '$'
#నిర్వచితమైతే VMS
		    && at_ప్రారంభం
#అయితే_ముగింపు
	   )
#ఒకవేళ defined(MSWIN)
		|| *src == '%'
#అయితే_ముగింపు
		|| (*src == '~' && at_ప్రారంభం))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be appended.
	     */
	    ఒకవేళ (*src != '~')				// environment var
	    {
		తోక = src + 1;
		var = dst;
		c = dstlen - 1;

#నిర్వచితమైతే UNIX
		// Unix has ${var-name} type environment vars
		ఒకవేళ (*తోక == '{' && !vim_isIDc('{'))
		{
		    తోక++;	// ignore '{'
		    అయితే (c-- > 0 && *తోక && *తోక != '}')
			*var++ = *తోక++;
		}
		లేకపోతే
#అయితే_ముగింపు
		{
		    అయితే (c-- > 0 && *తోక != NUL && ((vim_isIDc(*తోక))
#ఒకవేళ defined(MSWIN)
			    || (*src == '%' && *తోక != '%')
#అయితే_ముగింపు
			    ))
			*var++ = *తోక++;
		}

#ఒకవేళ defined(MSWIN) || defined(UNIX)
# ifdef UNIX
		ఒకవేళ (src[1] == '{' && *తోక != '}')
# లేకపోతే
		ఒకవేళ (*src == '%' && *తోక != '%')
# endif
		    var = ఖాళీ;
		లేకపోతే
		{
# ifdef UNIX
		    ఒకవేళ (src[1] == '{')
# లేకపోతే
		    ఒకవేళ (*src == '%')
# endif
			++తోక;
#అయితే_ముగింపు
		    *var = NUL;
		    var = vim_getenv(dst, &mustfree);
#ఒకవేళ defined(MSWIN) || defined(UNIX)
		}
#అయితే_ముగింపు
	    }
							// home directory
	    లేకపోతే ఒకవేళ (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != ఖాళీ)
	    {
		var = homedir;
		తోక = src + 1;
	    }
	    లేకపోతే					// user directory
	    {
#ఒకవేళ defined(UNIX) || (defined(VMS) && defined(USER_HOME))
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		తోక = src;
		var = dst;
		c = dstlen - 1;
		అయితే (	   c-- > 0
			&& *తోక
			&& vim_isfilec(*తోక)
			&& !vim_ispathsep(*తోక))
		    *var++ = *తోక++;
		*var = NUL;
# ifdef UNIX
		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
#  ఒకవేళ defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    // Note: memory allocated by getpwnam() is never freed.
		    // Calling endpwent() apparently doesn't help.
		    నిర్మాణం passwd *pw = (*dst == NUL)
					? ఖాళీ : getpwnam((అక్షరం *)dst + 1);

		    var = (pw == ఖాళీ) ? ఖాళీ : (char_u *)pw->pw_dir;
		}
		ఒకవేళ (var == ఖాళీ)
#  endif
		{
		    expand_T	xpc;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    var = ExpandOne(&xpc, dst, ఖాళీ,
				WILD_కూడు_SLASH|WILD_SILENT, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}

# లేకపోతే	// !UNIX, thus VMS
		/*
		 * USER_HOME is a comma-separated list of
		 * directories to search for the user account in.
		 */
		{
		    char_u	test[MAXPATHL], paths[MAXPATHL];
		    size_t	testlen;
		    char_u	*మార్గం, *తదుపరి_మార్గం, *ptr;
		    stat_T	st;

		    STRCPY(paths, USER_HOME);
		    తదుపరి_మార్గం = paths;
		    అయితే (*తదుపరి_మార్గం)
		    {
			కొరకు (మార్గం = తదుపరి_మార్గం; *తదుపరి_మార్గం && *తదుపరి_మార్గం != ',';
				తదుపరి_మార్గం++);
			ఒకవేళ (*తదుపరి_మార్గం)
			    *తదుపరి_మార్గం++ = NUL;
			testlen = vim_snprintf_safelen(
			    (అక్షరం *)test,
			    పరిమాణం(test),
			    "%s/%s",
			    మార్గం,
			    dst + 1);
			ఒకవేళ (mch_stat(test, &st) == 0)
			{
			    var = alloc(testlen + 1);
			    ఒకవేళ (var != ఖాళీ)
			    {
				STRCPY(var, test);
				mustfree = TRUE;
			    }
			    ఆపు;
			}
		    }
		}
# endif // UNIX
#లేకపోతే
		// cannot expand user's home directory, so don't try
		var = ఖాళీ;
		తోక = (char_u *)"";	// for gcc
#అయితే_ముగింపు // UNIX || VMS
	    }

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
	    // If 'shellslash' is set change backslashes to forward slashes.
	    // Can't use slash_adjust(), p_ssl may be set temporarily.
	    ఒకవేళ (p_ssl && var != ఖాళీ && vim_strchr(var, '\\') != ఖాళీ)
	    {
		char_u	*p = vim_strsave(var);

		ఒకవేళ (p != ఖాళీ)
		{
		    ఒకవేళ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		    forward_slash(var);
		}
	    }
#అయితే_ముగింపు

	    // If "var" contains any character from "esc_chars", escape it
	    // with a backslash.  The historical use is escaping spaces so
	    // that ":e ~/tt" works when $HOME contains a space.
	    ఒకవేళ (esc_chars != ఖాళీ && var != ఖాళీ
		    && vim_strpbrk(var, esc_chars) != ఖాళీ)
	    {
		char_u	*p = vim_strsave_escaped(var, esc_chars);

		ఒకవేళ (p != ఖాళీ)
		{
		    ఒకవేళ (mustfree)
			vim_free(var);
		    var = p;
		    mustfree = TRUE;
		}
	    }

	    ఒకవేళ (var != ఖాళీ && *var != NUL)
	    {
		c = (పూర్ణాంకం)STRLEN(var);

		ఒకవేళ (c + STRLEN(తోక) + 1 < (గుర్తులేని)dstlen)
		{
		    STRCPY(dst, var);
		    dstlen -= c;
		    // if var[] ends in a path separator and tail[] starts
		    // with it, skip a character
		    ఒకవేళ (after_pathsep(dst, dst + c)
#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME) || defined(AMIGA)
			    && dst[c - 1] != ':'
#అయితే_ముగింపు
			    && vim_ispathsep(*తోక))
			++తోక;
		    dst += c;
		    src = తోక;
		    copy_char = FALSE;
		}
	    }
	    ఒకవేళ (mustfree)
		vim_free(var);
	}

	ఒకవేళ (copy_char)	    // copy at least one char
	{
	    /*
	     * Recognize the start of a new name, for '~'.
	     * Don't do this when "one" is TRUE, to avoid expanding "~" in
	     * ":edit foo ~ foo".
	     */
	    at_ప్రారంభం = FALSE;
	    ఒకవేళ (src[0] == '\\' && src[1] != NUL)
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    లేకపోతే ఒకవేళ ((src[0] == ' ' || src[0] == ',') && !one)
		at_ప్రారంభం = TRUE;
	    ఒకవేళ (dstlen > 0)
	    {
		*dst++ = *src++;
		--dstlen;

		ఒకవేళ (startstr != ఖాళీ && src - startstr_len >= srcp
			&& STRNCMP(src - startstr_len, startstr,
							    startstr_len) == 0)
		    at_ప్రారంభం = TRUE;
	    }
	}

    }
    *dst = NUL;

    తిరిగి (size_t)(dst - dst_ప్రారంభం);
}

/*
 * If the string between "p" and "pend" ends in "name/", return "pend" minus
 * the length of "name/".  Otherwise return "pend".
 */
    స్థిర char_u *
remove_తోక(char_u *p, char_u *pend, char_u *name)
{
    పూర్ణాంకం		len = (పూర్ణాంకం)STRLEN(name) + 1;
    char_u	*newend = pend - len;

    ఒకవేళ (newend >= p
	    && fnamencmp(newend, name, len - 1) == 0
	    && (newend == p || after_pathsep(p, newend)))
	తిరిగి newend;
    తిరిగి pend;
}

/*
 * Check if the directory "vimdir/<version>" or "vimdir/runtime" exists.
 * Return NULL if not, return its name in allocated memory otherwise.
 */
    స్థిర char_u *
vim_సంస్కరణ_dir(char_u *vimdir)
{
    string_T	p;
    size_t	vimdir_len;

    ఒకవేళ (vimdir == ఖాళీ || *vimdir == NUL)
	తిరిగి ఖాళీ;
    vimdir_len = STRLEN(vimdir);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)VIM_సంస్కరణ_NODOT, STRLEN_LITERAL(VIM_సంస్కరణ_NODOT), TRUE, &p);
    ఒకవేళ (p.string != ఖాళీ && mch_isdir(p.string))
	తిరిగి p.string;
    vim_free(p.string);
    concat_fnames(vimdir, vimdir_len,
	(char_u *)RUNTIME_DIRNAME, STRLEN_LITERAL(RUNTIME_DIRNAME), TRUE, &p);
    ఒకవేళ (p.string != ఖాళీ && mch_isdir(p.string))
    {
	string_T    fname;

	concat_fnames(p.string, p.నిడివి,
	    (char_u *)"defaults.vim", STRLEN_LITERAL("defaults.vim"), TRUE, &fname);

	// Check that "defaults.vim" exists in this directory, to avoid picking
	// up a stray "runtime" directory, it would make many tests fail in
	// mysterious ways.
	ఒకవేళ (fname.string != ఖాళీ)
	{
	    పూర్ణాంకం exists = file_is_readable(fname.string);

	    vim_free(fname.string);
	    ఒకవేళ (exists)
		తిరిగి p.string;
	}
    }
    vim_free(p.string);
    తిరిగి ఖాళీ;
}

/*
 * Vim's version of getenv().
 * Special handling of $HOME, $VIM and $VIMRUNTIME.
 * Also does ACP to 'enc' conversion for Win32.
 * "mustfree" is set to TRUE when the returned string is allocated.  It must be
 * initialized to FALSE by the caller.
 */
    char_u *
vim_getenv(char_u *name, పూర్ణాంకం *mustfree)
{
    char_u	*p = ఖాళీ;
    char_u	*pend;
    పూర్ణాంకం		vimruntime;
#నిర్వచితమైతే MSWIN
    WCHAR	*wn, *wp;

    // use "C:/" when $HOME is not set
    ఒకవేళ (STRCMP(name, "HOME") == 0)
	తిరిగి homedir;

    // Use Wide function
    wn = enc_గమ్యం_utf16(name, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి ఖాళీ;

    wp = _wgetenv(wn);
    vim_free(wn);

    ఒకవేళ (wp != ఖాళీ && *wp == NUL)   // empty is the same as not set
	wp = ఖాళీ;

    ఒకవేళ (wp != ఖాళీ)
    {
	p = utf16_గమ్యం_enc(wp, ఖాళీ);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి ఖాళీ;

	*mustfree = TRUE;
	తిరిగి p;
    }
#లేకపోతే
    p = mch_getenv(name);
    ఒకవేళ (p != ఖాళీ && *p == NUL)	    // empty is the same as not set
	p = ఖాళీ;

    ఒకవేళ (p != ఖాళీ)
	తిరిగి p;

# ifdef __HAIKU__
    // special handling for user settings directory...
    ఒకవేళ (STRCMP(name, "BE_USER_SETTINGS") == 0)
    {
	స్థిర అక్షరం user_Settings_మార్గం[MAXPATHL];

	ఒకవేళ (కనుగొను_directory(B_USER_SETTINGS_DIRECTORY, 0, false,
					   user_Settings_మార్గం, MAXPATHL) == B_OK)
	    తిరిగి (char_u *)user_Settings_మార్గం;
	లేకపోతే
	    తిరిగి ఖాళీ;
    }
# endif
#అయితే_ముగింపు

    // handling $VIMRUNTIME and $VIM is below, bail out if it's another name.
    vimruntime = (STRCMP(name, "VIMRUNTIME") == 0);
    ఒకవేళ (!vimruntime && STRCMP(name, "VIM") != 0)
	తిరిగి ఖాళీ;

    /*
     * When expanding $VIMRUNTIME fails, try using $VIM/vim<version> or $VIM.
     * Don't do this when default_vimruntime_dir is non-empty.
     */
    ఒకవేళ (vimruntime
#నిర్వచితమైతే HAVE_PATHDEF
	    && *default_vimruntime_dir == NUL
#అయితే_ముగింపు
       )
    {
#నిర్వచితమైతే MSWIN
	// Use Wide function
	wp = _wgetenv(L"VIM");
	ఒకవేళ (wp != ఖాళీ && *wp == NUL)	    // empty is the same as not set
	    wp = ఖాళీ;
	ఒకవేళ (wp != ఖాళీ)
	{
	    char_u *q = utf16_గమ్యం_enc(wp, ఖాళీ);
	    ఒకవేళ (q != ఖాళీ)
	    {
		p = vim_సంస్కరణ_dir(q);
		*mustfree = TRUE;
		ఒకవేళ (p == ఖాళీ)
		    p = q;
	    }
	}
#లేకపోతే
	p = mch_getenv((char_u *)"VIM");
	ఒకవేళ (p != ఖాళీ && *p == NUL)	    // empty is the same as not set
	    p = ఖాళీ;
	ఒకవేళ (p != ఖాళీ)
	{
	    p = vim_సంస్కరణ_dir(p);
	    ఒకవేళ (p != ఖాళీ)
		*mustfree = TRUE;
	    లేకపోతే
		p = mch_getenv((char_u *)"VIM");
	}
#అయితే_ముగింపు
    }

    /*
     * When expanding $VIM or $VIMRUNTIME fails, try using:
     * - the directory name from 'helpfile' (unless it contains '$')
     * - the executable name from argv[0]
     */
    ఒకవేళ (p == ఖాళీ)
    {
	ఒకవేళ (p_hf != ఖాళీ && vim_strchr(p_hf, '$') == ఖాళీ)
	    p = p_hf;
#నిర్వచితమైతే USE_EXE_NAME
	/*
	 * Use the name of the executable, obtained from argv[0].
	 */
	లేకపోతే
	    p = exe_name;
#అయితే_ముగింపు
	ఒకవేళ (p != ఖాళీ)
	{
	    // remove the file name
	    pend = gettail(p);

	    // remove "doc/" from 'helpfile', if present
	    ఒకవేళ (p == p_hf)
		pend = remove_తోక(p, pend, (char_u *)"doc");

#నిర్వచితమైతే USE_EXE_NAME
# ifdef MACOS_X
	    // remove "MacOS" from exe_name and add "Resources/vim"
	    ఒకవేళ (p == exe_name)
	    {
		char_u	*pend1;
		char_u	*pnew;

		pend1 = remove_తోక(p, pend, (char_u *)"MacOS");
		ఒకవేళ (pend1 != pend)
		{
		    pnew = alloc(pend1 - p + 15);
		    ఒకవేళ (pnew != ఖాళీ)
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
	    ఒకవేళ (p == exe_name)
		pend = remove_తోక(p, pend, (char_u *)"src");
#అయితే_ముగింపు

	    // for $VIM, remove "runtime/" or "vim54/", if present
	    ఒకవేళ (!vimruntime)
	    {
		pend = remove_తోక(p, pend, (char_u *)RUNTIME_DIRNAME);
		pend = remove_తోక(p, pend, (char_u *)VIM_సంస్కరణ_NODOT);
	    }

	    // remove trailing path separator
	    ఒకవేళ (pend > p && after_pathsep(p, pend))
		--pend;

#నిర్వచితమైతే MACOS_X
	    ఒకవేళ (p == exe_name || p == p_hf)
#అయితే_ముగింపు
		// check that the result is a directory name
		p = vim_strnsave(p, pend - p);

	    ఒకవేళ (p != ఖాళీ && !mch_isdir(p))
		VIM_CLEAR(p);
	    లేకపోతే
	    {
#నిర్వచితమైతే USE_EXE_NAME
		// may add "/vim54" or "/runtime" if it exists
		ఒకవేళ (vimruntime && (pend = vim_సంస్కరణ_dir(p)) != ఖాళీ)
		{
		    vim_free(p);
		    p = pend;
		}
#అయితే_ముగింపు
		*mustfree = TRUE;
	    }
	}
    }

#నిర్వచితమైతే HAVE_PATHDEF
    // When there is a pathdef.c file we can use default_vim_dir and
    // default_vimruntime_dir
    ఒకవేళ (p == ఖాళీ)
    {
	// Only use default_vimruntime_dir when it is not empty
	ఒకవేళ (vimruntime && *default_vimruntime_dir != NUL)
	{
	    p = default_vimruntime_dir;
	    *mustfree = FALSE;
	}
	లేకపోతే ఒకవేళ (*default_vim_dir != NUL)
	{
	    ఒకవేళ (vimruntime && (p = vim_సంస్కరణ_dir(default_vim_dir)) != ఖాళీ)
		*mustfree = TRUE;
	    లేకపోతే
	    {
		p = default_vim_dir;
		*mustfree = FALSE;
	    }
	}
    }
#అయితే_ముగింపు

    /*
     * Set the environment variable, so that the new value can be found fast
     * next time, and others can also use it (e.g. Perl).
     */
    ఒకవేళ (p != ఖాళీ)
    {
	ఒకవేళ (vimruntime)
	{
	    vim_setenv((char_u *)"VIMRUNTIME", p);
	    didset_vimruntime = TRUE;
	}
	లేకపోతే
	{
	    vim_setenv((char_u *)"VIM", p);
	    didset_vim = TRUE;
	}
    }
    తిరిగి p;
}

    శూన్య
vim_unsetenv(char_u *var)
{
#నిర్వచితమైతే HAVE_UNSETENV
    unsetenv((అక్షరం *)var);
#లేకపోతే
    vim_setenv(var, (char_u *)"");
#అయితే_ముగింపు
}

/*
 * Removes environment variable "name" and take care of side effects.
 */
    శూన్య
vim_unsetenv_ext(char_u *var)
{
    vim_unsetenv(var);

    // "homedir" is not cleared, keep using the old value until $HOME is set.
    ఒకవేళ (STRICMP(var, "VIM") == 0)
	didset_vim = FALSE;
    లేకపోతే ఒకవేళ (STRICMP(var, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Set environment variable "name" and take care of side effects.
 */
    శూన్య
vim_setenv_ext(char_u *name, char_u *val)
{
    vim_setenv(name, val);
    ఒకవేళ (STRICMP(name, "HOME") == 0)
	init_homedir();
    లేకపోతే ఒకవేళ (didset_vim && STRICMP(name, "VIM") == 0)
	didset_vim = FALSE;
    లేకపోతే ఒకవేళ (didset_vimruntime && STRICMP(name, "VIMRUNTIME") == 0)
	didset_vimruntime = FALSE;
}
#అయితే_ముగింపు

/*
 * Our portable version of setenv.
 */
    శూన్య
vim_setenv(char_u *name, char_u *val)
{
#నిర్వచితమైతే HAVE_SETENV
    mch_setenv((అక్షరం *)name, (అక్షరం *)val, 1);
#లేకపోతే
    char_u	*envbuf;

    /*
     * Putenv does not copy the string, it has to remain
     * valid.  The allocated memory will never be freed.
     */
    envbuf = alloc(STRLEN(name) + STRLEN(val) + 2);
    ఒకవేళ (envbuf != ఖాళీ)
    {
	sprintf((అక్షరం *)envbuf, "%s=%s", name, val);
	putenv((అక్షరం *)envbuf);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GETTEXT
    /*
     * When setting $VIMRUNTIME adjust the directory to find message
     * translations to $VIMRUNTIME/lang.
     */
    ఒకవేళ (*val != NUL && STRICMP(name, "VIMRUNTIME") == 0)
    {
	char_u	*buf = concat_str(val, (char_u *)"/lang");

	ఒకవేళ (buf != ఖాళీ)
	{
	    bindtextdomain(VIMPACKAGE, (అక్షరం *)buf);
	    vim_free(buf);
	}
    }
#అయితే_ముగింపు
}

/*
 * Function given to ExpandGeneric() to obtain an environment variable name.
 */
    char_u *
get_env_name(
    expand_T	*xp వాడని,
    పూర్ణాంకం		idx)
{
#ఒకవేళ defined(AMIGA)
    // No environ[] on the Amiga.
    తిరిగి ఖాళీ;
#లేకపోతే
# ifndef __WIN32__
    // Borland C++ 5.2 has this in a header file.
    బాహ్య అక్షరం		**environ;
# endif
    char_u		*str;
    పూర్ణాంకం			n;

    str = (char_u *)environ[idx];
    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (n = 0; n < EXPAND_BUF_LEN - 1; ++n)
    {
	ఒకవేళ (str[n] == '=' || str[n] == NUL)
	    ఆపు;
	xp->xp_buf[n] = str[n];
    }
    xp->xp_buf[n] = NUL;
    తిరిగి xp->xp_buf;
#అయితే_ముగింపు
}

/*
 * Add a user name to the list of users in ga_users.
 * Do nothing if user name is NULL or empty.
 */
    స్థిర శూన్య
కూడు_user(char_u *user, పూర్ణాంకం need_copy)
{
    char_u	*user_copy = (user != ఖాళీ && need_copy)
						    ? vim_strsave(user) : user;

    ఒకవేళ (user_copy == ఖాళీ || *user_copy == NUL || ga_grow(&ga_users, 1) == FAIL)
    {
	ఒకవేళ (need_copy)
	    vim_free(user_copy);
	తిరిగి;
    }
    ((char_u **)(ga_users.ga_దత్తాంశం))[ga_users.ga_len++] = user_copy;
}

/*
 * Find all user names for user completion.
 * Done only once and then cached.
 */
    స్థిర శూన్య
init_users(శూన్య)
{
    స్థిర పూర్ణాంకం	lazy_init_పూర్తి = FALSE;

    ఒకవేళ (lazy_init_పూర్తి)
	తిరిగి;

    lazy_init_పూర్తి = TRUE;
    ga_init2(&ga_users, పరిమాణం(char_u *), 20);

#ఒకవేళ defined(HAVE_GETPWENT) && defined(HAVE_PWD_H)
    {
	నిర్మాణం passwd*	pw;

	setpwent();
	అయితే ((pw = getpwent()) != ఖాళీ)
	    కూడు_user((char_u *)pw->pw_name, TRUE);
	endpwent();
    }
#లేదావేళ defined(MSWIN)
    {
	DWORD		nusers = 0, ntotal = 0, i;
	PUSER_INFO_0	uinfo;

	ఒకవేళ (NetUserEnum(ఖాళీ, 0, 0, (LPBYTE *) &uinfo, MAX_PREFERRED_నిడివి,
				       &nusers, &ntotal, ఖాళీ) == NERR_Success)
	{
	    కొరకు (i = 0; i < nusers; i++)
		కూడు_user(utf16_గమ్యం_enc(uinfo[i].usri0_name, ఖాళీ), FALSE);

	    Net_Api_బఫరు_Free(uinfo);
	}
    }
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_GETPWNAM)
    {
	char_u	*user_env = mch_getenv((char_u *)"USER");

	// The $USER environment variable may be a valid remote user name (NIS,
	// LDAP) not already listed by getpwent(), as getpwent() only lists
	// local user names.  If $USER is not already listed, check whether it
	// is a valid remote user name using getpwnam() and if it is, add it to
	// the list of user names.

	ఒకవేళ (user_env != ఖాళీ && *user_env != NUL)
	{
	    పూర్ణాంకం	i;

	    కొరకు (i = 0; i < ga_users.ga_len; i++)
	    {
		char_u	*local_user = ((char_u **)ga_users.ga_దత్తాంశం)[i];

		ఒకవేళ (STRCMP(local_user, user_env) == 0)
		    ఆపు;
	    }

	    ఒకవేళ (i == ga_users.ga_len)
	    {
		నిర్మాణం passwd	*pw = getpwnam((అక్షరం *)user_env);

		ఒకవేళ (pw != ఖాళీ)
		    కూడు_user((char_u *)pw->pw_name, TRUE);
	    }
	}
    }
#అయితే_ముగింపు
}

/*
 * Function given to ExpandGeneric() to obtain user names.
 */
    char_u*
get_users(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    init_users();
    ఒకవేళ (idx < ga_users.ga_len)
	తిరిగి ((char_u **)ga_users.ga_దత్తాంశం)[idx];
    తిరిగి ఖాళీ;
}

/*
 * Check whether name matches a user name. Return:
 * 0 if name does not match any user name.
 * 1 if name partially matches the beginning of a user name.
 * 2 is name fully matches a user name.
 */
    పూర్ణాంకం
సరిపోలిక_user(char_u *name)
{
    పూర్ణాంకం i;
    పూర్ణాంకం n = (పూర్ణాంకం)STRLEN(name);
    పూర్ణాంకం ఫలితం = 0;

    init_users();
    కొరకు (i = 0; i < ga_users.ga_len; i++)
    {
	ఒకవేళ (STRCMP(((char_u **)ga_users.ga_దత్తాంశం)[i], name) == 0)
	    తిరిగి 2; // full match
	ఒకవేళ (STRNCMP(((char_u **)ga_users.ga_దత్తాంశం)[i], name, n) == 0)
	    ఫలితం = 1; // partial match
    }
    తిరిగి ఫలితం;
}

    స్థిర శూన్య
prepare_గమ్యం_exit(శూన్య)
{
#ఒకవేళ defined(SIGHUP) && defined(SIG_IGN)
    // Ignore SIGHUP, because a dropped connection causes a read error, which
    // makes Vim exit and then handling SIGHUP causes various reentrance
    // problems.
    mch_signal(SIGHUP, SIG_IGN);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	gui.dying = true;
	బహిర్గతం_trash();	// trash any pending output
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off);

	// When "full_screen" was reset, e.g. by deathtrap(), settmode() below
	// returns without switching the mouse off, so do it here.
	ఒకవేళ (!నిండిన_screen)
	    mch_setmouse(FALSE);

	/*
	 * Switch terminal mode back now, so messages end up on the "normal"
	 * screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
	stoptermcap();
	బహిర్గతం_flush();
    }
}

/*
 * Preserve files and exit.
 * When called IObuff must contain a message.
 * NOTE: This may be called from deathtrap() in a signal handler, avoid unsafe
 * functions, such as allocating memory.
 */
    శూన్య
preserve_exit(శూన్య)
{
    buf_T	*buf;

    prepare_గమ్యం_exit();

    // Setting this will prevent free() calls.  That avoids calling free()
    // recursively when free() was invoked with a bad pointer.
    really_exiting = TRUE;

    బహిర్గతం_str(IObuff);
    screen_ప్రారంభం();		    // don't know where cursor is now
    బహిర్గతం_flush();

    ml_close_notmod();		    // close all not-modified buffers

    FOR_ALL_BUFFERS(buf)
    {
	ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ && buf->b_ml.ml_mfp->mf_fname != ఖాళీ)
	{
	    బహిర్గతం_STR("Vim: preserving files...\r\n");
	    screen_ప్రారంభం();	    // don't know where cursor is now
	    బహిర్గతం_flush();
	    ml_sync_all(FALSE, FALSE);	// preserve all swap files
	    ఆపు;
	}
    }

    ml_close_all(FALSE);	    // close all memfiles, without deleting

    బహిర్గతం_STR("Vim: Finished.\r\n");

    getout(1);
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#నిర్వచితం_కాకపోతే BREAKCHECK_SKIP
# define BREAKCHECK_SKIP 1000
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	breakcheck_లెక్క = 0;

    శూన్య
పంక్తి_breakcheck(శూన్య)
{
    ఒకవేళ (++breakcheck_లెక్క >= BREAKCHECK_SKIP)
    {
	breakcheck_లెక్క = 0;
	ui_breakcheck();
    }
}

/*
 * Like line_breakcheck() but check 10 times less often.
 */
    శూన్య
fast_breakcheck(శూన్య)
{
    ఒకవేళ (++breakcheck_లెక్క >= BREAKCHECK_SKIP * 10)
    {
	breakcheck_లెక్క = 0;
	ui_breakcheck();
    }
}

#ఒకవేళ defined(FEAT_SPELL)
/*
 * Like line_breakcheck() but check 100 times less often.
 */
    శూన్య
veryfast_breakcheck(శూన్య)
{
    ఒకవేళ (++breakcheck_లెక్క >= BREAKCHECK_SKIP * 100)
    {
	breakcheck_లెక్క = 0;
	ui_breakcheck();
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL) || (defined(HAVE_LOCALE_H) || defined(X_LOCALE))

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
    పూర్ణాంకం		flags,		// can be SHELL_SILENT
    పూర్ణాంకం		*ret_len)
{
    char_u	*tempname;
    char_u	*command;
    char_u	*బఫరు = ఖాళీ;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i = 0;
    FILE	*fd;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి ఖాళీ;

    // Keep the file reserved until the shell opens it.  On MS-Windows,
    // deleting it here would let another Vim process reuse the same name.
    ఒకవేళ ((tempname = vim_tempname('o', TRUE)) == ఖాళీ)
    {
	emsg(_(e_cant_get_తాత్కాలిక_file_name));
	తిరిగి ఖాళీ;
    }

    // Add the redirection stuff
    command = make_filter_cmd(cmd, infile, tempname);
    ఒకవేళ (command == ఖాళీ)
	వెళ్ళు పూర్తి;

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
    fd = mch_fopen((అక్షరం *)tempname, "r");
# లేకపోతే
    fd = mch_fopen((అక్షరం *)tempname, READBIN);
# endif

    // Not being able to seek means we can't read the file.
    ఒకవేళ (fd == ఖాళీ
	    || fseek(fd, 0L, SEEK_END) == -1
	    || (len = ftell(fd)) == -1		// get size of temp file
	    || fseek(fd, 0L, SEEK_SET) == -1)	// back to the start
    {
	semsg(_(e_cannot_read_from_str_2), tempname);
	ఒకవేళ (fd != ఖాళీ)
	    fclose(fd);
	వెళ్ళు పూర్తి;
    }

    బఫరు = alloc(len + 1);
    ఒకవేళ (బఫరు != ఖాళీ)
	i = (పూర్ణాంకం)fread((అక్షరం *)బఫరు, (size_t)1, (size_t)len, fd);
    fclose(fd);
    ఒకవేళ (బఫరు == ఖాళీ)
	వెళ్ళు పూర్తి;
# ifdef VMS
    len = i;	// VMS doesn't give us what we asked for...
# endif
    ఒకవేళ (i != len)
    {
	semsg(_(e_cant_read_file_str), tempname);
	VIM_CLEAR(బఫరు);
    }
    లేకపోతే ఒకవేళ (ret_len == ఖాళీ)
    {
	// Change NUL into SOH, otherwise the string is truncated.
	కొరకు (i = 0; i < len; ++i)
	    ఒకవేళ (బఫరు[i] == NUL)
		బఫరు[i] = 1;

	బఫరు[len] = NUL;	// make sure the buffer is terminated
    }
    లేకపోతే
	*ret_len = len;

పూర్తి:
    mch_remove(tempname);
    vim_free(tempname);
    తిరిగి బఫరు;
}

# ఒకవేళ defined(FEAT_EVAL)

    స్థిర శూన్య
get_cmd_output_as_rettv(
    typval_T	*argvars,
    typval_T	*rettv,
    పూర్ణాంకం		retlist)
{
    char_u	*res = ఖాళీ;
    char_u	*p;
    char_u	*infile = ఖాళీ;
    పూర్ణాంకం		err = FALSE;
    FILE	*fd;
    list_T	*list = ఖాళీ;
    పూర్ణాంకం		flags = SHELL_SILENT;
    పూర్ణాంకం		use_argv = FALSE;
    అక్షరం	**argv = ఖాళీ;
    పూర్ణాంకం		argc = 0;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    ఒకవేళ (check_restricted() || check_secure())
	వెళ్ళు errret;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_or_list_arg(argvars, 0) == FAIL
		|| check_for_opt_string_or_సంఖ్య_or_list_arg(argvars, 1)
								      == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	/*
	 * Write the text to a temp file, to be used for input of the shell
	 * command.
	 */
	ఒకవేళ ((infile = vim_tempname('i', TRUE)) == ఖాళీ)
	{
	    emsg(_(e_cant_get_తాత్కాలిక_file_name));
	    వెళ్ళు errret;
	}

	fd = mch_fopen((అక్షరం *)infile, WRITEBIN);
	ఒకవేళ (fd == ఖాళీ)
	{
	    semsg(_(e_cant_open_file_str), infile);
	    వెళ్ళు errret;
	}
	ఒకవేళ (argvars[1].v_type == VAR_సంఖ్య)
	{
	    linenr_T	lnum;
	    buf_T	*buf;

	    buf = buflist_findnr(argvars[1].vval.v_సంఖ్య);
	    ఒకవేళ (buf == ఖాళీ)
	    {
		semsg(_(e_బఫరు_nr_does_not_exist), argvars[1].vval.v_సంఖ్య);
		fclose(fd);
		వెళ్ళు errret;
	    }

	    కొరకు (lnum = 1; lnum <= buf->b_ml.ml_పంక్తి_లెక్క; lnum++)
	    {
		కొరకు (p = ml_get_buf(buf, lnum, FALSE); *p != NUL; ++p)
		    ఒకవేళ (putc(*p == '\n' ? NUL : *p, fd) == EOF)
		    {
			err = TRUE;
			ఆపు;
		    }
		ఒకవేళ (putc(NL, fd) == EOF)
		{
		    err = TRUE;
		    ఆపు;
		}
	    }
	}
	లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_LIST)
	{
	    ఒకవేళ (write_list(fd, argvars[1].vval.v_list, TRUE) == FAIL)
		err = TRUE;
	}
	లేకపోతే
	{
	    size_t	len;
	    char_u	buf[NUMBUFLEN];

	    p = tv_get_string_buf_chk(&argvars[1], buf);
	    ఒకవేళ (p == ఖాళీ)
	    {
		fclose(fd);
		వెళ్ళు errret;		// type error; errmsg already given
	    }
	    len = STRLEN(p);
	    ఒకవేళ (len > 0 && fwrite(p, len, 1, fd) != 1)
		err = TRUE;
	}
	ఒకవేళ (fclose(fd) != 0)
	    err = TRUE;
	ఒకవేళ (err)
	{
	    emsg(_(e_error_writing_తాత్కాలిక_file));
	    వెళ్ళు errret;
	}
    }

    // When the command is a List, execute directly without the shell.
    ఒకవేళ (argvars[0].v_type == VAR_LIST)
    {
	list_T	*l = argvars[0].vval.v_list;

	ఒకవేళ (l == ఖాళీ || l->lv_len < 1)
	{
	    emsg(_(e_invalid_వాదం));
	    వెళ్ళు errret;
	}
	ఒకవేళ (build_argv_from_list(l, &argv, &argc) == FAIL)
	    వెళ్ళు errret;
	ఒకవేళ (argc == 0 || *skipwhite((char_u *)argv[0]) == NUL)
	{
	    emsg(_(e_invalid_వాదం));
	    వెళ్ళు errret;
	}
	use_argv = TRUE;

	ఒకవేళ (p_verbose > 3)
	{
	    పూర్ణాంకం		i;
	    garray_T	ga;

	    verbose_enter();
	    ga_init2(&ga, 1, 200);
	    కొరకు (i = 0; i < argc; ++i)
	    {
		ఒకవేళ (i > 0)
		    ga_append(&ga, ' ');
		ga_concat(&ga, (char_u *)argv[i]);
	    }
	    ga_append(&ga, NUL);
	    smsg(_("Executing directly: \"%s\""), (అక్షరం *)ga.ga_దత్తాంశం);
	    సందేశం_putchar_attr('\n', 0);
	    cursor_on();
	    verbose_leave();
	    ga_clear(&ga);
	}
    }

    // Omit SHELL_COOKED when invoked with ":silent".  Avoids that the shell
    // echoes typeahead, that messes up the display.
    ఒకవేళ (!సందేశం_silent)
	flags += SHELL_COOKED;

    ఒకవేళ (retlist)
    {
	పూర్ణాంకం		len;
	listitem_T	*li;
	char_u		*s = ఖాళీ;
	char_u		*ప్రారంభం;
	char_u		*చివర;
	పూర్ణాంకం		i;

	ఒకవేళ (use_argv)
	    res = mch_get_cmd_output_ప్రత్యక్ష(argv, infile, flags, &len);
	లేకపోతే
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, &len);
	ఒకవేళ (res == ఖాళీ)
	    వెళ్ళు errret;

	list = list_alloc();
	ఒకవేళ (list == ఖాళీ)
	    వెళ్ళు errret;

	కొరకు (i = 0; i < len; ++i)
	{
	    ప్రారంభం = res + i;
	    అయితే (i < len && res[i] != NL)
		++i;
	    చివర = res + i;

	    s = alloc(చివర - ప్రారంభం + 1);
	    ఒకవేళ (s == ఖాళీ)
		వెళ్ళు errret;

	    కొరకు (p = s; ప్రారంభం < చివర; ++p, ++ప్రారంభం)
		*p = *ప్రారంభం == NUL ? NL : *ప్రారంభం;
	    *p = NUL;

	    li = listitem_alloc();
	    ఒకవేళ (li == ఖాళీ)
	    {
		vim_free(s);
		వెళ్ళు errret;
	    }
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.v_తాళం = 0;
	    li->li_tv.vval.v_string = s;
	    list_append(list, li);
	}

	rettv_list_సమితి(rettv, list);
	list = ఖాళీ;
    }
    లేకపోతే
    {
	ఒకవేళ (use_argv)
	    res = mch_get_cmd_output_ప్రత్యక్ష(argv, infile, flags, ఖాళీ);
	లేకపోతే
	    res = get_cmd_output(tv_get_string(&argvars[0]), infile, flags, ఖాళీ);
#  ifdef USE_CRNL
	// translate <CR><NL> into <NL>
	ఒకవేళ (res != ఖాళీ)
	{
	    char_u	*s, *d;

	    d = res;
	    కొరకు (s = res; *s; ++s)
	    {
		ఒకవేళ (s[0] == CAR && s[1] == NL)
		    ++s;
		*d++ = *s;
	    }
	    *d = NUL;
	}
#  endif
	rettv->vval.v_string = res;
	res = ఖాళీ;
    }

errret:
    ఒకవేళ (argv != ఖాళీ)
    {
	పూర్ణాంకం i;
	కొరకు (i = 0; argv[i] != ఖాళీ; i++)
	    vim_free(argv[i]);
	vim_free(argv);
    }
    ఒకవేళ (infile != ఖాళీ)
    {
	mch_remove(infile);
	vim_free(infile);
    }
    ఒకవేళ (res != ఖాళీ)
	vim_free(res);
    ఒకవేళ (list != ఖాళీ)
	list_free(list);
}

/*
 * "system()" function
 */
    శూన్య
f_system(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, FALSE);
}

/*
 * "systemlist()" function
 */
    శూన్య
f_systemlist(typval_T *argvars, typval_T *rettv)
{
    get_cmd_output_as_rettv(argvars, rettv, TRUE);
}
# endif // FEAT_EVAL

#అయితే_ముగింపు

/*
 * Return TRUE when need to go to Insert mode because of 'insertmode'.
 * Don't do this when still processing a command or a mapping.
 * Don't do this when inside a ":normal" command.
 */
    పూర్ణాంకం
goto_im(శూన్య)
{
    తిరిగి (p_im && stuff_ఖాళీలు() && typebuf_typed());
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
get_isolated_shell_name(శూన్య)
{
    char_u *p;

#నిర్వచితమైతే MSWIN
    p = gettail(p_sh);
    p = vim_strnsave(p, skiptowhite(p) - p);
#లేకపోతే
    p = skiptowhite(p_sh);
    ఒకవేళ (*p == NUL)
    {
	// No white space, use the tail.
	p = vim_strsave(gettail(p_sh));
    }
    లేకపోతే
    {
	char_u  *p1, *p2;

	// Find the last path separator before the space.
	p1 = p_sh;
	కొరకు (p2 = p_sh; p2 < p; MB_PTR_ADV(p2))
	    ఒకవేళ (vim_ispathsep(*p2))
		p1 = p2 + 1;
	p = vim_strnsave(p1, p - p1);
    }
#అయితే_ముగింపు
    తిరిగి p;
}

/*
 * Check if the "://" of a URL is at the pointer, return URL_SLASH.
 * Also check for ":\\", which MS Internet Explorer accepts, return
 * URL_BACKSLASH.
 */
    పూర్ణాంకం
మార్గం_is_url(char_u *p)
{
    ఒకవేళ (STRNCMP(p, "://", (size_t)3) == 0)
	తిరిగి URL_SLASH;
    లేకపోతే ఒకవేళ (STRNCMP(p, ":\\\\", (size_t)3) == 0)
	తిరిగి URL_BACKSLASH;
    తిరిగి 0;
}

/*
 * Check if "fname" starts with "name://" or "name:\\".
 * Return URL_SLASH for "name://", URL_BACKSLASH for "name:\\".
 * Return zero otherwise.
 */
    పూర్ణాంకం
మార్గం_with_url(char_u *fname)
{
    char_u *p;

    // We accept alphabetic characters and a dash in scheme part.
    // RFC 3986 allows for more, but it increases the risk of matching
    // non-URL text.

    // first character must be alpha
    ఒకవేళ (!ASCII_ISALPHA(*fname))
	తిరిగి 0;

    // check body: alpha or dash
    కొరకు (p = fname + 1; (ASCII_ISALPHA(*p) || (*p == '-')); ++p)
	;

    // check last char is not a dash
    ఒకవేళ (p[-1] == '-')
	తిరిగి 0;

    // "://" or ":\\" must follow
    తిరిగి మార్గం_is_url(p);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the dictionary of v:event.
 * Save and clear the value in case it already has items.
 */
    dict_T *
get_v_event(save_v_event_T *sve)
{
    dict_T	*v_event = get_vim_var_dict(VV_EVENT);

    ఒకవేళ (v_event->dv_hashtab.ht_వాడిన > 0)
    {
	// recursive use of v:event, save, make empty and restore later
	sve->sve_did_save = TRUE;
	sve->sve_hashtab = v_event->dv_hashtab;
	hash_init(&v_event->dv_hashtab);
    }
    లేకపోతే
	sve->sve_did_save = FALSE;
    తిరిగి v_event;
}

    శూన్య
restore_v_event(dict_T *v_event, save_v_event_T *sve)
{
    dict_free_contents(v_event);
    ఒకవేళ (sve->sve_did_save)
	v_event->dv_hashtab = sve->sve_hashtab;
    లేకపోతే
	hash_init(&v_event->dv_hashtab);
}
#అయితే_ముగింపు

/*
 * Fires a ModeChanged autocmd event if appropriate.
 */
    శూన్య
may_trigger_modechanged(శూన్య)
{
#నిర్వచితమైతే FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;
    char_u	    curr_బహుళకం[బహుళకం_MAX_నిడివి];
    char_u	    నమూనా_buf[2 * బహుళకం_MAX_నిడివి];

    // Skip this when got_int is set, the autocommand will not be executed.
    // Better trigger it next time.
    ఒకవేళ (!has_modechanged() || got_int)
	తిరిగి;

    get_బహుళకం(curr_బహుళకం);
    ఒకవేళ (STRCMP(curr_బహుళకం, చివరి_బహుళకం) == 0)
	తిరిగి;

    v_event = get_v_event(&save_v_event);
    (శూన్య)dict_కూడు_string(v_event, "new_mode", curr_బహుళకం);
    (శూన్య)dict_కూడు_string(v_event, "old_mode", చివరి_బహుళకం);
    dict_సమితి_వస్తువులు_ro(v_event);

    // concatenate modes in format "old_mode:new_mode"
    vim_snprintf((అక్షరం *)నమూనా_buf, పరిమాణం(నమూనా_buf), "%s:%s", చివరి_బహుళకం,
	    curr_బహుళకం);

    apply_autocmds(EVENT_MODECHANGED, నమూనా_buf, ఖాళీ, FALSE, curbuf);
    STRCPY(చివరి_బహుళకం, curr_బహుళకం);

    restore_v_event(v_event, &save_v_event);
#అయితే_ముగింపు
}

// For overflow detection, add a digit safely to a long value.
    పూర్ణాంకం
vim_append_digit_long(దీర్ఘ *విలువ, పూర్ణాంకం digit)
{
    దీర్ఘ x = *విలువ;
    ఒకవేళ (x > ((LONG_MAX - (దీర్ఘ)digit) / 10))
	తిరిగి FAIL;
    *విలువ = x * 10 + (దీర్ఘ)digit;
    తిరిగి OK;
}

// Return something that fits into an int.
    పూర్ణాంకం
trim_గమ్యం_int(vimlong_T x)
{
    తిరిగి x > గరిష్ఠ_పూర్ణాంకం ? గరిష్ఠ_పూర్ణాంకం : x < INT_MIN ? INT_MIN : x;
}
