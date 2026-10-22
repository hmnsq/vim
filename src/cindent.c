/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * cindent.c: C indentation related functions
 *
 * Many of C-indenting functions originally come from Eric Fischer.
 *
 * Below "XXX" means that this function may unlock the current line.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

// values for the "lookfor" state
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_INITIAL		0
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_IF		1
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_DO		2
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_CASE		3
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_ANY		4
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_ಪದ		5
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_UNTERM		6
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_SCOPEDECL	7
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_NOBREAK		8
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_CPP_BASECLASS	9
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_ENUM_OR_INIT	10
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_JS_ಕೀಲಿ		11
#ವ್ಯಾಖ್ಯಾನಿಸು LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    ಪೂರ್ಣಾಂಕ
cin_is_cinword(char_u *ಸಾಲು)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ಪೂರ್ಣಾಂಕ		cinw_len;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		len;

    cinw_len = (ಪೂರ್ಣಾಂಕ)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ಆದರೆ (cinw_buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಸಾಲು = skipwhite(ಸಾಲು);
    ಗಾಗಿ (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ಆದರೆ (STRNCMP(ಸಾಲು, cinw_buf, len) == 0
		&& (!vim_iswordc(ಸಾಲು[len]) || !vim_iswordc(ಸಾಲು[len - 1])))
	{
	    retval = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
    }
    vim_free(cinw_buf);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ಸ್ಥಿರ char_u *
skip_string(char_u *p)
{
    ಪೂರ್ಣಾಂಕ	    i;

    // We loop, because strings may be concatenated: "date""time".
    ಗಾಗಿ ( ; ; ++p)
    {
	ಆದರೆ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ಆದರೆ (p[1] == NUL)		    // ' at end of line
		ನಿಲ್ಲಿಸು;
	    i = 2;
	    ಆದರೆ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ಆಗ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ಆದರೆ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ಮುಂದುವರಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '"')		    // start of string
	{
	    ಗಾಗಿ (++p; p[0]; ++p)
	    {
		ಆದರೆ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '"')	    // end of string
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (p[0] == '"')
		ಮುಂದುವರಿ; // continue for another string
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ಆದರೆ (paren != ಖಾಲಿ)
	    {
		size_t delim_len = paren - delim;

		ಗಾಗಿ (p += 3; *p; ++p)
		    ಆದರೆ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ನಿಲ್ಲಿಸು;
		    }
		ಆದರೆ (p[0] == '"')
		    ಮುಂದುವರಿ; // continue for another string
	    }
	}
	ನಿಲ್ಲಿಸು;				    // no string found
    }
    ಆದರೆ (!*p)
	--p;				    // backup from NUL
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ಪೂರ್ಣಾಂಕ
is_pos_ಒಳ_string(char_u *ಸಾಲು, colnr_T ಲಂಬಸಾಲು)
{
    char_u *p;

    ಗಾಗಿ (p = ಸಾಲು; *p && (colnr_T)(p - ಸಾಲು) < ಲಂಬಸಾಲು; ++p)
	p = skip_string(p);
    ಹಿಂತಿರುಗಿ !((colnr_T)(p - ಸಾಲು) <= ಲಂಬಸಾಲು);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ಪೂರ್ಣಾಂಕ
check_linecomment(char_u *ಸಾಲು)
{
    char_u  *p;

    p = ಸಾಲು;
    // skip Lispish one-line comments
    ಆದರೆ (curbuf->b_p_lisp)
    {
	ಆದರೆ (vim_strchr(p, ';') != ಖಾಲಿ) // there may be comments
	{
	    ಪೂರ್ಣಾಂಕ ಒಳ_str = FALSE;	// inside of string

	    p = ಸಾಲು;		// scan from start
	    ಆಗ ((p = vim_strpbrk(p, (char_u *)"\";")) != ಖಾಲಿ)
	    {
		ಆದರೆ (*p == '"')
		{
		    ಆದರೆ (ಒಳ_str)
		    {
			ಆದರೆ (*(p - 1) != '\\') // skip escaped quote
			    ಒಳ_str = FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p == ಸಾಲು || ((p - ಸಾಲು) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			ಒಳ_str = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಒಳ_str && ((p - ಸಾಲು) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_ಒಳ_string(ಸಾಲು, (colnr_T)(p - ಸಾಲು)))
		    ನಿಲ್ಲಿಸು;	// found!
		++p;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	ಗಾಗಿ ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ಆದರೆ (p[0] == '/' && p[1] == '/'
				 && (p == ಸಾಲು || p[-1] != '*' || p[2] != '*'))
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*p == NUL)
	    p = ಖಾಲಿ;
    }

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ MAXCOL;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ಸ್ಥಿರ pos_T *
ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment(ಶೂನ್ಯ)	    // XXX
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment(ಪೂರ್ಣಾಂಕ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ಪೂರ್ಣಾಂಕ		cur_maxcomment = ind_maxcomment;

    ಗಾಗಿ (;;)
    {
	pos = findmatchlimit(ಖಾಲಿ, '*', FM_BACKWARD, cur_maxcomment);
	ಆದರೆ (pos == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ಆದರೆ (!is_pos_ಒಳ_string(ml_get(pos->lnum), pos->ಲಂಬಸಾಲು))
	    ನಿಲ್ಲಿಸು;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ಆದರೆ (cur_maxcomment <= 0)
	{
	    pos = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಆರಂಭ_rawstring(ಪೂರ್ಣಾಂಕ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ಪೂರ್ಣಾಂಕ		cur_maxcomment = ind_maxcomment;

    ಗಾಗಿ (;;)
    {
	pos = findmatchlimit(ಖಾಲಿ, 'R', FM_BACKWARD, cur_maxcomment);
	ಆದರೆ (pos == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ಆದರೆ (!is_pos_ಒಳ_string(ml_get(pos->lnum), pos->ಲಂಬಸಾಲು))
	    ನಿಲ್ಲಿಸು;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ಆದರೆ (cur_maxcomment <= 0)
	{
	    pos = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ಸ್ಥಿರ pos_T *
ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(linenr_T *is_raw)	    // XXX
{
    ಸ್ಥಿರ pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment(curbuf->b_ind_maxcomment);
    ಆದರೆ (comment_pos != ಖಾಲಿ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ಆದರೆ (comment_pos == ಖಾಲಿ || (rs_pos != ಖಾಲಿ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ಆದರೆ (is_raw != ಖಾಲಿ && rs_pos != ಖಾಲಿ)
	    *is_raw = rs_pos->lnum;
	ಹಿಂತಿರುಗಿ rs_pos;
    }
    ಹಿಂತಿರುಗಿ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ಪೂರ್ಣಾಂಕ
cindent_on(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (!p_paste && (curbuf->b_p_cin
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#ಆದರೆ_ಅಂತ್ಯ
		    ));
}

// Find result cache for cpp_baseclass
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    ಸಿಕ್ಕಿತು;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ಸ್ಥಿರ char_u *
cin_skipcomment(char_u *s)
{
    ಆಗ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ಆದರೆ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*s != '/')
	    ನಿಲ್ಲಿಸು;
	++s;
	ಆದರೆ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*s != '*')
	    ನಿಲ್ಲಿಸು;
	ಗಾಗಿ (++s; *s; ++s)	// skip slash-star comment
	    ಆದರೆ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ನಿಲ್ಲಿಸು;
	    }
    }
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_nocode(char_u *s)
{
    ಹಿಂತಿರುಗಿ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_iscomment(char_u *p)
{
    ಹಿಂತಿರುಗಿ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_islinecomment(char_u *p)
{
    ಹಿಂತಿರುಗಿ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಸಾಲು_comment(ಶೂನ್ಯ) // XXX
{
    ಸ್ಥಿರ pos_T pos;
    char_u	 *ಸಾಲು;
    char_u	 *p;

    pos = curwin->w_cursor;
    ಆಗ (--pos.lnum > 0)
    {
	ಸಾಲು = ml_get(pos.lnum);
	p = skipwhite(ಸಾಲು);
	ಆದರೆ (cin_islinecomment(p))
	{
	    pos.ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
	    ಹಿಂತಿರುಗಿ &pos;
	}
	ಆದರೆ (*p != NUL)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_has_js_ಕೀಲಿ(char_u *ಪಠ್ಯ)
{
    char_u *s = skipwhite(ಪಠ್ಯ);
    ಪೂರ್ಣಾಂಕ	    quote = -1;

    ಆದರೆ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ಆದರೆ (!vim_isIDc(*s))	    // need at least one ID character
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಗ (vim_isIDc(*s))
	++s;
    ಆದರೆ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ಹಿಂತಿರುಗಿ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_islabel_skip(char_u **s)
{
    ಆದರೆ (!vim_isIDc(**s))	    // need at least one ID character
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಗ (vim_isIDc(**s))
    {
	ಆದರೆ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ಇಲ್ಲದಿದ್ದರೆ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ಹಿಂತಿರುಗಿ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ಪೂರ್ಣಾಂಕ	    ಸಿಕ್ಕಿತು = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ಆದರೆ (cinsd_buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ಪೂರ್ಣಾಂಕ)cinsd_len, ",");
	ಆದರೆ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ಆದರೆ (*skip == ':' && skip[1] != ':')
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    vim_free(cinsd_buf);
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_ispreproc(char_u *s)
{
    ಆದರೆ (*skipwhite(s) == '#')
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ಪೂರ್ಣಾಂಕ *amount)
{
    char_u	*ಸಾಲು = *pp;
    linenr_T	lnum = *lnump;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		candidate_amount = *amount;

    ಆದರೆ (*ಸಾಲು != NUL && ಸಾಲು[STRLEN(ಸಾಲು) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (cin_ispreproc(ಸಾಲು))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (lnum == 1)
	    ನಿಲ್ಲಿಸು;
	ಸಾಲು = ml_get(--lnum);
	ಆದರೆ (*ಸಾಲು == NUL || ಸಾಲು[STRLEN(ಸಾಲು) - 1] != '\\')
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (lnum != *lnump)
	*pp = ml_get(*lnump);
    ಆದರೆ (retval)
	*amount = candidate_amount;
    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_iselse(
    char_u  *p)
{
    ಆದರೆ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ಹಿಂತಿರುಗಿ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
}

/*
 * Recognize a line that starts with '{' or '}', or ends with ';', ',', '{' or
 * '}'.
 * Don't consider "} else" a terminated line.
 * If a line begins with an "else", only consider it terminated if no unmatched
 * opening braces follow (handle "else { foo();" correctly).
 * Return the character terminating the line (ending char's have precedence if
 * both apply in order to determine initializations).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isterminated(
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		incl_open,	// include '{' at the end as terminator
    ಪೂರ್ಣಾಂಕ		incl_comma)	// recognize a trailing comma
{
    char_u	ಸಿಕ್ಕಿತು_ಆರಂಭ = 0;
    ಚಿಹ್ನೆರಹಿತ	n_open = 0;
    ಪೂರ್ಣಾಂಕ		is_else = FALSE;

    s = cin_skipcomment(s);

    ಆದರೆ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	ಸಿಕ್ಕಿತು_ಆರಂಭ = *s;

    ಆದರೆ (!ಸಿಕ್ಕಿತು_ಆರಂಭ)
	is_else = cin_iselse(s);

    ಆಗ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ಆದರೆ (*s == '}' && n_open > 0)
	    --n_open;
	ಆದರೆ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ಹಿಂತಿರುಗಿ *s;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '{')
	{
	    ಆದರೆ (incl_open && cin_nocode(s + 1))
		ಹಿಂತಿರುಗಿ *s;
	    ಇಲ್ಲದಿದ್ದರೆ
		++n_open;
	}

	ಆದರೆ (*s)
	    s++;
    }
    ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_ಆರಂಭ;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_starts_with(char_u *s, ಅಕ್ಷರ *ಶಬ್ದ)
{
    ಪೂರ್ಣಾಂಕ l = (ಪೂರ್ಣಾಂಕ)STRLEN(ಶಬ್ದ);

    ಹಿಂತಿರುಗಿ (STRNCMP(s, ಶಬ್ದ, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isdefault(char_u *s)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_iscase(
    char_u *s,
    ಪೂರ್ಣಾಂಕ strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ಆದರೆ (cin_starts_with(s, "case"))
    {
	ಗಾಗಿ (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ಆದರೆ (*s == NUL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*s == ':')
	    {
		ಆದರೆ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	    ಆದರೆ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ಹಿಂತಿರುಗಿ FALSE;		// stop at comment
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '"')
	    {
		// JS etc.
		ಆದರೆ (strict)
		    ಹಿಂತಿರುಗಿ FALSE;		// stop at string
		ಇಲ್ಲದಿದ್ದರೆ
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (cin_isdefault(s))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_islabel(ಶೂನ್ಯ)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ಆದರೆ (cin_isdefault(s))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (cin_isscopedecl(s))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (!cin_islabel_skip(&s))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*ಸಾಲು;

    cursor_save = curwin->w_cursor;
    ಆಗ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	ಆದರೆ ((trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ)) != ಖಾಲಿ) // XXX
	    curwin->w_cursor = *trypos;

	ಸಾಲು = ml_get_curline();
	ಆದರೆ (cin_ispreproc(ಸಾಲು))	// ignore #defines, #if, etc.
	    ಮುಂದುವರಿ;
	ಆದರೆ (*(ಸಾಲು = cin_skipcomment(ಸಾಲು)) == NUL)
	    ಮುಂದುವರಿ;

	curwin->w_cursor = cursor_save;
	ಆದರೆ (cin_isterminated(ಸಾಲು, TRUE, FALSE)
		|| cin_isscopedecl(ಸಾಲು)
		|| cin_iscase(ಸಾಲು, TRUE)
		|| (cin_islabel_skip(&ಸಾಲು) && cin_nocode(ಸಾಲು)))
	    ಹಿಂತಿರುಗಿ TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    curwin->w_cursor = cursor_save;
    ಹಿಂತಿರುಗಿ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_ತುದಿಗಳು_ಒಳ(char_u *s, char_u *ಕಂಡುಹಿಡಿ)
{
    char_u	*p = s;
    char_u	*r;
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಕಂಡುಹಿಡಿ);

    ಆಗ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ಆದರೆ (STRNCMP(p, ಕಂಡುಹಿಡಿ, len) == 0)
	{
	    r = skipwhite(p + len);
	    ಆದರೆ (cin_nocode(r))
		ಹಿಂತಿರುಗಿ TRUE;
	}
	ಆದರೆ (*p != NUL)
	    ++p;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ಸ್ಥಿರ char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ಖಾಲಿ, *p = s;
    ಮಾಡು
    {
	r = p;
	p = cin_skipcomment(p);
	ಆದರೆ (*p)
	    p = skip_string(p);
    } ಆಗ (p != r);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ಖಾಲಿ;

    ಆಗ (*p)
    {
	ಆದರೆ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ಇಲ್ಲದಿದ್ದರೆ
	    p = cin_skip_comment_and_string(p + 1);
    }
    ಆದರೆ (!r)
	ಹಿಂತಿರುಗಿ FALSE;
    p = r; // p points now after '=' or "return"

    ಆದರೆ (cin_nocode(p))
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (*p == '&')
	p = cin_skipcomment(p + 1);

    ಆದರೆ (*p == '(') // skip a typecast
    {
	ಪೂರ್ಣಾಂಕ open_ಎಣಿಕೆ = 1;
	ಮಾಡು
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ಆದರೆ (cin_nocode(p))
		ಹಿಂತಿರುಗಿ TRUE;
	    open_ಎಣಿಕೆ += (*p == '(') - (*p == ')');
	} ಆಗ (open_ಎಣಿಕೆ);
	p = cin_skipcomment(p + 1);
	ಆದರೆ (cin_nocode(p))
	    ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆಗ (*p == '{')
	p = cin_skipcomment(p + 1);
    ಹಿಂತಿರುಗಿ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isinit(ಶೂನ್ಯ)
{
    char_u	*s;
    ಸ್ಥಿರ ಅಕ್ಷರ *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ಆದರೆ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ i, l;

	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(skip); ++i)
	{
	    l = (ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(skip[i]);
	    ಆದರೆ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (l != 0)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (cin_starts_with(s, "enum"))
	ಹಿಂತಿರುಗಿ TRUE;

    ಹಿಂತಿರುಗಿ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#ವ್ಯಾಖ್ಯಾನಿಸು ಕಂಡುಹಿಡಿ_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		has_name = FALSE;
    ಪೂರ್ಣಾಂಕ		has_name_ಆರಂಭ = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ಆಗ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ಆದರೆ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '{')
	    {
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_iswordc(*p))
	    {
		has_name_ಆರಂಭ = TRUE;
		ಆದರೆ (has_name)
		    ಹಿಂತಿರುಗಿ FALSE; // word character after skipping past name
		++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ಆದರೆ (!has_name_ಆರಂಭ || has_name)
		    ಹಿಂತಿರುಗಿ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ಆದರೆ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ಆಗ (*p != NUL)
	{
	    ಆದರೆ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '{')
	    {
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ಆದರೆ (has_string_literal)
		    ಹಿಂತಿರುಗಿ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ಆದರೆ (has_string_literal)
		    ಹಿಂತಿರುಗಿ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
	ಹಿಂತಿರುಗಿ has_string_literal ? TRUE : FALSE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ಸ್ಥಿರ char_u *
after_label(char_u *l)
{
    ಗಾಗಿ ( ; *l; ++l)
    {
	ಆದರೆ (*l == ':')
	{
	    ಆದರೆ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!cin_iscase(l + 1, FALSE))
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ಆದರೆ (*l == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    l = cin_skipcomment(l + 1);
    ಆದರೆ (*l == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	ಲಂಬಸಾಲು;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    fp.ಲಂಬಸಾಲು = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ಪೂರ್ಣಾಂಕ		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ಆದರೆ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ಆದರೆ (l == ಖಾಲಿ)		// just in case
	    l = ml_get_curline();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ಹಿಂತಿರುಗಿ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_ಮೊದಲ_id_amount(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು, *p, *s;
    ಪೂರ್ಣಾಂಕ		len;
    pos_T	fp;
    colnr_T	ಲಂಬಸಾಲು;

    ಸಾಲು = ml_get_curline();
    p = skipwhite(ಸಾಲು);
    len = (ಪೂರ್ಣಾಂಕ)(skiptowhite(p) - p);
    ಆದರೆ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ಪೂರ್ಣಾಂಕ)(skiptowhite(p) - p);
    }
    ಆದರೆ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ಆದರೆ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    ಗಾಗಿ (len = 0; vim_isIDc(p[len]); ++len)
	;
    ಆದರೆ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ಹಿಂತಿರುಗಿ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.ಲಂಬಸಾಲು = (colnr_T)(p - ಸಾಲು);
    getvcol(curwin, &fp, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು;
}

/*
 * Return the indent of the first non-blank after an equal sign.
 *       char *foo = "here";
 * Return zero if no (useful) equal sign found.
 * Return -1 if the line above "lnum" ends in a backslash.
 *      foo = "asdf\
 *	       asdf\
 *	       here";
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*ಸಾಲು;
    char_u	*s;
    colnr_T	ಲಂಬಸಾಲು;
    pos_T	fp;

    ಆದರೆ (lnum > 1)
    {
	ಸಾಲು = ml_get(lnum - 1);
	ಆದರೆ (*ಸಾಲು != NUL && ಸಾಲು[STRLEN(ಸಾಲು) - 1] == '\\')
	    ಹಿಂತಿರುಗಿ -1;
    }

    ಸಾಲು = s = ml_get(lnum);
    ಆಗ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ಖಾಲಿ)
    {
	ಆದರೆ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಇಲ್ಲದಿದ್ದರೆ
	    ++s;
    }
    ಆದರೆ (*s != '=')
	ಹಿಂತಿರುಗಿ 0;

    s = skipwhite(s + 1);
    ಆದರೆ (cin_nocode(s))
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.ಲಂಬಸಾಲು = (colnr_T)(s - ಸಾಲು);
    getvcol(curwin, &fp, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_skip2pos(pos_T *trypos)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*new_p;

    p = ಸಾಲು = ml_get(trypos->lnum);
    ಆಗ (*p && (colnr_T)(p - ಸಾಲು) < trypos->ಲಂಬಸಾಲು)
    {
	ಆದರೆ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    new_p = skip_string(p);
	    ಆದರೆ (new_p == p)
		++p;
	    ಇಲ್ಲದಿದ್ದರೆ
		p = new_p;
	}
    }
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
}

    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_char(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ಸ್ಥಿರ pos_T pos_copy;
    ಪೂರ್ಣಾಂಕ		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ಆದರೆ ((trypos = findmatchlimit(ಖಾಲಿ, c, FM_SKIPCOMM, ind_maxp_wk)) != ಖಾಲಿ)
    {
	// check if the ( is in a // comment
	ಆದರೆ ((colnr_T)cin_skip2pos(trypos) > trypos->ಲಂಬಸಾಲು)
	{
	    ind_maxp_wk = ind_maxparen - (ಪೂರ್ಣಾಂಕ)(cursor_save.lnum - trypos->lnum);
	    ಆದರೆ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;	// XXX
		ಹೋಗು retry;
	    }
	    trypos = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ಆದರೆ ((trypos_wk = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ)) != ಖಾಲಿ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ಪೂರ್ಣಾಂಕ)(cursor_save.lnum
			- trypos_wk->lnum);
		ಆದರೆ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    ಹೋಗು retry;
		}
		trypos = ಖಾಲಿ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ಹಿಂತಿರುಗಿ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(ಪೂರ್ಣಾಂಕ ind_maxparen)	// XXX
{
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(char_u *l, ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಕೊನೆ)
{
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		open_ಎಣಿಕೆ = 0;

    curwin->w_cursor.ಲಂಬಸಾಲು = 0;		    // default is start of line

    ಗಾಗಿ (i = 0; l[i] != NUL; i++)
    {
	i = (ಪೂರ್ಣಾಂಕ)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ಆದರೆ (l[i] == NUL)
	    ನಿಲ್ಲಿಸು;
	i = (ಪೂರ್ಣಾಂಕ)(skip_string(l + i) - l);    // ignore parens in quotes
	ಆದರೆ (l[i] == ಆರಂಭ)
	    ++open_ಎಣಿಕೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l[i] == ಕೊನೆ)
	{
	    ಆದರೆ (open_ಎಣಿಕೆ > 0)
		--open_ಎಣಿಕೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		curwin->w_cursor.ಲಂಬಸಾಲು = i;
		retval = TRUE;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Recognize the basic picture of a function declaration -- it needs to
 * have an open paren somewhere and a close paren at the end of the line and
 * no semicolons anywhere.
 * When a line ends in a comma we continue looking in the next line.
 * "sp" points to a string with the line.  When looking at other lines it must
 * be restored to the line.  When it's NULL fetch lines here.
 * "first_lnum" is where we start looking.
 * "min_lnum" is the line before which we will not be looking.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	ಮೊದಲ_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = ಮೊದಲ_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    pos_T	*trypos;
    ಪೂರ್ಣಾಂಕ		just_started = TRUE;

    ಆದರೆ (sp == ಖಾಲಿ)
	s = ml_get(lnum);
    ಇಲ್ಲದಿದ್ದರೆ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(s, '(', ')')
	&& (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen)) != ಖಾಲಿ)
    {
	lnum = trypos->lnum;
	ಆದರೆ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ಆದರೆ (cin_ispreproc(s))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಗ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ಆದರೆ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == ':')
	{
	    ಆದರೆ (*(s + 1) == ':')
		s += 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ಹಿಂತಿರುಗಿ FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ++s;
    }
    ಆದರೆ (*s != '(')
	ಹಿಂತಿರುಗಿ FALSE;		// ';', ' or "  before any () or no '('

    ಆಗ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ಆದರೆ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = ಮೊದಲ_lnum - 1;
	    s = ml_get(lnum);
	    ಆದರೆ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    ಹೋಗು ಮುಗಿದ;
	}
	ಆದರೆ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ಪೂರ್ಣಾಂಕ comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ನಿಲ್ಲಿಸು;
		s = ml_get(++lnum);
		ಆದರೆ (!cin_ispreproc(s))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ಆದರೆ (!just_started && (!comma && *s != ',' && *s != ')'))
		ನಿಲ್ಲಿಸು;
	    just_started = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

ಮುಗಿದ:
    ಆದರೆ (lnum != ಮೊದಲ_lnum && sp != ಖಾಲಿ)
	*sp = ml_get(ಮೊದಲ_lnum);

    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isif(char_u *p)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isdo(char_u *p)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    p = cin_skipcomment(p);
    ಆದರೆ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ಆದರೆ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	p = ml_get_curline();
	ಆಗ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.ಲಂಬಸಾಲು;
	}
	ಆದರೆ ((trypos = findmatchlimit(ಖಾಲಿ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ಖಾಲಿ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_is_if_for_while_before_ಸ್ಥಾನಾಂತರ(char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ *poffset)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = *poffset;

    ಆದರೆ (ಸ್ಥಾನಾಂತರ-- < 2)
	ಹಿಂತಿರುಗಿ 0;
    ಆಗ (ಸ್ಥಾನಾಂತರ > 2 && VIM_ISWHITE(ಸಾಲು[ಸ್ಥಾನಾಂತರ]))
	--ಸ್ಥಾನಾಂತರ;

    ಸ್ಥಾನಾಂತರ -= 1;
    ಆದರೆ (STRNCMP(ಸಾಲು + ಸ್ಥಾನಾಂತರ, "if", 2) == 0)
	ಹೋಗು probably_ಸಿಕ್ಕಿತು;

    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 1)
    {
	ಸ್ಥಾನಾಂತರ -= 1;
	ಆದರೆ (STRNCMP(ಸಾಲು + ಸ್ಥಾನಾಂತರ, "for", 3) == 0)
	    ಹೋಗು probably_ಸಿಕ್ಕಿತು;

	ಆದರೆ (ಸ್ಥಾನಾಂತರ >= 2)
	{
	    ಸ್ಥಾನಾಂತರ -= 2;
	    ಆದರೆ (STRNCMP(ಸಾಲು + ಸ್ಥಾನಾಂತರ, "while", 5) == 0)
		ಹೋಗು probably_ಸಿಕ್ಕಿತು;
	}
    }
    ಹಿಂತಿರುಗಿ 0;

probably_ಸಿಕ್ಕಿತು:
    ಆದರೆ (!ಸ್ಥಾನಾಂತರ || !vim_isIDc(ಸಾಲು[ಸ್ಥಾನಾಂತರ - 1]))
    {
	*poffset = ಸ್ಥಾನಾಂತರ;
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_iswhileofdo_ಕೊನೆ(ಪೂರ್ಣಾಂಕ terminated)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (terminated != ';')	// there must be a ';' at the end
	ಹಿಂತಿರುಗಿ FALSE;

    p = ಸಾಲು = ml_get_curline();
    ಆಗ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ಆದರೆ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ಆದರೆ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ಪೂರ್ಣಾಂಕ)(p - ಸಾಲು);
		curwin->w_cursor.ಲಂಬಸಾಲು = i;
		trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen);
		ಆದರೆ (trypos != ಖಾಲಿ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ಆದರೆ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ಆದರೆ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ಹಿಂತಿರುಗಿ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		ಸಾಲು = ml_get_curline();
		p = ಸಾಲು + i;
	    }
	}
	ಆದರೆ (*p != NUL)
	    ++p;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_isbreak(char_u *p)
{
    ಹಿಂತಿರುಗಿ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
}

/*
 * Find the position of a C++ base-class declaration or
 * constructor-initialization. eg:
 *
 * class MyClass :
 *	baseClass		<-- here
 * class MyClass : public baseClass,
 *	anotherBaseClass	<-- here (should probably lineup ??)
 * MyClass::MyClass(...) :
 *	baseClass(...)		<-- here (constructor-initialization)
 *
 * This is a lot of guessing.  Watch out for "cond ? func() : foo".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		class_or_struct, lookfor_ctor_init, cpp_ತಳ_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*ಸಾಲು = ml_get_curline();

    ಆದರೆ (pos->lnum <= lnum)
	ಹಿಂತಿರುಗಿ cached->ಸಿಕ್ಕಿತು;	// Use the cached result

    pos->ಲಂಬಸಾಲು = 0;

    s = skipwhite(ಸಾಲು);
    ಆದರೆ (*s == '#')		// skip #define FOO x ? (x) : x
	ಹಿಂತಿರುಗಿ FALSE;
    s = cin_skipcomment(s);
    ಆದರೆ (*s == NUL)
	ಹಿಂತಿರುಗಿ FALSE;

    cpp_ತಳ_class = lookfor_ctor_init = class_or_struct = FALSE;

    // Search for a line starting with '#', empty, ending in ';' or containing
    // '{' or '}' and start below it.  This handles the following situations:
    //	a = cond ?
    //	      func() :
    //		   asdf;
    //	func::foo()
    //	      : something
    //	{}
    //	Foo::Foo (int one, int two)
    //		: something(4),
    //		somethingelse(3)
    //	{}
    ಆಗ (lnum > 1)
    {
	ಸಾಲು = ml_get(lnum - 1);
	s = skipwhite(ಸಾಲು);
	ಆದರೆ (*s == '#' || *s == NUL)
	    ನಿಲ್ಲಿಸು;
	ಆಗ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ಆದರೆ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*s != NUL)
		++s;
	}
	ಆದರೆ (*s != NUL)
	    ನಿಲ್ಲಿಸು;
	--lnum;
    }

    pos->lnum = lnum;
    ಸಾಲು = ml_get(lnum);
    s = ಸಾಲು;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (*s == NUL)
	{
	    ಆದರೆ (lnum == curwin->w_cursor.lnum)
		ನಿಲ್ಲಿಸು;
	    // Continue in the cursor line.
	    ಸಾಲು = ml_get(++lnum);
	    s = ಸಾಲು;
	}
	ಆದರೆ (s == ಸಾಲು)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ಆದರೆ (cin_iscase(s, FALSE))
		ನಿಲ್ಲಿಸು;
	    s = cin_skipcomment(ಸಾಲು);
	    ಆದರೆ (*s == NUL)
		ಮುಂದುವರಿ;
	}

	ಆದರೆ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == ':')
	{
	    ಆದರೆ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_ತಳ_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->ಲಂಬಸಾಲು = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		s = cin_skipcomment(s + 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ಆದರೆ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ಇಲ್ಲದಿದ್ದರೆ
		s = cin_skipcomment(s + 6);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_ತಳ_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos->ಲಂಬಸಾಲು == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ಆದರೆ (cpp_ತಳ_class)
		    pos->ಲಂಬಸಾಲು = (colnr_T)(s - ಸಾಲು);
	    }

	    // When the line ends in a comma don't align with it.
	    ಆದರೆ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->ಲಂಬಸಾಲು = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->ಸಿಕ್ಕಿತು = cpp_ತಳ_class;
    ಆದರೆ (cpp_ತಳ_class)
	pos->lnum = lnum;
    ಹಿಂತಿರುಗಿ cpp_ತಳ_class;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_baseclass_amount(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ಆದರೆ (ಲಂಬಸಾಲು == 0)
    {
	amount = get_indent();
	ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(ml_get_curline(), '(', ')')
		&& (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen)) != ಖಾಲಿ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ಆದರೆ (!cin_ತುದಿಗಳು_ಒಳ(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	getvcol(curwin, &curwin->w_cursor, &vcol, ಖಾಲಿ, ಖಾಲಿ, 0);
	amount = (ಪೂರ್ಣಾಂಕ)vcol;
    }
    ಆದರೆ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ಹಿಂತಿರುಗಿ amount;
}

/*
 * Find the '{' at the start of the block we are in.
 * Return NULL if no match found.
 * Ignore a '{' that is in a comment, makes indenting the next three lines
 * work.
 */
// foo()
// {
// }

    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace(ಶೂನ್ಯ)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ಸ್ಥಿರ pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ಆಗ ((trypos = findmatchlimit(ಖಾಲಿ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ಖಾಲಿ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ಖಾಲಿ;
	// ignore the { if it's in a // or / *  * / comment
	ಆದರೆ ((colnr_T)cin_skip2pos(trypos) == trypos->ಲಂಬಸಾಲು
		       && (pos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ)) == ಖಾಲಿ) // XXX
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (pos != ಖಾಲಿ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ಹಿಂತಿರುಗಿ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ಸ್ಥಿರ pos_T *
ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren_after_brace (ಪೂರ್ಣಾಂಕ ind_maxparen)	    // XXX
{
    pos_T	*trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(ind_maxparen);

    ಆದರೆ (trypos == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    pos_T	*tryposBrace = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ಆದರೆ (tryposBrace != ಖಾಲಿ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->ಲಂಬಸಾಲು < tryposBrace->ಲಂಬಸಾಲು))
	trypos = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
corr_ind_maxparen(pos_T *startpos)
{
    ದೀರ್ಘ	n = (ದೀರ್ಘ)startpos->lnum - (ದೀರ್ಘ)curwin->w_cursor.lnum;

    ಆದರೆ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ಹಿಂತಿರುಗಿ curbuf->b_ind_maxparen - (ಪೂರ್ಣಾಂಕ)n;
    ಹಿಂತಿರುಗಿ curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    ಶೂನ್ಯ
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    ಪೂರ್ಣಾಂಕ		divider;
    ಪೂರ್ಣಾಂಕ		fraction = 0;
    ಪೂರ್ಣಾಂಕ		sw;
    ದೀರ್ಘ	t = get_sw_ಮೌಲ್ಯ(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ಆದರೆ (t > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / 2)
	sw = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ / 2;
    ಇಲ್ಲದಿದ್ದರೆ
	sw = (ಪೂರ್ಣಾಂಕ)t;

    // Set the default values.

    // Spaces from a block's opening brace the prevailing indent for that
    // block should be.
    buf->b_ind_level = sw;

    // Spaces from the edge of the line an open brace that's at the end of a
    // line is imagined to be.
    buf->b_ind_open_imag = 0;

    // Spaces from the prevailing indent for a line that is not preceded by
    // an opening brace.
    buf->b_ind_no_brace = 0;

    // Column where the first { of a function should be located }.
    buf->b_ind_ಮೊದಲ_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_ಎಡ_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_ಸಂಕೇತ = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_ಸಂಕೇತ = sw;

    // Amount K&R-style parameters should be indented.
    buf->b_ind_param = sw;

    // Amount a function type spec should be indented.
    buf->b_ind_func_type = sw;

    // Amount a cpp base class declaration or constructor initialization
    // should be indented.
    buf->b_ind_cpp_baseclass = sw;

    // additional spaces beyond the prevailing indent a continuation line
    // should be located.
    buf->b_ind_continuation = sw;

    // Spaces from the indent of the line with an unclosed parenthesis.
    buf->b_ind_unclosed = sw * 2;

    // Spaces from the indent of the line with an unclosed parenthesis, which
    // itself is also unclosed.
    buf->b_ind_unclosed2 = sw;

    // Suppress ignoring spaces from the indent of a line starting with an
    // unclosed parenthesis.
    buf->b_ind_unclosed_noignore = 0;

    // If the opening paren is the last nonwhite character on the line, and
    // b_ind_unclosed_wrapped is nonzero, use this indent relative to the outer
    // context (for very long lines).
    buf->b_ind_unclosed_wrapped = 0;

    // Suppress ignoring white space when lining up with the character after
    // an unclosed parenthesis.
    buf->b_ind_unclosed_whiteok = 0;

    // Indent a closing parenthesis under the line start of the matching
    // opening parenthesis.
    buf->b_ind_matching_paren = 0;

    // Indent a closing parenthesis under the previous line.
    buf->b_ind_paren_prev = 0;

    // Extra indent for comments.
    buf->b_ind_comment = 0;

    // Spaces from the comment opener when there is nothing after it.
    buf->b_ind_ಒಳ_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_ಒಳ_comment2 = 0;

    // Max lines to search for an open paren.
    buf->b_ind_maxparen = 20;

    // Max lines to search for an open comment.
    buf->b_ind_maxcomment = 70;

    // Handle braces for java code.
    buf->b_ind_java = 0;

    // Not to confuse JS object properties with labels.
    buf->b_ind_js = 0;

    // Handle blocked cases correctly.
    buf->b_ind_keep_case_label = 0;

    // Handle C++ namespace.
    buf->b_ind_cpp_namespace = 0;

    // Handle continuation lines containing conditions of if (), for () and
    // while ().
    buf->b_ind_if_for_while = 0;

    // indentation for # comments
    buf->b_ind_hash_comment = 0;

    // Handle C++ extern "C" or "C++"
    buf->b_ind_cpp_extern_c = 0;

    // Handle C #pragma directives
    buf->b_ind_pragma = 0;

    ಗಾಗಿ (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ಆದರೆ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ಆದರೆ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((ಅಕ್ಷರ *)++p);
	    ಆಗ (VIM_ISDIGIT(*p))
	    {
		++p;
		ಆದರೆ (divider)
		    divider *= 10;
		ಇಲ್ಲದಿದ್ದರೆ
		    divider = 10;
	    }
	}
	ಆದರೆ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ಆದರೆ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		n *= sw;
		ಆದರೆ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ಆದರೆ (l[1] == '-')
	    n = -n;

	n = trim_ಗಮ್ಯ_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	ಆಯ್ಕೆ (*l)
	{
	    ಸ್ಥಿತಿ '>': buf->b_ind_level = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'e': buf->b_ind_open_imag = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'n': buf->b_ind_no_brace = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'f': buf->b_ind_ಮೊದಲ_open = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '{': buf->b_ind_open_extra = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '}': buf->b_ind_close_extra = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '^': buf->b_ind_open_ಎಡ_imag = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'L': buf->b_ind_jump_label = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ':': buf->b_ind_case = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '=': buf->b_ind_case_ಸಂಕೇತ = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'b': buf->b_ind_case_break = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'p': buf->b_ind_param = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 't': buf->b_ind_func_type = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '/': buf->b_ind_comment = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'c': buf->b_ind_ಒಳ_comment = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'C': buf->b_ind_ಒಳ_comment2 = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'i': buf->b_ind_cpp_baseclass = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '+': buf->b_ind_continuation = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '(': buf->b_ind_unclosed = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'u': buf->b_ind_unclosed2 = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'U': buf->b_ind_unclosed_noignore = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'W': buf->b_ind_unclosed_wrapped = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'w': buf->b_ind_unclosed_whiteok = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'm': buf->b_ind_matching_paren = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'M': buf->b_ind_paren_prev = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ')': buf->b_ind_maxparen = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '*': buf->b_ind_maxcomment = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'g': buf->b_ind_scopedecl = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'h': buf->b_ind_scopedecl_ಸಂಕೇತ = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'j': buf->b_ind_java = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'J': buf->b_ind_js = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'l': buf->b_ind_keep_case_label = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '#': buf->b_ind_hash_comment = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'N': buf->b_ind_cpp_namespace = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'k': buf->b_ind_if_for_while = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'E': buf->b_ind_cpp_extern_c = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'P': buf->b_ind_pragma = (ಪೂರ್ಣಾಂಕ)n; ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*p == ',')
	    ++p;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ(ಪೂರ್ಣಾಂಕ lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ಪೂರ್ಣಾಂಕ		elselevel;
    ಪೂರ್ಣಾಂಕ		whilelevel;

    ಆದರೆ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಆಗ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;

	look = cin_skipcomment(ml_get_curline());
	ಆದರೆ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace();  // XXX
	    ಆದರೆ (theirscope == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ಆದರೆ (theirscope->lnum < ourscope)
		ನಿಲ್ಲಿಸು;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ಆದರೆ (theirscope->lnum > ourscope)
		ಮುಂದುವರಿ;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ಆದರೆ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ಆದರೆ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ಆದರೆ (!cin_isif(mightbeif))
			++elselevel;
		    ಮುಂದುವರಿ;
		}

		// If it's an "if" decrement elselevel
		ಆದರೆ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ಆದರೆ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ಆದರೆ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ಮುಂದುವರಿ;
	    }

	    // If it's a "do" decrement whilelevel
	    ಆದರೆ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ಆದರೆ (elselevel <= 0 && whilelevel <= 0)
		ಹಿಂತಿರುಗಿ OK;
	}
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ಪೂರ್ಣಾಂಕ
get_c_indent(ಶೂನ್ಯ)
{
    pos_T	cur_curpos;
    ಪೂರ್ಣಾಂಕ		amount;
    ಪೂರ್ಣಾಂಕ		scope_amount;
    ಪೂರ್ಣಾಂಕ		cur_amount = MAXCOL;
    colnr_T	ಲಂಬಸಾಲು;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ಖಾಲಿ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_brace;
#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_ಒಳ_COL0		1	    // '{' is in column 0
#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_AT_ಆರಂಭ		2	    // '{' is at start of line
#ವ್ಯಾಖ್ಯಾನಿಸು BRACE_AT_ಕೊನೆ		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ಪೂರ್ಣಾಂಕ		lookfor;
    ಪೂರ್ಣಾಂಕ		whilelevel;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		iscase;
    ಪೂರ್ಣಾಂಕ		lookfor_break;
    ಪೂರ್ಣಾಂಕ		lookfor_cpp_namespace = FALSE;
    ಪೂರ್ಣಾಂಕ		cont_amount = 0;    // amount for continuation line
    ಪೂರ್ಣಾಂಕ		original_ಸಾಲು_islabel;
    ಪೂರ್ಣಾಂಕ		added_ಗಮ್ಯ_amount = 0;
    ಪೂರ್ಣಾಂಕ		js_cur_has_ಕೀಲಿ = 0;
    linenr_T	raw_string_ಆರಂಭ = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ಪೂರ್ಣಾಂಕ		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ಆದರೆ (cur_curpos.lnum == 1)
	ಹಿಂತಿರುಗಿ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ಆದರೆ (linecopy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು)
	    && curwin->w_cursor.ಲಂಬಸಾಲು < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.ಲಂಬಸಾಲು] == ')')
	linecopy[curwin->w_cursor.ಲಂಬಸಾಲು] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    original_ಸಾಲು_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_comment();
    ಆದರೆ (comment_pos != ಖಾಲಿ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_rawstring(curbuf->b_ind_maxcomment);
    ಆದರೆ (trypos != ಖಾಲಿ && (comment_pos == ಖಾಲಿ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	ಹೋಗು laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ಆದರೆ (*theline == '#' && (*linecopy == '#' || ಒಳ_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ಆದರೆ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    ಹೋಗು theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ಆದರೆ (original_ಸಾಲು_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	ಹೋಗು theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ಆದರೆ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = ಕಂಡುಹಿಡಿ_ಸಾಲು_comment(); // XXX
	ಆದರೆ (trypos == ಖಾಲಿ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.ಲಂಬಸಾಲು =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ಆದರೆ (linecomment_pos.ಲಂಬಸಾಲು != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ಆದರೆ (trypos  != ಖಾಲಿ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
	    amount = ಲಂಬಸಾಲು;
	    ಹೋಗು theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ಆದರೆ (!cin_iscomment(theline) && comment_pos != ಖಾಲಿ) // XXX
    {
	ಪೂರ್ಣಾಂಕ	lead_ಆರಂಭ_len = 2;
	ಪೂರ್ಣಾಂಕ	lead_ಮಧ್ಯ_len = 1;
	char_u	lead_ಆರಂಭ[COM_MAX_LEN];	// start-comment string
	char_u	lead_ಮಧ್ಯ[COM_MAX_LEN];	// middle-comment string
	char_u	lead_ಕೊನೆ[COM_MAX_LEN];		// end-comment string
	ಪೂರ್ಣಾಂಕ	lead_ಕೊನೆ_len;
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	ಆರಂಭ_align = 0;
	ಪೂರ್ಣಾಂಕ	ಆರಂಭ_off = 0;
	ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
	amount = ಲಂಬಸಾಲು;
	*lead_ಆರಂಭ = NUL;
	*lead_ಮಧ್ಯ = NUL;

	p = curbuf->b_p_com;
	ಆಗ (*p != NUL)
	{
	    ಪೂರ್ಣಾಂಕ	align = 0;
	    ಪೂರ್ಣಾಂಕ	off = 0;
	    ಪೂರ್ಣಾಂಕ what = 0;

	    ಆಗ (*p != NUL && *p != ':')
	    {
		ಆದರೆ (*p == COM_ಆರಂಭ || *p == COM_ಕೊನೆ || *p == COM_ಮಧ್ಯ)
		    what = *p++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == COM_ಎಡ || *p == COM_ಬಲ)
		    align = *p++;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ಇಲ್ಲದಿದ್ದರೆ
		    ++p;
	    }

	    ಆದರೆ (*p == ':')
		++p;
	    lead_ಕೊನೆ_len = copy_option_part(&p, lead_ಕೊನೆ, COM_MAX_LEN, ",");
	    ಆದರೆ (what == COM_ಆರಂಭ)
	    {
		STRCPY(lead_ಆರಂಭ, lead_ಕೊನೆ);
		lead_ಆರಂಭ_len = lead_ಕೊನೆ_len;
		ಆರಂಭ_off = off;
		ಆರಂಭ_align = align;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == COM_ಮಧ್ಯ)
	    {
		STRCPY(lead_ಮಧ್ಯ, lead_ಕೊನೆ);
		lead_ಮಧ್ಯ_len = lead_ಕೊನೆ_len;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == COM_ಕೊನೆ)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ಆದರೆ (STRNCMP(theline, lead_ಮಧ್ಯ, lead_ಮಧ್ಯ_len) == 0
			&& STRNCMP(theline, lead_ಕೊನೆ, lead_ಕೊನೆ_len) != 0)
		{
		    ಮುಗಿದ = TRUE;
		    ಆದರೆ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ಆದರೆ (STRNCMP(look, lead_ಆರಂಭ, lead_ಆರಂಭ_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(look, lead_ಮಧ್ಯ,
							lead_ಮಧ್ಯ_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ನಿಲ್ಲಿಸು;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->ಲಂಬಸಾಲು,
					     lead_ಆರಂಭ, lead_ಆರಂಭ_len) != 0)
			    ಮುಂದುವರಿ;
		    }
		    ಆದರೆ (ಆರಂಭ_off != 0)
			amount += ಆರಂಭ_off;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_align == COM_ಬಲ)
			amount += vim_strsize(lead_ಆರಂಭ)
						   - vim_strsize(lead_ಮಧ್ಯ);
		    ನಿಲ್ಲಿಸು;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ಆದರೆ (STRNCMP(theline, lead_ಮಧ್ಯ, lead_ಮಧ್ಯ_len) != 0
			&& STRNCMP(theline, lead_ಕೊನೆ, lead_ಕೊನೆ_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ಆದರೆ (off != 0)
			amount += off;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (align == COM_ಬಲ)
			amount += vim_strsize(lead_ಆರಂಭ)
						   - vim_strsize(lead_ಮಧ್ಯ);
		    ಮುಗಿದ = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ಆದರೆ (ಮುಗಿದ)
	    ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (theline[0] == '*')
	    amount += 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    ಗಾಗಿ (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ಆದರೆ (linewhite(lnum))		    // skip blank lines
		    ಮುಂದುವರಿ;
		amount = get_indent_lnum(lnum);	    // XXX
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (amount == -1)			    // use the comment opener
	    {
		ಆದರೆ (!curbuf->b_ind_ಒಳ_comment2)
		{
		    ಆರಂಭ = ml_get(comment_pos->lnum);
		    look = ಆರಂಭ + comment_pos->ಲಂಬಸಾಲು + 2; // skip / and *
		    ಆದರೆ (*look != NUL)		    // if something after it
			comment_pos->ಲಂಬಸಾಲು = (colnr_T)(skipwhite(look) - ಆರಂಭ);
		}
		getvcol(curwin, comment_pos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
		amount = ಲಂಬಸಾಲು;
		ಆದರೆ (curbuf->b_ind_ಒಳ_comment2 || *look == NUL)
		    amount += curbuf->b_ind_ಒಳ_comment;
	    }
	}
	ಹೋಗು theend;
    }

    // Are we looking at a ']' that has a match?
    ಆದರೆ (*skipwhite(theline) == ']'
	    && (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_char('[', curbuf->b_ind_maxparen)) != ಖಾಲಿ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	ಹೋಗು theend;
    }

    // Are we inside parentheses or braces?  XXX
    ಆದರೆ (((trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen)) != ಖಾಲಿ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) != ಖಾಲಿ
	    || trypos != ಖಾಲಿ)
    {
      ಆದರೆ (trypos != ಖಾಲಿ && tryposBrace != ಖಾಲಿ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ಆದರೆ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->ಲಂಬಸಾಲು < tryposBrace->ಲಂಬಸಾಲು)
	      trypos = ಖಾಲಿ;
	  ಇಲ್ಲದಿದ್ದರೆ
	      tryposBrace = ಖಾಲಿ;
      }

      ಆದರೆ (trypos != ಖಾಲಿ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ಆದರೆ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    ಗಾಗಿ (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ಆದರೆ (cin_nocode(l))		// skip comment lines
		    ಮುಂದುವರಿ;
		ಆದರೆ (cin_ispreproc_cont(&l, &lnum, &amount))
		    ಮುಂದುವರಿ;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ಆದರೆ ((trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ)) != ಖಾಲಿ)
		{
		    lnum = trypos->lnum + 1;
		    ಮುಂದುವರಿ;
		}

		// XXX
		ಆದರೆ ((trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(
			corr_ind_maxparen(&cur_curpos))) != ಖಾಲಿ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->ಲಂಬಸಾಲು == our_paren_pos.ಲಂಬಸಾಲು)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ಆದರೆ (theline[0] == ')')
		    {
			ಆದರೆ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ಆದರೆ (amount == -1)
	{
	    ಪೂರ್ಣಾಂಕ	    ignore_paren_ಲಂಬಸಾಲು = 0;
	    ಪೂರ್ಣಾಂಕ	    is_if_for_while = 0;

	    ಆದರೆ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *ಸಾಲು;

		trypos = &our_paren_pos;
		ಮಾಡು
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.ಲಂಬಸಾಲು = outermost.ಲಂಬಸಾಲು;

		    trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen);
		} ಆಗ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		ಸಾಲು = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_ಸ್ಥಾನಾಂತರ(ಸಾಲು, &outermost.ಲಂಬಸಾಲು);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ಆದರೆ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *ಸಾಲು;
		ಪೂರ್ಣಾಂಕ	    look_ಲಂಬಸಾಲು;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		ಸಾಲು = ml_get_curline();
		look_ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)(look - ಸಾಲು);
		curwin->w_cursor.ಲಂಬಸಾಲು = look_ಲಂಬಸಾಲು + 1;
		ಆದರೆ ((trypos = findmatchlimit(ಖಾಲಿ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ಖಾಲಿ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->ಲಂಬಸಾಲು < our_paren_pos.ಲಂಬಸಾಲು)
		    ignore_paren_ಲಂಬಸಾಲು = trypos->ಲಂಬಸಾಲು + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_ಲಂಬಸಾಲು;
	    }
	    ಆದರೆ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_ಲಂಬಸಾಲು == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		ಆದರೆ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ಆದರೆ (curbuf->b_ind_unclosed_wrapped
				       && cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < our_paren_pos.ಲಂಬಸಾಲು; ++ಲಂಬಸಾಲು)
			{
			    ಆಯ್ಕೆ (l[ಲಂಬಸಾಲು])
			    {
				ಸ್ಥಿತಿ '(':
				ಸ್ಥಿತಿ '{': ++n;
					  ನಿಲ್ಲಿಸು;

				ಸ್ಥಿತಿ ')':
				ಸ್ಥಿತಿ '}': ಆದರೆ (n > 1)
					      --n;
					  ನಿಲ್ಲಿಸು;
			    }
			}

			our_paren_pos.ಲಂಬಸಾಲು = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.ಲಂಬಸಾಲು++;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಲಂಬಸಾಲು = our_paren_pos.ಲಂಬಸಾಲು + 1;
			ಆಗ (VIM_ISWHITE(l[ಲಂಬಸಾಲು]))
			    ಲಂಬಸಾಲು++;
			ಆದರೆ (l[ಲಂಬಸಾಲು] != NUL)	// In case of trailing space
			    our_paren_pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
			ಇಲ್ಲದಿದ್ದರೆ
			    our_paren_pos.ಲಂಬಸಾಲು++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ಆದರೆ (our_paren_pos.ಲಂಬಸಾಲು > 0)
		{
		    getvcol(curwin, &our_paren_pos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
		    ಆದರೆ (cur_amount > (ಪೂರ್ಣಾಂಕ)ಲಂಬಸಾಲು)
			cur_amount = ಲಂಬಸಾಲು;
		}
	    }

	    ಆದರೆ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_ಲಂಬಸಾಲು == 0))
	    {
		ಆದರೆ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		ಲಂಬಸಾಲು = our_paren_pos.ಲಂಬಸಾಲು;
		ಆಗ ((ಪೂರ್ಣಾಂಕ)our_paren_pos.ಲಂಬಸಾಲು > ignore_paren_ಲಂಬಸಾಲು)
		{
		    --our_paren_pos.ಲಂಬಸಾಲು;
		    ಆಯ್ಕೆ (*ml_get_pos(&our_paren_pos))
		    {
			ಸ್ಥಿತಿ '(': amount += curbuf->b_ind_unclosed2;
				  ಲಂಬಸಾಲು = our_paren_pos.ಲಂಬಸಾಲು;
				  ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ')': amount -= curbuf->b_ind_unclosed2;
				  ಲಂಬಸಾಲು = MAXCOL;
				  ನಿಲ್ಲಿಸು;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ಆದರೆ (ಲಂಬಸಾಲು == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
		    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren_after_brace(curbuf->b_ind_maxparen)
								      != ಖಾಲಿ)
			amount += curbuf->b_ind_unclosed2;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ಆದರೆ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ಆದರೆ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      ಇಲ್ಲದಿದ್ದರೆ
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	ಆರಂಭ = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(ಆರಂಭ);
	ಆದರೆ (*look == '{')
	{
	    getvcol(curwin, trypos, &ಲಂಬಸಾಲು, ಖಾಲಿ, ಖಾಲಿ, 0);
	    amount = ಲಂಬಸಾಲು;
	    ಆದರೆ (*ಆರಂಭ == '{')
		ಆರಂಭ_brace = BRACE_ಒಳ_COL0;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆರಂಭ_brace = BRACE_AT_ಆರಂಭ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(ಆರಂಭ, '(', ')')
			&& (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen))
								      != ಖಾಲಿ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ಆದರೆ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ಇಲ್ಲದಿದ್ದರೆ
		amount = skip_label(lnum, &l);

	    ಆರಂಭ_brace = BRACE_AT_ಕೊನೆ;
	}

	// For Javascript check if the line starts with "key:".
	ಆದರೆ (curbuf->b_ind_js)
	    js_cur_has_ಕೀಲಿ = cin_has_js_ಕೀಲಿ(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ಆದರೆ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ಆದರೆ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ಆದರೆ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ಆದರೆ (ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    ಹೋಗು theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ಆದರೆ (ಆರಂಭ_brace == BRACE_ಒಳ_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ಎಡ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_brace == BRACE_AT_ಆರಂಭ &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಆರಂಭ_brace == BRACE_AT_ಕೊನೆ)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ಆದರೆ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ಆದರೆ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ಆದರೆ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (curbuf->b_ind_case_break && cin_isbreak(theline))
		    // break; ...
		    lookfor_break = TRUE;

		lookfor = LOOKFOR_INITIAL;
		// b_ind_level from start of block
		amount += curbuf->b_ind_level;
	    }
	    scope_amount = amount;
	    whilelevel = 0;

	    // Search backwards.  If we find something we recognize, line up
	    // with that.
	    //
	    // If we're looking at an open brace, indent
	    // the usual amount relative to the conditional
	    // that opens the block.
	    curwin->w_cursor = cur_curpos;
	    ಗಾಗಿ (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ಆದರೆ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ಆದರೆ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಆದರೆ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ಆದರೆ (cont_amount > 0)
				amount = cont_amount;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ನಿಲ್ಲಿಸು;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ);
			ಆದರೆ (trypos != ಖಾಲಿ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
			    ಮುಂದುವರಿ;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ಆದರೆ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ಮುಂದುವರಿ;

			ಆದರೆ (cin_nocode(l))
			    ಮುಂದುವರಿ;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ಆದರೆ (ಆರಂಭ_brace != BRACE_ಒಳ_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ಆದರೆ (terminated == ',')
				ನಿಲ್ಲಿಸು;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ಆದರೆ (terminated != ';' && cin_isinit())
				ನಿಲ್ಲಿಸು;

			    // nothing useful found
			    ಆದರೆ (terminated == 0 || terminated == '{')
				ಮುಂದುವರಿ;
			}

			ಆದರೆ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ಖಾಲಿ;
			    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '(', ')'))
				trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(
						      curbuf->b_ind_maxparen);

			    ಆದರೆ (trypos == ಖಾಲಿ && ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '{', '}'))
				trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace();

			    ಆದರೆ (trypos != ಖಾಲಿ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ಲಂಬಸಾಲು = 0;
				ಮುಂದುವರಿ;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lookfor == LOOKFOR_UNTERM)
		    {
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (lookfor != LOOKFOR_ಪದ
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ಆದರೆ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_ಗಮ್ಯ_amount = curbuf->b_ind_open_extra;
			    }
			}

			ಆದರೆ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ಆದರೆ (curwin->w_cursor.lnum == ourscope)
				ಮುಂದುವರಿ;

			    ಆದರೆ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - ಕಂಡುಹಿಡಿ_NAMESPACE_LIM)
				ನಿಲ್ಲಿಸು;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ);
			    ಆದರೆ (trypos != ಖಾಲಿ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ಲಂಬಸಾಲು = 0;
				ಮುಂದುವರಿ;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ಆದರೆ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ಮುಂದುವರಿ;

			    // Finally the actual check for "namespace".
			    ಆದರೆ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_ಗಮ್ಯ_amount;
				ನಿಲ್ಲಿಸು;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_ಗಮ್ಯ_amount;
				ನಿಲ್ಲಿಸು;
			    }

			    ಆದರೆ (cin_nocode(l))
				ಮುಂದುವರಿ;
			}
		    }
		    ನಿಲ್ಲಿಸು;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ಆದರೆ ((trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(&raw_string_ಆರಂಭ)) != ಖಾಲಿ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		    ಮುಂದುವರಿ;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ಆದರೆ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ಆದರೆ (lookfor == LOOKFOR_CPP_BASECLASS)
			ನಿಲ್ಲಿಸು;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ಆದರೆ (whilelevel > 0)
			ಮುಂದುವರಿ;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ಆದರೆ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
			ನಿಲ್ಲಿಸು;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ಆದರೆ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ಆದರೆ ((trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) == ಖಾಲಿ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ನಿಲ್ಲಿಸು;
			}
			ಮುಂದುವರಿ;
		    }

		    n = get_indent_nolabel(curwin->w_cursor.lnum);  // XXX

		    //	 case xx: if (cond)	    <- line up with this if
		    //		      y = y + 1;
		    // ->	  s = 99;
		    //
		    //	 case xx:
		    //	     if (cond)		<- line up with this line
		    //		 y = y + 1;
		    // ->    s = 99;
		    ಆದರೆ (lookfor == LOOKFOR_ಪದ)
		    {
			ಆದರೆ (n)
			    amount = n;

			ಆದರೆ (!lookfor_break)
			    ನಿಲ್ಲಿಸು;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ಆದರೆ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ಆದರೆ (l != ಖಾಲಿ && cin_is_cinword(l))
			{
			    ಆದರೆ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ಇಲ್ಲದಿದ್ದರೆ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ನಿಲ್ಲಿಸು;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_ಸಂಕೇತ
					: curbuf->b_ind_scopedecl_ಸಂಕೇತ);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    ಮುಂದುವರಿ;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ಆದರೆ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '{', '}')
				     && (trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) != ಖಾಲಿ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.ಲಂಬಸಾಲು = 0;
		    }
		    ಮುಂದುವರಿ;
		}

		// Ignore jump labels with nothing after them.
		ಆದರೆ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ಆದರೆ (l == ಖಾಲಿ || cin_nocode(l))
			ಮುಂದುವರಿ;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ಆದರೆ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ಮುಂದುವರಿ;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ಆದರೆ (lookfor != LOOKFOR_ಪದ && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ಆದರೆ (n)
		{
		    ಆದರೆ (lookfor == LOOKFOR_UNTERM)
		    {
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ಮುಂದುವರಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.ಲಂಬಸಾಲು);
		    ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ಆದರೆ (cin_isterminated(l, TRUE, FALSE))
			ನಿಲ್ಲಿಸು;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಮುಂದುವರಿ;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//ಇಲ್ಲಿ
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		ಆದರೆ (js_cur_has_ಕೀಲಿ)
		{
		    js_cur_has_ಕೀಲಿ = 0; // only check the first line
		    ಆದರೆ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_ಕೀಲಿ;
		    }
		}
		ಆದರೆ (lookfor == LOOKFOR_JS_ಕೀಲಿ && cin_has_js_ಕೀಲಿ(l))
		{
		    amount = get_indent();
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (lookfor == LOOKFOR_COMMA)
		{
		    ಆದರೆ (tryposBrace != ಖಾಲಿ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ನಿಲ್ಲಿಸು;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			amount = get_indent();
			ಆದರೆ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ನಿಲ್ಲಿಸು;
		    }
		}

		ಆದರೆ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ಆದರೆ (lookfor != LOOKFOR_ENUM_OR_INIT &&
			    (*skipwhite(l) == '[' || l[STRLEN(l) - 1] == '['))
			amount += ind_continuation;
		    // if we're in the middle of a paren thing,
		    // go back to the line that starts it so
		    // we can get the right prevailing indent
		    //	   if ( foo &&
		    //		    bar )

		    // Position the cursor over the rightmost paren, so that
		    // matching it will take us back to the start of the line.
		    // Ignore a match before the start of the block.
		    (ಶೂನ್ಯ)ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '(', ')');
		    trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(corr_ind_maxparen(&cur_curpos));
		    ಆದರೆ (trypos != ಖಾಲಿ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->ಲಂಬಸಾಲು < tryposBrace->ಲಂಬಸಾಲು)))
			trypos = ಖಾಲಿ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ಆದರೆ (trypos == ಖಾಲಿ && terminated == ',')
		    {
			ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '{', '}'))
			    trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace();
			l = ml_get_curline();
		    }

		    ಆದರೆ (trypos != ಖಾಲಿ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ಆದರೆ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
			    ಮುಂದುವರಿ;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ಆದರೆ (terminated == ',')
		    {
			ಆಗ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ಆದರೆ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ನಿಲ್ಲಿಸು;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ಆದರೆ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ಇಲ್ಲದಿದ್ದರೆ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ಆದರೆ (terminated != ',' && lookfor != LOOKFOR_ಪದ
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ಆದರೆ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ಆದರೆ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ಮುಂದುವರಿ;
			}
			ನಿಲ್ಲಿಸು;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ಆದರೆ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ಆದರೆ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ಆದರೆ (cont_amount > 0)
				amount = cont_amount;
			    ಇಲ್ಲದಿದ್ದರೆ
				amount += ind_continuation;
			    ನಿಲ್ಲಿಸು;
			}

			// If this is just above the line we are indenting, we
			// are finished.
			//	    while (not)
			// ->		here;
			// Otherwise this indent can be used when the line
			// before this is terminated.
			//	yyy;
			//	if (stat)
			//	    while (not)
			//		xxx;
			// ->	here;
			amount = cur_amount;
			ಆದರೆ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ಆದರೆ (lookfor != LOOKFOR_ಪದ)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ನಿಲ್ಲಿಸು;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ಆದರೆ (cin_isdo(l))
			{
			    ಆದರೆ (whilelevel == 0)
				ನಿಲ್ಲಿಸು;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ಆದರೆ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ಆದರೆ (*l == '}')
				curwin->w_cursor.ಲಂಬಸಾಲು =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ಆದರೆ ((trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) == ಖಾಲಿ
				       || ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ನಿಲ್ಲಿಸು;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ಆದರೆ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ಆದರೆ (terminated == ',')
				amount += ind_continuation;
			    ನಿಲ್ಲಿಸು;
			}

			ಆದರೆ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ಆದರೆ (terminated == ',')
			    {
				ಆದರೆ (curbuf->b_ind_cpp_baseclass == 0)
				    ನಿಲ್ಲಿಸು;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ಮುಂದುವರಿ;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ಆದರೆ (amount > cur_amount)
				amount = cur_amount;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ಪೂರ್ಣಾಂಕ)STRLEN(l);
			    ಆದರೆ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ನಿಲ್ಲಿಸು;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ಆದರೆ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ಆದರೆ (curbuf->b_ind_js)
				{
				    // Search for a line ending in a comma
				    // and line up with the line below it
				    // (could be the current line).
				    // some = [
				    //     1,     <- line up here
				    //     2,
				    // some = [
				    //     3 +    <- line up here
				    //       4 *
				    //        5,
				    //     6,
				    ಆದರೆ (cin_iscomment(skipwhite(l)))
					ನಿಲ್ಲಿಸು;
				    lookfor = LOOKFOR_COMMA;
				    trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_char('[',
						      curbuf->b_ind_maxparen);
				    ಆದರೆ (trypos != ಖಾಲಿ)
				    {
					ಆದರೆ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ನಿಲ್ಲಿಸು;
					}
					ourscope = trypos->lnum;
				    }
				}
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_ಮೊದಲ_id_amount();
				}
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				ಆದರೆ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ಆದರೆ (lookfor != LOOKFOR_ಪದ
						&& lookfor != LOOKFOR_JS_ಕೀಲಿ
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_ಆರಂಭ != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cin_iswhileofdo_ಕೊನೆ(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ಆದರೆ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
			ನಿಲ್ಲಿಸು;
		    }

		    ಆದರೆ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_ಪದ;
			amount = get_indent();	    // XXX
			ಆದರೆ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ಆದರೆ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ಮುಂದುವರಿ;
		    }

		    // Handle "do {" line.
		    ಆದರೆ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ಆದರೆ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ಮುಂದುವರಿ;
			}
		    }

		    // Found a terminated line above an unterminated line. Add
		    // the amount for a continuation line.
		    //	 x = 1;
		    //	 y = foo +
		    // ->	here;
		    // or
		    //	 int x = 1;
		    //	 int foo,
		    // ->	here;
		    ಆದರೆ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಆದರೆ (cont_amount > 0)
			    amount = cont_amount;
			ಇಲ್ಲದಿದ್ದರೆ
			    amount += ind_continuation;
			ನಿಲ್ಲಿಸು;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ಆದರೆ (lookfor == LOOKFOR_ಪದ)
		    {
			ಆದರೆ (!lookfor_break && whilelevel == 0)
			    ನಿಲ್ಲಿಸು;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
ಪದ_again:
			l = ml_get_curline();
			ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '(', ')')
				&& (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(
					   curbuf->b_ind_maxparen)) != ಖಾಲಿ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ಆದರೆ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.ಲಂಬಸಾಲು = 0;
				ಮುಂದುವರಿ;
			    }
			}

			// When aligning with the case statement, don't align
			// with a statement after it.
			//  case 1: {   <-- don't use this { position
			//	stat;
			//  }
			//  case 2:
			//	stat;
			// }
			iscase = (curbuf->b_ind_keep_case_label
						     && cin_iscase(l, FALSE));

			// Get indent and pointer to text for current line,
			// ignoring any jump label.
			amount = skip_label(curwin->w_cursor.lnum, &l);

			ಆದರೆ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ಆದರೆ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_ಪದ;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ಆದರೆ (lookfor == LOOKFOR_ಪದ
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ಆದರೆ ((trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) == ಖಾಲಿ
				       || ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ನಿಲ್ಲಿಸು;
			    ಮುಂದುವರಿ;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '{', '}') // XXX
				     && (trypos = ಕಂಡುಹಿಡಿ_ಆರಂಭ_brace()) != ಖಾಲಿ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ಆದರೆ (*l == '}' || !cin_iselse(l))
				ಹೋಗು ಪದ_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ಆದರೆ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ಆದರೆ (curbuf->b_ind_jump_label > 0 && original_ಸಾಲು_islabel)
	  amount -= curbuf->b_ind_jump_label;

      ಹೋಗು theend;
    }

    // ok -- we're not inside any sort of structure at all!
    //
    // This means we're at the top level, and everything should
    // basically just match where the previous line is, except
    // for the lines immediately following a function declaration,
    // which are K&R-style parameters and need to be indented.
    //
    // if our line starts with an open brace, forget about any
    // prevailing indent and make sure it looks like the start
    // of a function

    ಆದರೆ (theline[0] == '{')
    {
	amount = curbuf->b_ind_ಮೊದಲ_open;
	ಹೋಗು theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ಆದರೆ (cur_curpos.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ಖಾಲಿ
	    && vim_strchr(theline, '}') == ಖಾಲಿ
	    && !cin_ತುದಿಗಳು_ಒಳ(theline, (char_u *)":")
	    && !cin_ತುದಿಗಳು_ಒಳ(theline, (char_u *)",")
	    && cin_isfuncdecl(ಖಾಲಿ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	ಹೋಗು theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ಆಗ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ಆದರೆ ((trypos = ind_ಕಂಡುಹಿಡಿ_ಆರಂಭ_CORS(ಖಾಲಿ)) != ಖಾಲಿ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    ಮುಂದುವರಿ;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ಆದರೆ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ಆದರೆ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.ಲಂಬಸಾಲು);
	    ನಿಲ್ಲಿಸು;
	}

	// Skip preprocessor directives and blank lines.
	ಆದರೆ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ಮುಂದುವರಿ;

	ಆದರೆ (cin_nocode(l))
	    ಮುಂದುವರಿ;

	// If the previous line ends in ',', use one level of
	// indentation:
	// int foo,
	//     bar;
	// do this before checking for '}' in case of eg.
	// enum foobar
	// {
	//   ...
	// } foo,
	//   bar;
	n = 0;
	ಆದರೆ (cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '(', ')')
		    && (trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(
				     curbuf->b_ind_maxparen)) != ಖಾಲಿ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ಆಗ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ಆದರೆ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ನಿಲ್ಲಿಸು;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	    }

	    amount = get_indent();	    // XXX

	    ಆದರೆ (amount == 0)
		amount = cin_ಮೊದಲ_id_amount();
	    ಆದರೆ (amount == 0)
		amount = ind_continuation;
	    ನಿಲ್ಲಿಸು;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ಆದರೆ (cin_isfuncdecl(ಖಾಲಿ, cur_curpos.lnum, 0))  // XXX
	    ನಿಲ್ಲಿಸು;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ಆದರೆ (*skipwhite(l) == '}')
	    ನಿಲ್ಲಿಸು;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ಆದರೆ (cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)"};"))
	    ನಿಲ್ಲಿಸು;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ಆದರೆ (cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ನಿಲ್ಲಿಸು;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ಆದರೆ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ಆಗ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ಆದರೆ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (curwin->w_cursor.lnum > 0
			    && cin_ತುದಿಗಳು_ಒಳ(look, (char_u *)"}"))
		ನಿಲ್ಲಿಸು;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ಆದರೆ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ನಿಲ್ಲಿಸು;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ಆದರೆ (cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ಆದರೆ (cin_ತುದಿಗಳು_ಒಳ(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ನಿಲ್ಲಿಸು;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_paren(l, '(', ')');

	ಆದರೆ ((trypos = ಕಂಡುಹಿಡಿ_ಹೊಂದಿಕೆ_paren(curbuf->b_ind_maxparen)) != ಖಾಲಿ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ನಿಲ್ಲಿಸು;
    }

    // add extra indent for a comment
    ಆದರೆ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ಆದರೆ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ಆದರೆ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ಆದರೆ (cur_amount > 0)
		amount = cur_amount;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ಆದರೆ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ಹಿಂತಿರುಗಿ amount;
}

/*
 * return TRUE if 'cinkeys' contains the key "keytyped",
 * when == '*':	    Only if key is preceded with '*'	(indent before insert)
 * when == '!':	    Only if key is preceded with '!'	(don't insert)
 * when == ' ':	    Only if key is not preceded with '*'(indent afterwards)
 *
 * "keytyped" can have a few special values:
 * KEY_OPEN_FORW
 * KEY_OPEN_BACK
 * KEY_COMPLETE	    just finished completion.
 *
 * If line_is_empty is TRUE accept keys with '0' before them.
 */
    ಪೂರ್ಣಾಂಕ
ಒಳ_cinkeys(
    ಪೂರ್ಣಾಂಕ		keytyped,
    ಪೂರ್ಣಾಂಕ		when,
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_is_ಖಾಲಿಗಳು)
{
    char_u	*look;
    ಪೂರ್ಣಾಂಕ		try_ಹೊಂದಿಕೆ;
    ಪೂರ್ಣಾಂಕ		try_ಹೊಂದಿಕೆ_ಶಬ್ದ;
    char_u	*p;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		icase;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ಆಗ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	ಆಯ್ಕೆ (when)
	{
	    ಸ್ಥಿತಿ '*': try_ಹೊಂದಿಕೆ = (*look == '*'); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '!': try_ಹೊಂದಿಕೆ = (*look == '!'); ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: try_ಹೊಂದಿಕೆ = (*look != '*'); ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ಆದರೆ (*look == '0')
	{
	    try_ಹೊಂದಿಕೆ_ಶಬ್ದ = try_ಹೊಂದಿಕೆ;
	    ಆದರೆ (!ಸಾಲು_is_ಖಾಲಿಗಳು)
		try_ಹೊಂದಿಕೆ = FALSE;
	    ++look;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    try_ಹೊಂದಿಕೆ_ಶಬ್ದ = FALSE;

	// does it look like a control character?
	ಆದರೆ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && keytyped == Ctrl_chr(look[1]))
		ಹಿಂತಿರುಗಿ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == 'o')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && keytyped == ಕೀಲಿ_OPEN_FORW)
		ಹಿಂತಿರುಗಿ TRUE;
	    ++look;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == 'O')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && keytyped == ಕೀಲಿ_OPEN_ಹಿಂಭಾಗ)
		ಹಿಂತಿರುಗಿ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == 'e')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && keytyped == 'e' && curwin->w_cursor.ಲಂಬಸಾಲು >= 4)
	    {
		p = ml_get_curline();
		ಆದರೆ (skipwhite(p) == p + curwin->w_cursor.ಲಂಬಸಾಲು - 4 &&
			STRNCMP(p + curwin->w_cursor.ಲಂಬಸಾಲು - 4, "else", 4) == 0)
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == ':')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && keytyped == ':')
	    {
		p = ml_get_curline();
		ಆದರೆ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ಹಿಂತಿರುಗಿ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > 2
			&& p[curwin->w_cursor.ಲಂಬಸಾಲು - 1] == ':'
			&& p[curwin->w_cursor.ಲಂಬಸಾಲು - 2] == ':')
		{
		    p[curwin->w_cursor.ಲಂಬಸಾಲು - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.ಲಂಬಸಾಲು - 1] = ':';
		    ಆದರೆ (i)
			ಹಿಂತಿರುಗಿ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == '<')
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ಆದರೆ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ಖಾಲಿ
						       && keytyped == look[1])
		    ಹಿಂತಿರುಗಿ TRUE;

		ಆದರೆ (keytyped == get_special_ಕೀಲಿ_ಸಂಕೇತ(look + 1))
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	    ಆಗ (*look && *look != '>')
		look++;
	    ಆಗ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ಆದರೆ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ಆದರೆ (p == ಖಾಲಿ)
		p = look + STRLEN(look);
	    ಆದರೆ ((try_ಹೊಂದಿಕೆ || try_ಹೊಂದಿಕೆ_ಶಬ್ದ)
		    && curwin->w_cursor.ಲಂಬಸಾಲು >= (colnr_T)(p - look))
	    {
		ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ = FALSE;

		ಆದರೆ (keytyped == ಕೀಲಿ_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    ಸಾಲು = ml_get_curline();
		    ಆದರೆ (has_mbyte)
		    {
			char_u	*n;

			ಗಾಗಿ (s = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು; s > ಸಾಲು; s = n)
			{
			    n = mb_prevptr(ಸಾಲು, s);
			    ಆದರೆ (!vim_iswordp(n))
				ನಿಲ್ಲಿಸು;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಗಾಗಿ (s = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು; s > ಸಾಲು; --s)
			    ಆದರೆ (!vim_iswordc(s[-1]))
				ನಿಲ್ಲಿಸು;
		    ಆದರೆ (s + (p - look) <= ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			ಹೊಂದಿಕೆ = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    // TODO: multi-byte
		    ಆದರೆ (keytyped == (ಪೂರ್ಣಾಂಕ)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ಪೂರ್ಣಾಂಕ)p[-1])))
		{
		    ಸಾಲು = ml_get_cursor();
		    ಆದರೆ ((curwin->w_cursor.ಲಂಬಸಾಲು == (colnr_T)(p - look)
				|| !vim_iswordc(ಸಾಲು[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(ಸಾಲು - (p - look), look, p - look)
				: STRNCMP(ಸಾಲು - (p - look), look, p - look))
									 == 0)
			ಹೊಂದಿಕೆ = TRUE;
		}
		ಆದರೆ (ಹೊಂದಿಕೆ && try_ಹೊಂದಿಕೆ_ಶಬ್ದ && !try_ಹೊಂದಿಕೆ)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ಆದರೆ (getwhitecols_curline() !=
				     (ಪೂರ್ಣಾಂಕ)(curwin->w_cursor.ಲಂಬಸಾಲು - (p - look)))
			ಹೊಂದಿಕೆ = FALSE;
		}
		ಆದರೆ (ಹೊಂದಿಕೆ)
		    ಹಿಂತಿರುಗಿ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (try_ಹೊಂದಿಕೆ && *look == keytyped)
		ಹಿಂತಿರುಗಿ TRUE;
	    ಆದರೆ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_ಗಮ್ಯ_option_part(look);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    ಶೂನ್ಯ
do_c_ಅಭಿವ್ಯಕ್ತಿ_indent(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (*curbuf->b_p_inde != NUL)
	fixthisline(get_ಅಭಿವ್ಯಕ್ತಿ_indent);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	fixthisline(get_c_indent);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    ಶೂನ್ಯ
f_cindent(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ಆದರೆ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ಸಂಖ್ಯೆ = get_c_indent();
	curwin->w_cursor = pos;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#ಆದರೆ_ಅಂತ್ಯ
