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
#define LOOKFOR_INITIAL		0
#define LOOKFOR_IF		1
#define LOOKFOR_DO		2
#define LOOKFOR_CASE		3
#define LOOKFOR_ANY		4
#define LOOKFOR_TERM		5
#define LOOKFOR_UNTERM		6
#define LOOKFOR_SCOPEDECL	7
#define LOOKFOR_NOBREAK		8
#define LOOKFOR_CPP_BASECLASS	9
#define LOOKFOR_ENUM_OR_INIT	10
#define LOOKFOR_JS_ಕೀಲಿ		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    ಪೂರ್ಣಾಙ್ಕ
cin_is_cinword(char_u *ಸಾಲು)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    ಪೂರ್ಣಾಙ್ಕ		cinw_len;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len;

    cinw_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ಯದಿ (cinw_buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಸಾಲು = skipwhite(ಸಾಲು);
    ಪ್ರತಿ (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ಯದಿ (STRNCMP(ಸಾಲು, cinw_buf, len) == 0
		&& (!vim_iswordc(ಸಾಲು[len]) || !vim_iswordc(ಸಾಲು[len - 1])))
	{
	    retval = TRUE;
	    ಭಙ್ಗ;
	}
    }
    vim_free(cinw_buf);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ಸ್ಥಿರ char_u *
skip_string(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    // We loop, because strings may be concatenated: "date""time".
    ಪ್ರತಿ ( ; ; ++p)
    {
	ಯದಿ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ಯದಿ (p[1] == NUL)		    // ' at end of line
		ಭಙ್ಗ;
	    i = 2;
	    ಯದಿ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ಯಾವತ್ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ಯದಿ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ಅನುವರ್ತ;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '"')		    // start of string
	{
	    ಪ್ರತಿ (++p; p[0]; ++p)
	    {
		ಯದಿ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ಅನ್ಯಥಾ ಯದಿ (p[0] == '"')	    // end of string
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (p[0] == '"')
		ಅನುವರ್ತ; // continue for another string
	}
	ಅನ್ಯಥಾ ಯದಿ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ಯದಿ (paren != ರಿಕ್ತ)
	    {
		size_t delim_len = paren - delim;

		ಪ್ರತಿ (p += 3; *p; ++p)
		    ಯದಿ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ಭಙ್ಗ;
		    }
		ಯದಿ (p[0] == '"')
		    ಅನುವರ್ತ; // continue for another string
	    }
	}
	ಭಙ್ಗ;				    // no string found
    }
    ಯದಿ (!*p)
	--p;				    // backup from NUL
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_pos_in_string(char_u *ಸಾಲು, colnr_T col)
{
    char_u *p;

    ಪ್ರತಿ (p = ಸಾಲು; *p && (colnr_T)(p - ಸಾಲು) < col; ++p)
	p = skip_string(p);
    ಪ್ರತ್ಯಾಗಮ !((colnr_T)(p - ಸಾಲು) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    ಪೂರ್ಣಾಙ್ಕ
check_linecomment(char_u *ಸಾಲು)
{
    char_u  *p;

    p = ಸಾಲು;
    // skip Lispish one-line comments
    ಯದಿ (curbuf->b_p_lisp)
    {
	ಯದಿ (vim_strchr(p, ';') != ರಿಕ್ತ) // there may be comments
	{
	    ಪೂರ್ಣಾಙ್ಕ in_str = FALSE;	// inside of string

	    p = ಸಾಲು;		// scan from start
	    ಯಾವತ್ ((p = vim_strpbrk(p, (char_u *)"\";")) != ರಿಕ್ತ)
	    {
		ಯದಿ (*p == '"')
		{
		    ಯದಿ (in_str)
		    {
			ಯದಿ (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (p == ಸಾಲು || ((p - ಸಾಲು) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		ಅನ್ಯಥಾ ಯದಿ (!in_str && ((p - ಸಾಲು) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(ಸಾಲು, (colnr_T)(p - ಸಾಲು)))
		    ಭಙ್ಗ;	// found!
		++p;
	    }
	}
	ಅನ್ಯಥಾ
	    p = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	ಪ್ರತಿ ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ಯದಿ (p[0] == '/' && p[1] == '/'
				 && (p == ಸಾಲು || p[-1] != '*' || p[2] != '*'))
		ಭಙ್ಗ;
	}
	ಯದಿ (*p == NUL)
	    p = ರಿಕ್ತ;
    }

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ MAXCOL;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ಸ್ಥಿರ pos_T *
ind_find_start_comment(ಶೂನ್ಯ)	    // XXX
{
    ಪ್ರತ್ಯಾಗಮ find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(ಪೂರ್ಣಾಙ್ಕ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ಪೂರ್ಣಾಙ್ಕ		cur_maxcomment = ind_maxcomment;

    ಪ್ರತಿ (;;)
    {
	pos = findmatchlimit(ರಿಕ್ತ, '*', FM_BACKWARD, cur_maxcomment);
	ಯದಿ (pos == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ಯದಿ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ಭಙ್ಗ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ಯದಿ (cur_maxcomment <= 0)
	{
	    pos = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ಸ್ಥಿರ pos_T *
find_start_rawstring(ಪೂರ್ಣಾಙ್ಕ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    ಪೂರ್ಣಾಙ್ಕ		cur_maxcomment = ind_maxcomment;

    ಪ್ರತಿ (;;)
    {
	pos = findmatchlimit(ರಿಕ್ತ, 'R', FM_BACKWARD, cur_maxcomment);
	ಯದಿ (pos == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ಯದಿ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ಭಙ್ಗ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ಯದಿ (cur_maxcomment <= 0)
	{
	    pos = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ pos;
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
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    ಸ್ಥಿರ pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    ಯದಿ (comment_pos != ರಿಕ್ತ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ಯದಿ (comment_pos == ರಿಕ್ತ || (rs_pos != ರಿಕ್ತ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ಯದಿ (is_raw != ರಿಕ್ತ && rs_pos != ರಿಕ್ತ)
	    *is_raw = rs_pos->lnum;
	ಪ್ರತ್ಯಾಗಮ rs_pos;
    }
    ಪ್ರತ್ಯಾಗಮ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    ಪೂರ್ಣಾಙ್ಕ
cindent_on(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ಸ್ಥಿರ char_u *
cin_skipcomment(char_u *s)
{
    ಯಾವತ್ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ಯದಿ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ಭಙ್ಗ;
	}
	ಯದಿ (*s != '/')
	    ಭಙ್ಗ;
	++s;
	ಯದಿ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ಭಙ್ಗ;
	}
	ಯದಿ (*s != '*')
	    ಭಙ್ಗ;
	ಪ್ರತಿ (++s; *s; ++s)	// skip slash-star comment
	    ಯದಿ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ಭಙ್ಗ;
	    }
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_nocode(char_u *s)
{
    ಪ್ರತ್ಯಾಗಮ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_iscomment(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_islinecomment(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ಸ್ಥಿರ pos_T *
find_ಸಾಲು_comment(ಶೂನ್ಯ) // XXX
{
    ಸ್ಥಿರ pos_T pos;
    char_u	 *ಸಾಲು;
    char_u	 *p;

    pos = curwin->w_cursor;
    ಯಾವತ್ (--pos.lnum > 0)
    {
	ಸಾಲು = ml_get(pos.lnum);
	p = skipwhite(ಸಾಲು);
	ಯದಿ (cin_islinecomment(p))
	{
	    pos.col = (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
	    ಪ್ರತ್ಯಾಗಮ &pos;
	}
	ಯದಿ (*p != NUL)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_has_js_ಕೀಲಿ(char_u *text)
{
    char_u *s = skipwhite(text);
    ಪೂರ್ಣಾಙ್ಕ	    quote = -1;

    ಯದಿ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ಯದಿ (!vim_isIDc(*s))	    // need at least one ID character
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯಾವತ್ (vim_isIDc(*s))
	++s;
    ಯದಿ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ಪ್ರತ್ಯಾಗಮ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_islabel_skip(char_u **s)
{
    ಯದಿ (!vim_isIDc(**s))	    // need at least one ID character
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯಾವತ್ (vim_isIDc(**s))
    {
	ಯದಿ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ಅನ್ಯಥಾ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ಪ್ರತ್ಯಾಗಮ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    ಪೂರ್ಣಾಙ್ಕ	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ಯದಿ (cinsd_buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತಿ (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (ಪೂರ್ಣಾಙ್ಕ)cinsd_len, ",");
	ಯದಿ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ಯದಿ (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }

    vim_free(cinsd_buf);
    ಪ್ರತ್ಯಾಗಮ found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_ispreproc(char_u *s)
{
    ಯದಿ (*skipwhite(s) == '#')
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, ಪೂರ್ಣಾಙ್ಕ *amount)
{
    char_u	*ಸಾಲು = *pp;
    linenr_T	lnum = *lnump;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		candidate_amount = *amount;

    ಯದಿ (*ಸಾಲು != NUL && ಸಾಲು[STRLEN(ಸಾಲು) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (cin_ispreproc(ಸಾಲು))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ಭಙ್ಗ;
	}
	ಯದಿ (lnum == 1)
	    ಭಙ್ಗ;
	ಸಾಲು = ml_get(--lnum);
	ಯದಿ (*ಸಾಲು == NUL || ಸಾಲು[STRLEN(ಸಾಲು) - 1] != '\\')
	    ಭಙ್ಗ;
    }

    ಯದಿ (lnum != *lnump)
	*pp = ml_get(*lnump);
    ಯದಿ (retval)
	*amount = candidate_amount;
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_iselse(
    char_u  *p)
{
    ಯದಿ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isterminated(
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		incl_open,	// include '{' at the end as terminator
    ಪೂರ್ಣಾಙ್ಕ		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_else = FALSE;

    s = cin_skipcomment(s);

    ಯದಿ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    ಯದಿ (!found_start)
	is_else = cin_iselse(s);

    ಯಾವತ್ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ಯದಿ (*s == '}' && n_open > 0)
	    --n_open;
	ಯದಿ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ಪ್ರತ್ಯಾಗಮ *s;
	ಅನ್ಯಥಾ ಯದಿ (*s == '{')
	{
	    ಯದಿ (incl_open && cin_nocode(s + 1))
		ಪ್ರತ್ಯಾಗಮ *s;
	    ಅನ್ಯಥಾ
		++n_open;
	}

	ಯದಿ (*s)
	    s++;
    }
    ಪ್ರತ್ಯಾಗಮ found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_starts_with(char_u *s, ವರ್ಣ *word)
{
    ಪೂರ್ಣಾಙ್ಕ l = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(word);

    ಪ್ರತ್ಯಾಗಮ (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isdefault(char_u *s)
{
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_iscase(
    char_u *s,
    ಪೂರ್ಣಾಙ್ಕ strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ಯದಿ (cin_starts_with(s, "case"))
    {
	ಪ್ರತಿ (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ಯದಿ (*s == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (*s == ':')
	    {
		ಯದಿ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ಅನ್ಯಥಾ
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    ಯದಿ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ಅನ್ಯಥಾ ಯದಿ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ಪ್ರತ್ಯಾಗಮ FALSE;		// stop at comment
	    ಅನ್ಯಥಾ ಯದಿ (*s == '"')
	    {
		// JS etc.
		ಯದಿ (strict)
		    ಪ್ರತ್ಯಾಗಮ FALSE;		// stop at string
		ಅನ್ಯಥಾ
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (cin_isdefault(s))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_islabel(ಶೂನ್ಯ)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ಯದಿ (cin_isdefault(s))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (cin_isscopedecl(s))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!cin_islabel_skip(&s))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (ind_find_start_CORS(ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*ಸಾಲು;

    cursor_save = curwin->w_cursor;
    ಯಾವತ್ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	ಯದಿ ((trypos = ind_find_start_CORS(ರಿಕ್ತ)) != ರಿಕ್ತ) // XXX
	    curwin->w_cursor = *trypos;

	ಸಾಲು = ml_get_curline();
	ಯದಿ (cin_ispreproc(ಸಾಲು))	// ignore #defines, #if, etc.
	    ಅನುವರ್ತ;
	ಯದಿ (*(ಸಾಲು = cin_skipcomment(ಸಾಲು)) == NUL)
	    ಅನುವರ್ತ;

	curwin->w_cursor = cursor_save;
	ಯದಿ (cin_isterminated(ಸಾಲು, TRUE, FALSE)
		|| cin_isscopedecl(ಸಾಲು)
		|| cin_iscase(ಸಾಲು, TRUE)
		|| (cin_islabel_skip(&ಸಾಲು) && cin_nocode(ಸಾಲು)))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    curwin->w_cursor = cursor_save;
    ಪ್ರತ್ಯಾಗಮ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(find);

    ಯಾವತ್ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ಯದಿ (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    ಯದಿ (cin_nocode(r))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	ಯದಿ (*p != NUL)
	    ++p;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ಸ್ಥಿರ char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ರಿಕ್ತ, *p = s;
    ಕುರು
    {
	r = p;
	p = cin_skipcomment(p);
	ಯದಿ (*p)
	    p = skip_string(p);
    } ಯಾವತ್ (p != r);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ರಿಕ್ತ;

    ಯಾವತ್ (*p)
    {
	ಯದಿ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ಅನ್ಯಥಾ
	    p = cin_skip_comment_and_string(p + 1);
    }
    ಯದಿ (!r)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    p = r; // p points now after '=' or "return"

    ಯದಿ (cin_nocode(p))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (*p == '&')
	p = cin_skipcomment(p + 1);

    ಯದಿ (*p == '(') // skip a typecast
    {
	ಪೂರ್ಣಾಙ್ಕ open_ಎಣಿಕೆ = 1;
	ಕುರು
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ಯದಿ (cin_nocode(p))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    open_ಎಣಿಕೆ += (*p == '(') - (*p == ')');
	} ಯಾವತ್ (open_ಎಣಿಕೆ);
	p = cin_skipcomment(p + 1);
	ಯದಿ (cin_nocode(p))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯಾವತ್ (*p == '{')
	p = cin_skipcomment(p + 1);
    ಪ್ರತ್ಯಾಗಮ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isinit(ಶೂನ್ಯ)
{
    char_u	*s;
    ಸ್ಥಿರ ವರ್ಣ *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ಯದಿ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ i, l;

	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(skip); ++i)
	{
	    l = (ಪೂರ್ಣಾಙ್ಕ)strlen(skip[i]);
	    ಯದಿ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (l != 0)
	    ಭಙ್ಗ;
    }

    ಯದಿ (cin_starts_with(s, "enum"))
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪ್ರತ್ಯಾಗಮ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		has_name = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ಯಾವತ್ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ಯದಿ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ಯಾವತ್ (*p != NUL)
	{
	    ಯದಿ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == '{')
	    {
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		ಯದಿ (has_name)
		    ಪ್ರತ್ಯಾಗಮ FALSE; // word character after skipping past name
		++p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ಯದಿ (!has_name_start || has_name)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ಯದಿ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ಯಾವತ್ (*p != NUL)
	{
	    ಯದಿ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == '{')
	    {
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ಯದಿ (has_string_literal)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ಯದಿ (has_string_literal)
		    ಪ್ರತ್ಯಾಗಮ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ has_string_literal ? TRUE : FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪ್ರತಿ ( ; *l; ++l)
    {
	ಯದಿ (*l == ':')
	{
	    ಯದಿ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ಅನ್ಯಥಾ ಯದಿ (!cin_iscase(l + 1, FALSE))
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ಯದಿ (*l == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    l = cin_skipcomment(l + 1);
    ಯದಿ (*l == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    ಪೂರ್ಣಾಙ್ಕ		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ಯದಿ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ಯದಿ (l == ರಿಕ್ತ)		// just in case
	    l = ml_get_curline();
    }
    ಅನ್ಯಥಾ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ಪ್ರತ್ಯಾಗಮ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_first_id_amount(ಶೂನ್ಯ)
{
    char_u	*ಸಾಲು, *p, *s;
    ಪೂರ್ಣಾಙ್ಕ		len;
    pos_T	fp;
    colnr_T	col;

    ಸಾಲು = ml_get_curline();
    p = skipwhite(ಸಾಲು);
    len = (ಪೂರ್ಣಾಙ್ಕ)(skiptowhite(p) - p);
    ಯದಿ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (ಪೂರ್ಣಾಙ್ಕ)(skiptowhite(p) - p);
    }
    ಯದಿ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    ಅನ್ಯಥಾ ಯದಿ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ಅನ್ಯಥಾ ಯದಿ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ಯದಿ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    ಪ್ರತಿ (len = 0; vim_isIDc(p[len]); ++len)
	;
    ಯದಿ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ಪ್ರತ್ಯಾಗಮ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - ಸಾಲು);
    getvcol(curwin, &fp, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)col;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*ಸಾಲು;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    ಯದಿ (lnum > 1)
    {
	ಸಾಲು = ml_get(lnum - 1);
	ಯದಿ (*ಸಾಲು != NUL && ಸಾಲು[STRLEN(ಸಾಲು) - 1] == '\\')
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಸಾಲು = s = ml_get(lnum);
    ಯಾವತ್ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ರಿಕ್ತ)
    {
	ಯದಿ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಅನ್ಯಥಾ
	    ++s;
    }
    ಯದಿ (*s != '=')
	ಪ್ರತ್ಯಾಗಮ 0;

    s = skipwhite(s + 1);
    ಯದಿ (cin_nocode(s))
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - ಸಾಲು);
    getvcol(curwin, &fp, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_skip2pos(pos_T *trypos)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*new_p;

    p = ಸಾಲು = ml_get(trypos->lnum);
    ಯಾವತ್ (*p && (colnr_T)(p - ಸಾಲು) < trypos->col)
    {
	ಯದಿ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ಅನ್ಯಥಾ
	{
	    new_p = skip_string(p);
	    ಯದಿ (new_p == p)
		++p;
	    ಅನ್ಯಥಾ
		p = new_p;
	}
    }
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
}

    ಸ್ಥಿರ pos_T *
find_match_char(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ಸ್ಥಿರ pos_T pos_copy;
    ಪೂರ್ಣಾಙ್ಕ		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ಯದಿ ((trypos = findmatchlimit(ರಿಕ್ತ, c, FM_SKIPCOMM, ind_maxp_wk)) != ರಿಕ್ತ)
    {
	// check if the ( is in a // comment
	ಯದಿ ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (ಪೂರ್ಣಾಙ್ಕ)(cursor_save.lnum - trypos->lnum);
	    ಯದಿ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		ಗಚ್ಛ retry;
	    }
	    trypos = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ಯದಿ ((trypos_wk = ind_find_start_CORS(ರಿಕ್ತ)) != ರಿಕ್ತ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (ಪೂರ್ಣಾಙ್ಕ)(cursor_save.lnum
			- trypos_wk->lnum);
		ಯದಿ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    ಗಚ್ಛ retry;
		}
		trypos = ರಿಕ್ತ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ಪ್ರತ್ಯಾಗಮ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ಸ್ಥಿರ pos_T *
find_match_paren(ಪೂರ್ಣಾಙ್ಕ ind_maxparen)	// XXX
{
    ಪ್ರತ್ಯಾಗಮ find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_last_paren(char_u *l, ಪೂರ್ಣಾಙ್ಕ start, ಪೂರ್ಣಾಙ್ಕ end)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		open_ಎಣಿಕೆ = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    ಪ್ರತಿ (i = 0; l[i] != NUL; i++)
    {
	i = (ಪೂರ್ಣಾಙ್ಕ)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ಯದಿ (l[i] == NUL)
	    ಭಙ್ಗ;
	i = (ಪೂರ್ಣಾಙ್ಕ)(skip_string(l + i) - l);    // ignore parens in quotes
	ಯದಿ (l[i] == start)
	    ++open_ಎಣಿಕೆ;
	ಅನ್ಯಥಾ ಯದಿ (l[i] == end)
	{
	    ಯದಿ (open_ಎಣಿಕೆ > 0)
		--open_ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    pos_T	*trypos;
    ಪೂರ್ಣಾಙ್ಕ		just_started = TRUE;

    ಯದಿ (sp == ರಿಕ್ತ)
	s = ml_get(lnum);
    ಅನ್ಯಥಾ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ಯದಿ (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
    {
	lnum = trypos->lnum;
	ಯದಿ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ಯದಿ (cin_ispreproc(s))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯಾವತ್ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ಯದಿ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಅನ್ಯಥಾ ಯದಿ (*s == ':')
	{
	    ಯದಿ (*(s + 1) == ':')
		s += 2;
	    ಅನ್ಯಥಾ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಅನ್ಯಥಾ
	    ++s;
    }
    ಯದಿ (*s != '(')
	ಪ್ರತ್ಯಾಗಮ FALSE;		// ';', ' or "  before any () or no '('

    ಯಾವತ್ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ಯದಿ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    ಯದಿ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    ಗಚ್ಛ done;
	}
	ಯದಿ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    ಪೂರ್ಣಾಙ್ಕ comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ಭಙ್ಗ;
		s = ml_get(++lnum);
		ಯದಿ (!cin_ispreproc(s))
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (lnum >= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ಯದಿ (!just_started && (!comma && *s != ',' && *s != ')'))
		ಭಙ್ಗ;
	    just_started = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ಅನ್ಯಥಾ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    ಯದಿ (lnum != first_lnum && sp != ರಿಕ್ತ)
	*sp = ml_get(first_lnum);

    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isif(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isdo(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    p = cin_skipcomment(p);
    ಯದಿ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ಯದಿ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	ಯಾವತ್ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	ಯದಿ ((trypos = findmatchlimit(ರಿಕ್ತ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ರಿಕ್ತ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_is_if_for_while_before_offset(char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ *poffset)
{
    ಪೂರ್ಣಾಙ್ಕ offset = *poffset;

    ಯದಿ (offset-- < 2)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯಾವತ್ (offset > 2 && VIM_ISWHITE(ಸಾಲು[offset]))
	--offset;

    offset -= 1;
    ಯದಿ (STRNCMP(ಸಾಲು + offset, "if", 2) == 0)
	ಗಚ್ಛ probablyFound;

    ಯದಿ (offset >= 1)
    {
	offset -= 1;
	ಯದಿ (STRNCMP(ಸಾಲು + offset, "for", 3) == 0)
	    ಗಚ್ಛ probablyFound;

	ಯದಿ (offset >= 2)
	{
	    offset -= 2;
	    ಯದಿ (STRNCMP(ಸಾಲು + offset, "while", 5) == 0)
		ಗಚ್ಛ probablyFound;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;

probablyFound:
    ಯದಿ (!offset || !vim_isIDc(ಸಾಲು[offset - 1]))
    {
	*poffset = offset;
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_iswhileofdo_end(ಪೂರ್ಣಾಙ್ಕ terminated)
{
    char_u	*ಸಾಲು;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (terminated != ';')	// there must be a ';' at the end
	ಪ್ರತ್ಯಾಗಮ FALSE;

    p = ಸಾಲು = ml_get_curline();
    ಯಾವತ್ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ಯದಿ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ಯದಿ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (ಪೂರ್ಣಾಙ್ಕ)(p - ಸಾಲು);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		ಯದಿ (trypos != ರಿಕ್ತ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ಯದಿ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ಯದಿ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ಪ್ರತ್ಯಾಗಮ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		ಸಾಲು = ml_get_curline();
		p = ಸಾಲು + i;
	    }
	}
	ಯದಿ (*p != NUL)
	    ++p;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_isbreak(char_u *p)
{
    ಪ್ರತ್ಯಾಗಮ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*ಸಾಲು = ml_get_curline();

    ಯದಿ (pos->lnum <= lnum)
	ಪ್ರತ್ಯಾಗಮ cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(ಸಾಲು);
    ಯದಿ (*s == '#')		// skip #define FOO x ? (x) : x
	ಪ್ರತ್ಯಾಗಮ FALSE;
    s = cin_skipcomment(s);
    ಯದಿ (*s == NUL)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;

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
    ಯಾವತ್ (lnum > 1)
    {
	ಸಾಲು = ml_get(lnum - 1);
	s = skipwhite(ಸಾಲು);
	ಯದಿ (*s == '#' || *s == NUL)
	    ಭಙ್ಗ;
	ಯಾವತ್ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ಯದಿ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ಭಙ್ಗ;
	    ಯದಿ (*s != NUL)
		++s;
	}
	ಯದಿ (*s != NUL)
	    ಭಙ್ಗ;
	--lnum;
    }

    pos->lnum = lnum;
    ಸಾಲು = ml_get(lnum);
    s = ಸಾಲು;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (*s == NUL)
	{
	    ಯದಿ (lnum == curwin->w_cursor.lnum)
		ಭಙ್ಗ;
	    // Continue in the cursor line.
	    ಸಾಲು = ml_get(++lnum);
	    s = ಸಾಲು;
	}
	ಯದಿ (s == ಸಾಲು)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ಯದಿ (cin_iscase(s, FALSE))
		ಭಙ್ಗ;
	    s = cin_skipcomment(ಸಾಲು);
	    ಯದಿ (*s == NUL)
		ಅನುವರ್ತ;
	}

	ಯದಿ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ಅನ್ಯಥಾ ಯದಿ (s[0] == ':')
	{
	    ಯದಿ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ಅನ್ಯಥಾ
		s = cin_skipcomment(s + 1);
	}
	ಅನ್ಯಥಾ ಯದಿ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ಯದಿ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ಅನ್ಯಥಾ
		s = cin_skipcomment(s + 6);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ಯದಿ (cpp_base_class)
		    pos->col = (colnr_T)(s - ಸಾಲು);
	    }

	    // When the line ends in a comma don't align with it.
	    ಯದಿ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    ಯದಿ (cpp_base_class)
	pos->lnum = lnum;
    ಪ್ರತ್ಯಾಗಮ cpp_base_class;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_baseclass_amount(ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ಯದಿ (col == 0)
    {
	amount = get_indent();
	ಯದಿ (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ಯದಿ (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ಅನ್ಯಥಾ
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	amount = (ಪೂರ್ಣಾಙ್ಕ)vcol;
    }
    ಯದಿ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ಪ್ರತ್ಯಾಗಮ amount;
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
find_start_brace(ಶೂನ್ಯ)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ಸ್ಥಿರ pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ಯಾವತ್ ((trypos = findmatchlimit(ರಿಕ್ತ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ರಿಕ್ತ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ರಿಕ್ತ;
	// ignore the { if it's in a // or / *  * / comment
	ಯದಿ ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(ರಿಕ್ತ)) == ರಿಕ್ತ) // XXX
	    ಭಙ್ಗ;
	ಯದಿ (pos != ರಿಕ್ತ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ಪ್ರತ್ಯಾಗಮ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ಸ್ಥಿರ pos_T *
find_match_paren_after_brace (ಪೂರ್ಣಾಙ್ಕ ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    ಯದಿ (trypos == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ಯದಿ (tryposBrace != ರಿಕ್ತ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    ಯದಿ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ಪ್ರತ್ಯಾಗಮ curbuf->b_ind_maxparen - (ಪೂರ್ಣಾಙ್ಕ)n;
    ಪ್ರತ್ಯಾಗಮ curbuf->b_ind_maxparen;
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
    ಪೂರ್ಣಾಙ್ಕ		divider;
    ಪೂರ್ಣಾಙ್ಕ		fraction = 0;
    ಪೂರ್ಣಾಙ್ಕ		sw;
    long	t = get_sw_ಮೌಲ್ಯ(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ಯದಿ (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    ಅನ್ಯಥಾ
	sw = (ಪೂರ್ಣಾಙ್ಕ)t;

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
    buf->b_ind_first_open = 0;

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
    buf->b_ind_case_code = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_code = sw;

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
    buf->b_ind_in_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_in_comment2 = 0;

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

    ಪ್ರತಿ (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ಯದಿ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ಯದಿ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((ವರ್ಣ *)++p);
	    ಯಾವತ್ (VIM_ISDIGIT(*p))
	    {
		++p;
		ಯದಿ (divider)
		    divider *= 10;
		ಅನ್ಯಥಾ
		    divider = 10;
	    }
	}
	ಯದಿ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ಯದಿ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ಅನ್ಯಥಾ
	    {
		n *= sw;
		ಯದಿ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ಯದಿ (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	ವಿಕಲ್ಪ (*l)
	{
	    ಪಕ್ಷ '>': buf->b_ind_level = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'e': buf->b_ind_open_imag = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'n': buf->b_ind_no_brace = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'f': buf->b_ind_first_open = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '{': buf->b_ind_open_extra = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '}': buf->b_ind_close_extra = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '^': buf->b_ind_open_ಎಡ_imag = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'L': buf->b_ind_jump_label = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ ':': buf->b_ind_case = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '=': buf->b_ind_case_code = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'b': buf->b_ind_case_break = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'p': buf->b_ind_param = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 't': buf->b_ind_func_type = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '/': buf->b_ind_comment = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'c': buf->b_ind_in_comment = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'C': buf->b_ind_in_comment2 = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'i': buf->b_ind_cpp_baseclass = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '+': buf->b_ind_continuation = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '(': buf->b_ind_unclosed = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'u': buf->b_ind_unclosed2 = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'U': buf->b_ind_unclosed_noignore = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'W': buf->b_ind_unclosed_wrapped = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'w': buf->b_ind_unclosed_whiteok = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'm': buf->b_ind_matching_paren = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'M': buf->b_ind_paren_prev = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ ')': buf->b_ind_maxparen = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '*': buf->b_ind_maxcomment = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'g': buf->b_ind_scopedecl = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'h': buf->b_ind_scopedecl_code = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'j': buf->b_ind_java = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'J': buf->b_ind_js = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'l': buf->b_ind_keep_case_label = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ '#': buf->b_ind_hash_comment = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'N': buf->b_ind_cpp_namespace = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'k': buf->b_ind_if_for_while = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'E': buf->b_ind_cpp_extern_c = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	    ಪಕ್ಷ 'P': buf->b_ind_pragma = (ಪೂರ್ಣಾಙ್ಕ)n; ಭಙ್ಗ;
	}
	ಯದಿ (*p == ',')
	    ++p;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_match(ಪೂರ್ಣಾಙ್ಕ lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    ಪೂರ್ಣಾಙ್ಕ		elselevel;
    ಪೂರ್ಣಾಙ್ಕ		whilelevel;

    ಯದಿ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    ಅನ್ಯಥಾ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    ಯಾವತ್ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	ಯದಿ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    ಯದಿ (theirscope == ರಿಕ್ತ)
		ಭಙ್ಗ;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ಯದಿ (theirscope->lnum < ourscope)
		ಭಙ್ಗ;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ಯದಿ (theirscope->lnum > ourscope)
		ಅನುವರ್ತ;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ಯದಿ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ಯದಿ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ಯದಿ (!cin_isif(mightbeif))
			++elselevel;
		    ಅನುವರ್ತ;
		}

		// If it's an "if" decrement elselevel
		ಯದಿ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ಯದಿ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ಯದಿ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ಅನುವರ್ತ;
	    }

	    // If it's a "do" decrement whilelevel
	    ಯದಿ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ಯದಿ (elselevel <= 0 && whilelevel <= 0)
		ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    ಪೂರ್ಣಾಙ್ಕ
get_c_indent(ಶೂನ್ಯ)
{
    pos_T	cur_curpos;
    ಪೂರ್ಣಾಙ್ಕ		amount;
    ಪೂರ್ಣಾಙ್ಕ		scope_amount;
    ಪೂರ್ಣಾಙ್ಕ		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ರಿಕ್ತ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    ಪೂರ್ಣಾಙ್ಕ		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    ಪೂರ್ಣಾಙ್ಕ		lookfor;
    ಪೂರ್ಣಾಙ್ಕ		whilelevel;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		iscase;
    ಪೂರ್ಣಾಙ್ಕ		lookfor_break;
    ಪೂರ್ಣಾಙ್ಕ		lookfor_cpp_namespace = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		cont_amount = 0;    // amount for continuation line
    ಪೂರ್ಣಾಙ್ಕ		original_ಸಾಲು_islabel;
    ಪೂರ್ಣಾಙ್ಕ		added_to_amount = 0;
    ಪೂರ್ಣಾಙ್ಕ		js_cur_has_ಕೀಲಿ = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    ಪೂರ್ಣಾಙ್ಕ		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ಯದಿ (cur_curpos.lnum == 1)
	ಪ್ರತ್ಯಾಗಮ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ಯದಿ (linecopy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ಯದಿ ((State & MODE_ಸೇರಿಸು)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_ಸಾಲು_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    ಯದಿ (comment_pos != ರಿಕ್ತ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    ಯದಿ (trypos != ರಿಕ್ತ && (comment_pos == ರಿಕ್ತ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	ಗಚ್ಛ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ಯದಿ (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ಯದಿ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    ಗಚ್ಛ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ಯದಿ (original_ಸಾಲು_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	ಗಚ್ಛ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ಯದಿ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_ಸಾಲು_comment(); // XXX
	ಯದಿ (trypos == ರಿಕ್ತ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ಯದಿ (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ಯದಿ (trypos  != ರಿಕ್ತ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	    amount = col;
	    ಗಚ್ಛ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ಯದಿ (!cin_iscomment(theline) && comment_pos != ರಿಕ್ತ) // XXX
    {
	ಪೂರ್ಣಾಙ್ಕ	lead_start_len = 2;
	ಪೂರ್ಣಾಙ್ಕ	lead_ಮಧ್ಯ_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_ಮಧ್ಯ[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	ಪೂರ್ಣಾಙ್ಕ	lead_end_len;
	char_u	*p;
	ಪೂರ್ಣಾಙ್ಕ	start_align = 0;
	ಪೂರ್ಣಾಙ್ಕ	start_off = 0;
	ಪೂರ್ಣಾಙ್ಕ	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	amount = col;
	*lead_start = NUL;
	*lead_ಮಧ್ಯ = NUL;

	p = curbuf->b_p_com;
	ಯಾವತ್ (*p != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ	align = 0;
	    ಪೂರ್ಣಾಙ್ಕ	off = 0;
	    ಪೂರ್ಣಾಙ್ಕ what = 0;

	    ಯಾವತ್ (*p != NUL && *p != ':')
	    {
		ಯದಿ (*p == COM_START || *p == COM_END || *p == COM_ಮಧ್ಯ)
		    what = *p++;
		ಅನ್ಯಥಾ ಯದಿ (*p == COM_ಎಡ || *p == COM_ಬಲ)
		    align = *p++;
		ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ಅನ್ಯಥಾ
		    ++p;
	    }

	    ಯದಿ (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    ಯದಿ (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (what == COM_ಮಧ್ಯ)
	    {
		STRCPY(lead_ಮಧ್ಯ, lead_end);
		lead_ಮಧ್ಯ_len = lead_end_len;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ಯದಿ (STRNCMP(theline, lead_ಮಧ್ಯ, lead_ಮಧ್ಯ_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    ಯದಿ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ಯದಿ (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ಅನ್ಯಥಾ ಯದಿ (STRNCMP(look, lead_ಮಧ್ಯ,
							lead_ಮಧ್ಯ_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ಭಙ್ಗ;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ಅನ್ಯಥಾ ಯದಿ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    ಅನುವರ್ತ;
		    }
		    ಯದಿ (start_off != 0)
			amount += start_off;
		    ಅನ್ಯಥಾ ಯದಿ (start_align == COM_ಬಲ)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_ಮಧ್ಯ);
		    ಭಙ್ಗ;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ಯದಿ (STRNCMP(theline, lead_ಮಧ್ಯ, lead_ಮಧ್ಯ_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ಯದಿ (off != 0)
			amount += off;
		    ಅನ್ಯಥಾ ಯದಿ (align == COM_ಬಲ)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_ಮಧ್ಯ);
		    done = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ಯದಿ (done)
	    ;
	ಅನ್ಯಥಾ ಯದಿ (theline[0] == '*')
	    amount += 1;
	ಅನ್ಯಥಾ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    ಪ್ರತಿ (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ಯದಿ (linewhite(lnum))		    // skip blank lines
		    ಅನುವರ್ತ;
		amount = get_indent_lnum(lnum);	    // XXX
		ಭಙ್ಗ;
	    }
	    ಯದಿ (amount == -1)			    // use the comment opener
	    {
		ಯದಿ (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    ಯದಿ (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
		amount = col;
		ಯದಿ (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	ಗಚ್ಛ theend;
    }

    // Are we looking at a ']' that has a match?
    ಯದಿ (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	ಗಚ್ಛ theend;
    }

    // Are we inside parentheses or braces?  XXX
    ಯದಿ (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ರಿಕ್ತ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != ರಿಕ್ತ
	    || trypos != ರಿಕ್ತ)
    {
      ಯದಿ (trypos != ರಿಕ್ತ && tryposBrace != ರಿಕ್ತ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ಯದಿ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = ರಿಕ್ತ;
	  ಅನ್ಯಥಾ
	      tryposBrace = ರಿಕ್ತ;
      }

      ಯದಿ (trypos != ರಿಕ್ತ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ಯದಿ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ಅನ್ಯಥಾ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    ಪ್ರತಿ (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ಯದಿ (cin_nocode(l))		// skip comment lines
		    ಅನುವರ್ತ;
		ಯದಿ (cin_ispreproc_cont(&l, &lnum, &amount))
		    ಅನುವರ್ತ;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ಯದಿ ((trypos = ind_find_start_CORS(ರಿಕ್ತ)) != ರಿಕ್ತ)
		{
		    lnum = trypos->lnum + 1;
		    ಅನುವರ್ತ;
		}

		// XXX
		ಯದಿ ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != ರಿಕ್ತ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ಯದಿ (theline[0] == ')')
		    {
			ಯದಿ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ಭಙ್ಗ;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ಯದಿ (amount == -1)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    ignore_paren_col = 0;
	    ಪೂರ್ಣಾಙ್ಕ	    is_if_for_while = 0;

	    ಯದಿ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *ಸಾಲು;

		trypos = &our_paren_pos;
		ಕುರು
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} ಯಾವತ್ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		ಸಾಲು = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(ಸಾಲು, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ಯದಿ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *ಸಾಲು;
		ಪೂರ್ಣಾಙ್ಕ	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		ಸಾಲು = ml_get_curline();
		look_col = (ಪೂರ್ಣಾಙ್ಕ)(look - ಸಾಲು);
		curwin->w_cursor.col = look_col + 1;
		ಯದಿ ((trypos = findmatchlimit(ರಿಕ್ತ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ರಿಕ್ತ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    ಯದಿ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_col == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		ಯದಿ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ಯದಿ (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			ಪ್ರತಿ (col = 0; col < our_paren_pos.col; ++col)
			{
			    ವಿಕಲ್ಪ (l[col])
			    {
				ಪಕ್ಷ '(':
				ಪಕ್ಷ '{': ++n;
					  ಭಙ್ಗ;

				ಪಕ್ಷ ')':
				ಪಕ್ಷ '}': ಯದಿ (n > 1)
					      --n;
					  ಭಙ್ಗ;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    ಅನ್ಯಥಾ
		    {
			col = our_paren_pos.col + 1;
			ಯಾವತ್ (VIM_ISWHITE(l[col]))
			    col++;
			ಯದಿ (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			ಅನ್ಯಥಾ
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ಯದಿ (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
		    ಯದಿ (cur_amount > (ಪೂರ್ಣಾಙ್ಕ)col)
			cur_amount = col;
		}
	    }

	    ಯದಿ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		ಯದಿ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		ಯಾವತ್ ((ಪೂರ್ಣಾಙ್ಕ)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    ವಿಕಲ್ಪ (*ml_get_pos(&our_paren_pos))
		    {
			ಪಕ್ಷ '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  ಭಙ್ಗ;
			ಪಕ್ಷ ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  ಭಙ್ಗ;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ಯದಿ (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ಅನ್ಯಥಾ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    ಯದಿ (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != ರಿಕ್ತ)
			amount += curbuf->b_ind_unclosed2;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ಅನ್ಯಥಾ
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ಯದಿ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ಯದಿ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      ಅನ್ಯಥಾ
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	start = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(start);
	ಯದಿ (*look == '{')
	{
	    getvcol(curwin, trypos, &col, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	    amount = col;
	    ಯದಿ (*start == '{')
		start_brace = BRACE_IN_COL0;
	    ಅನ್ಯಥಾ
		start_brace = BRACE_AT_START;
	}
	ಅನ್ಯಥಾ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ಯದಿ (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != ರಿಕ್ತ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ಯದಿ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ಅನ್ಯಥಾ
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	ಯದಿ (curbuf->b_ind_js)
	    js_cur_has_ಕೀಲಿ = cin_has_js_ಕೀಲಿ(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ಯದಿ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	ಅನ್ಯಥಾ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ಯದಿ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ಅನ್ಯಥಾ ಯದಿ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ಯದಿ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ಯದಿ (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    ಗಚ್ಛ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ಯದಿ (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ಎಡ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ಯದಿ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ಅನ್ಯಥಾ ಯದಿ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ಅನ್ಯಥಾ
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ಯದಿ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ಯದಿ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    ಪ್ರತಿ (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ಯದಿ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ಯದಿ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಯದಿ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ಯದಿ (cont_amount > 0)
				amount = cont_amount;
			    ಅನ್ಯಥಾ ಯದಿ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ಭಙ್ಗ;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(ರಿಕ್ತ);
			ಯದಿ (trypos != ರಿಕ್ತ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    ಅನುವರ್ತ;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ಯದಿ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ಅನುವರ್ತ;

			ಯದಿ (cin_nocode(l))
			    ಅನುವರ್ತ;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ಯದಿ (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ಯದಿ (terminated == ',')
				ಭಙ್ಗ;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ಯದಿ (terminated != ';' && cin_isinit())
				ಭಙ್ಗ;

			    // nothing useful found
			    ಯದಿ (terminated == 0 || terminated == '{')
				ಅನುವರ್ತ;
			}

			ಯದಿ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ರಿಕ್ತ;
			    ಯದಿ (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    ಯದಿ (trypos == ರಿಕ್ತ && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    ಯದಿ (trypos != ರಿಕ್ತ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ಅನುವರ್ತ;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (lookfor == LOOKFOR_UNTERM)
		    {
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ಯದಿ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			ಯದಿ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ಯದಿ (curwin->w_cursor.lnum == ourscope)
				ಅನುವರ್ತ;

			    ಯದಿ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				ಭಙ್ಗ;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(ರಿಕ್ತ);
			    ಯದಿ (trypos != ರಿಕ್ತ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ಅನುವರ್ತ;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ಯದಿ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ಅನುವರ್ತ;

			    // Finally the actual check for "namespace".
			    ಯದಿ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				ಭಙ್ಗ;
			    }
			    ಅನ್ಯಥಾ ಯದಿ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				ಭಙ್ಗ;
			    }

			    ಯದಿ (cin_nocode(l))
				ಅನುವರ್ತ;
			}
		    }
		    ಭಙ್ಗ;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ಯದಿ ((trypos = ind_find_start_CORS(&raw_string_start)) != ರಿಕ್ತ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    ಅನುವರ್ತ;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ಯದಿ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ಯದಿ (lookfor == LOOKFOR_CPP_BASECLASS)
			ಭಙ್ಗ;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ಯದಿ (whilelevel > 0)
			ಅನುವರ್ತ;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ಯದಿ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
			ಭಙ್ಗ;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ಯದಿ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ಯದಿ ((trypos = find_start_brace()) == ರಿಕ್ತ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ಭಙ್ಗ;
			}
			ಅನುವರ್ತ;
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
		    ಯದಿ (lookfor == LOOKFOR_TERM)
		    {
			ಯದಿ (n)
			    amount = n;

			ಯದಿ (!lookfor_break)
			    ಭಙ್ಗ;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ಯದಿ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ಯದಿ (l != ರಿಕ್ತ && cin_is_cinword(l))
			{
			    ಯದಿ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ಅನ್ಯಥಾ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ಭಙ್ಗ;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_code
					: curbuf->b_ind_scopedecl_code);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    ಅನುವರ್ತ;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ಯದಿ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ಯದಿ (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != ರಿಕ್ತ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    ಅನುವರ್ತ;
		}

		// Ignore jump labels with nothing after them.
		ಯದಿ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ಯದಿ (l == ರಿಕ್ತ || cin_nocode(l))
			ಅನುವರ್ತ;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ಯದಿ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ಅನುವರ್ತ;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ಯದಿ (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ಯದಿ (n)
		{
		    ಯದಿ (lookfor == LOOKFOR_UNTERM)
		    {
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ಅನುವರ್ತ;
		    }
		    ಅನ್ಯಥಾ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ಯದಿ (cin_isterminated(l, TRUE, FALSE))
			ಭಙ್ಗ;
		    ಅನ್ಯಥಾ
			ಅನುವರ್ತ;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//	  here
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		ಯದಿ (js_cur_has_ಕೀಲಿ)
		{
		    js_cur_has_ಕೀಲಿ = 0; // only check the first line
		    ಯದಿ (curbuf->b_ind_js && terminated == ',')
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
		ಯದಿ (lookfor == LOOKFOR_JS_ಕೀಲಿ && cin_has_js_ಕೀಲಿ(l))
		{
		    amount = get_indent();
		    ಭಙ್ಗ;
		}
		ಯದಿ (lookfor == LOOKFOR_COMMA)
		{
		    ಯದಿ (tryposBrace != ರಿಕ್ತ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ಭಙ್ಗ;
		    ಯದಿ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ಭಙ್ಗ;
		    ಅನ್ಯಥಾ
		    {
			amount = get_indent();
			ಯದಿ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ಭಙ್ಗ;
		    }
		}

		ಯದಿ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ಯದಿ (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (ಶೂನ್ಯ)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    ಯದಿ (trypos != ರಿಕ್ತ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = ರಿಕ್ತ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ಯದಿ (trypos == ರಿಕ್ತ && terminated == ',')
		    {
			ಯದಿ (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    ಯದಿ (trypos != ರಿಕ್ತ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ಯದಿ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    ಅನುವರ್ತ;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ಯದಿ (terminated == ',')
		    {
			ಯಾವತ್ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ಯದಿ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ಭಙ್ಗ;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ಯದಿ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ಅನ್ಯಥಾ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ಯದಿ (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ಯದಿ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ಯದಿ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ಅನುವರ್ತ;
			}
			ಭಙ್ಗ;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ಯದಿ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ಯದಿ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ಯದಿ (cont_amount > 0)
				amount = cont_amount;
			    ಅನ್ಯಥಾ
				amount += ind_continuation;
			    ಭಙ್ಗ;
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
			ಯದಿ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ಯದಿ (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ಭಙ್ಗ;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ಯದಿ (cin_isdo(l))
			{
			    ಯದಿ (whilelevel == 0)
				ಭಙ್ಗ;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ಯದಿ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ಯದಿ (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ಯದಿ ((trypos = find_start_brace()) == ರಿಕ್ತ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ಭಙ್ಗ;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ಅನ್ಯಥಾ
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ಯದಿ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ಯದಿ (terminated == ',')
				amount += ind_continuation;
			    ಭಙ್ಗ;
			}

			ಯದಿ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ಯದಿ (terminated == ',')
			    {
				ಯದಿ (curbuf->b_ind_cpp_baseclass == 0)
				    ಭಙ್ಗ;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ಅನುವರ್ತ;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ಯದಿ (amount > cur_amount)
				amount = cur_amount;
			}
			ಅನ್ಯಥಾ
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(l);
			    ಯದಿ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ಭಙ್ಗ;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ಯದಿ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ಯದಿ (curbuf->b_ind_js)
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
				    ಯದಿ (cin_iscomment(skipwhite(l)))
					ಭಙ್ಗ;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    ಯದಿ (trypos != ರಿಕ್ತ)
				    {
					ಯದಿ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ಭಙ್ಗ;
					}
					ourscope = trypos->lnum;
				    }
				}
				ಅನ್ಯಥಾ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    ಅನ್ಯಥಾ
			    {
				ಯದಿ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ಯದಿ (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_ಕೀಲಿ
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ಅನ್ಯಥಾ ಯದಿ (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ಯದಿ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
			ಭಙ್ಗ;
		    }

		    ಯದಿ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			ಯದಿ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		ಅನ್ಯಥಾ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ಯದಿ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ಅನುವರ್ತ;
		    }

		    // Handle "do {" line.
		    ಯದಿ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ಯದಿ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ಅನುವರ್ತ;
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
		    ಯದಿ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ಯದಿ (cont_amount > 0)
			    amount = cont_amount;
			ಅನ್ಯಥಾ
			    amount += ind_continuation;
			ಭಙ್ಗ;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ಯದಿ (lookfor == LOOKFOR_TERM)
		    {
			ಯದಿ (!lookfor_break && whilelevel == 0)
			    ಭಙ್ಗ;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ಅನ್ಯಥಾ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			ಯದಿ (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ಯದಿ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				ಅನುವರ್ತ;
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

			ಯದಿ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ಯದಿ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ಯದಿ (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ಯದಿ ((trypos = find_start_brace()) == ರಿಕ್ತ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ಭಙ್ಗ;
			    ಅನುವರ್ತ;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ಯದಿ (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != ರಿಕ್ತ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ಯದಿ (*l == '}' || !cin_iselse(l))
				ಗಚ್ಛ term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ಯದಿ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ಯದಿ (curbuf->b_ind_jump_label > 0 && original_ಸಾಲು_islabel)
	  amount -= curbuf->b_ind_jump_label;

      ಗಚ್ಛ theend;
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

    ಯದಿ (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	ಗಚ್ಛ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ಯದಿ (cur_curpos.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ರಿಕ್ತ
	    && vim_strchr(theline, '}') == ರಿಕ್ತ
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(ರಿಕ್ತ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	ಗಚ್ಛ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ಯಾವತ್ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ಯದಿ ((trypos = ind_find_start_CORS(ರಿಕ್ತ)) != ರಿಕ್ತ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    ಅನುವರ್ತ;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ಯದಿ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ಯದಿ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    ಭಙ್ಗ;
	}

	// Skip preprocessor directives and blank lines.
	ಯದಿ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ಅನುವರ್ತ;

	ಯದಿ (cin_nocode(l))
	    ಅನುವರ್ತ;

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
	ಯದಿ (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ಯದಿ (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ಯಾವತ್ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ಯದಿ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ಭಙ್ಗ;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    ಯದಿ (amount == 0)
		amount = cin_first_id_amount();
	    ಯದಿ (amount == 0)
		amount = ind_continuation;
	    ಭಙ್ಗ;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ಯದಿ (cin_isfuncdecl(ರಿಕ್ತ, cur_curpos.lnum, 0))  // XXX
	    ಭಙ್ಗ;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ಯದಿ (*skipwhite(l) == '}')
	    ಭಙ್ಗ;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ಯದಿ (cin_ends_in(l, (char_u *)"};"))
	    ಭಙ್ಗ;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ಯದಿ (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ಭಙ್ಗ;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ಯದಿ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ಯಾವತ್ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ಯದಿ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ಭಙ್ಗ;
	    }
	    ಯದಿ (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		ಭಙ್ಗ;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ಯದಿ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ಭಙ್ಗ;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ಯದಿ (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ಯದಿ (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ಭಙ್ಗ;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	ಯದಿ ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ರಿಕ್ತ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ಭಙ್ಗ;
    }

    // add extra indent for a comment
    ಯದಿ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ಯದಿ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ಯದಿ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ಯದಿ (cur_amount > 0)
		amount = cur_amount;
	    ಅನ್ಯಥಾ ಯದಿ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ಯದಿ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ಪ್ರತ್ಯಾಗಮ amount;
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
    ಪೂರ್ಣಾಙ್ಕ
in_cinkeys(
    ಪೂರ್ಣಾಙ್ಕ		keytyped,
    ಪೂರ್ಣಾಙ್ಕ		when,
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_is_empty)
{
    char_u	*look;
    ಪೂರ್ಣಾಙ್ಕ		try_match;
    ಪೂರ್ಣಾಙ್ಕ		try_match_word;
    char_u	*p;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		icase;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ಪ್ರತ್ಯಾಗಮ FALSE;

#ifdef FEAT_EVAL
    ಯದಿ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ಅನ್ಯಥಾ
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ಯಾವತ್ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	ವಿಕಲ್ಪ (when)
	{
	    ಪಕ್ಷ '*': try_match = (*look == '*'); ಭಙ್ಗ;
	    ಪಕ್ಷ '!': try_match = (*look == '!'); ಭಙ್ಗ;
	    ಸ್ವಭಾವ: try_match = (*look != '*'); ಭಙ್ಗ;
	}
	ಯದಿ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ಯದಿ (*look == '0')
	{
	    try_match_word = try_match;
	    ಯದಿ (!ಸಾಲು_is_empty)
		try_match = FALSE;
	    ++look;
	}
	ಅನ್ಯಥಾ
	    try_match_word = FALSE;

	// does it look like a control character?
	ಯದಿ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ಯದಿ (try_match && keytyped == Ctrl_chr(look[1]))
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ಅನ್ಯಥಾ ಯದಿ (*look == 'o')
	{
	    ಯದಿ (try_match && keytyped == ಕೀಲಿ_OPEN_FORW)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ++look;
	}
	ಅನ್ಯಥಾ ಯದಿ (*look == 'O')
	{
	    ಯದಿ (try_match && keytyped == ಕೀಲಿ_OPEN_BACK)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ಅನ್ಯಥಾ ಯದಿ (*look == 'e')
	{
	    ಯದಿ (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		ಯದಿ (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ಅನ್ಯಥಾ ಯದಿ (*look == ':')
	{
	    ಯದಿ (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		ಯದಿ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ಪ್ರತ್ಯಾಗಮ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ಯದಿ (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    ಯದಿ (i)
			ಪ್ರತ್ಯಾಗಮ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ಅನ್ಯಥಾ ಯದಿ (*look == '<')
	{
	    ಯದಿ (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ಯದಿ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ರಿಕ್ತ
						       && keytyped == look[1])
		    ಪ್ರತ್ಯಾಗಮ TRUE;

		ಯದಿ (keytyped == get_special_ಕೀಲಿ_code(look + 1))
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    ಯಾವತ್ (*look && *look != '>')
		look++;
	    ಯಾವತ್ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ಅನ್ಯಥಾ ಯದಿ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ಯದಿ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ಅನ್ಯಥಾ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ಯದಿ (p == ರಿಕ್ತ)
		p = look + STRLEN(look);
	    ಯದಿ ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		ಪೂರ್ಣಾಙ್ಕ		match = FALSE;

		ಯದಿ (keytyped == ಕೀಲಿ_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    ಸಾಲು = ml_get_curline();
		    ಯದಿ (has_mbyte)
		    {
			char_u	*n;

			ಪ್ರತಿ (s = ಸಾಲು + curwin->w_cursor.col; s > ಸಾಲು; s = n)
			{
			    n = mb_prevptr(ಸಾಲು, s);
			    ಯದಿ (!vim_iswordp(n))
				ಭಙ್ಗ;
			}
		    }
		    ಅನ್ಯಥಾ
			ಪ್ರತಿ (s = ಸಾಲು + curwin->w_cursor.col; s > ಸಾಲು; --s)
			    ಯದಿ (!vim_iswordc(s[-1]))
				ಭಙ್ಗ;
		    ಯದಿ (s + (p - look) <= ಸಾಲು + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		ಅನ್ಯಥಾ
		    // TODO: multi-byte
		    ಯದಿ (keytyped == (ಪೂರ್ಣಾಙ್ಕ)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((ಪೂರ್ಣಾಙ್ಕ)p[-1])))
		{
		    ಸಾಲು = ml_get_cursor();
		    ಯದಿ ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(ಸಾಲು[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(ಸಾಲು - (p - look), look, p - look)
				: STRNCMP(ಸಾಲು - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		ಯದಿ (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ಯದಿ (getwhitecols_curline() !=
				     (ಪೂರ್ಣಾಙ್ಕ)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		ಯದಿ (match)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (try_match && *look == keytyped)
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    ಯದಿ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    ಶೂನ್ಯ
do_c_ಅಭಿವ್ಯಕ್ತಿ_indent(ಶೂನ್ಯ)
{
#ifdef FEAT_EVAL
    ಯದಿ (*curbuf->b_p_inde != NUL)
	fixthisline(get_ಅಭಿವ್ಯಕ್ತಿ_indent);
    ಅನ್ಯಥಾ
#endif
	fixthisline(get_c_indent);
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    ಶೂನ್ಯ
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ಯದಿ (lnum >= 1 && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_ಸಂಖ್ಯೆ = get_c_indent();
	curwin->w_cursor = pos;
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}
#endif
