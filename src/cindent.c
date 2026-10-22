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

#অন্তর্ভাব "vim.h"

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
#define LOOKFOR_JS_চাবি		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    পূর্ণাঙ্ক
cin_is_cinword(char_u *সারি)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    পূর্ণাঙ্ক		cinw_len;
    পূর্ণাঙ্ক		retval = FALSE;
    পূর্ণাঙ্ক		len;

    cinw_len = (পূর্ণাঙ্ক)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    যদি (cinw_buf == রিক্ত)
	প্রত্যাগম FALSE;

    সারি = skipwhite(সারি);
    প্রতি (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	যদি (STRNCMP(সারি, cinw_buf, len) == 0
		&& (!vim_iswordc(সারি[len]) || !vim_iswordc(সারি[len - 1])))
	{
	    retval = TRUE;
	    ভঙ্গ;
	}
    }
    vim_free(cinw_buf);
    প্রত্যাগম retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    স্থির char_u *
skip_string(char_u *p)
{
    পূর্ণাঙ্ক	    i;

    // We loop, because strings may be concatenated: "date""time".
    প্রতি ( ; ; ++p)
    {
	যদি (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    যদি (p[1] == NUL)		    // ' at end of line
		ভঙ্গ;
	    i = 2;
	    যদি (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		যাবত্ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    যদি (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		অনুবর্ত;
	    }
	}
	অন্যথা যদি (p[0] == '"')		    // start of string
	{
	    প্রতি (++p; p[0]; ++p)
	    {
		যদি (p[0] == '\\' && p[1] != NUL)
		    ++p;
		অন্যথা যদি (p[0] == '"')	    // end of string
		    ভঙ্গ;
	    }
	    যদি (p[0] == '"')
		অনুবর্ত; // continue for another string
	}
	অন্যথা যদি (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    যদি (paren != রিক্ত)
	    {
		size_t delim_len = paren - delim;

		প্রতি (p += 3; *p; ++p)
		    যদি (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			ভঙ্গ;
		    }
		যদি (p[0] == '"')
		    অনুবর্ত; // continue for another string
	    }
	}
	ভঙ্গ;				    // no string found
    }
    যদি (!*p)
	--p;				    // backup from NUL
    প্রত্যাগম p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    পূর্ণাঙ্ক
is_pos_in_string(char_u *সারি, colnr_T col)
{
    char_u *p;

    প্রতি (p = সারি; *p && (colnr_T)(p - সারি) < col; ++p)
	p = skip_string(p);
    প্রত্যাগম !((colnr_T)(p - সারি) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    পূর্ণাঙ্ক
check_linecomment(char_u *সারি)
{
    char_u  *p;

    p = সারি;
    // skip Lispish one-line comments
    যদি (curbuf->b_p_lisp)
    {
	যদি (vim_strchr(p, ';') != রিক্ত) // there may be comments
	{
	    পূর্ণাঙ্ক in_str = FALSE;	// inside of string

	    p = সারি;		// scan from start
	    যাবত্ ((p = vim_strpbrk(p, (char_u *)"\";")) != রিক্ত)
	    {
		যদি (*p == '"')
		{
		    যদি (in_str)
		    {
			যদি (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    অন্যথা যদি (p == সারি || ((p - সারি) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		অন্যথা যদি (!in_str && ((p - সারি) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(সারি, (colnr_T)(p - সারি)))
		    ভঙ্গ;	// found!
		++p;
	    }
	}
	অন্যথা
	    p = রিক্ত;
    }
    অন্যথা
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	প্রতি ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    যদি (*p == NUL)
		ভঙ্গ;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    যদি (p[0] == '/' && p[1] == '/'
				 && (p == সারি || p[-1] != '*' || p[2] != '*'))
		ভঙ্গ;
	}
	যদি (*p == NUL)
	    p = রিক্ত;
    }

    যদি (p == রিক্ত)
	প্রত্যাগম MAXCOL;
    প্রত্যাগম (পূর্ণাঙ্ক)(p - সারি);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    স্থির pos_T *
ind_find_start_comment(শূন্য)	    // XXX
{
    প্রত্যাগম find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(পূর্ণাঙ্ক ind_maxcomment)	// XXX
{
    pos_T	*pos;
    পূর্ণাঙ্ক		cur_maxcomment = ind_maxcomment;

    প্রতি (;;)
    {
	pos = findmatchlimit(রিক্ত, '*', FM_BACKWARD, cur_maxcomment);
	যদি (pos == রিক্ত)
	    ভঙ্গ;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	যদি (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ভঙ্গ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	যদি (cur_maxcomment <= 0)
	{
	    pos = রিক্ত;
	    ভঙ্গ;
	}
    }
    প্রত্যাগম pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    স্থির pos_T *
find_start_rawstring(পূর্ণাঙ্ক ind_maxcomment)	// XXX
{
    pos_T	*pos;
    পূর্ণাঙ্ক		cur_maxcomment = ind_maxcomment;

    প্রতি (;;)
    {
	pos = findmatchlimit(রিক্ত, 'R', FM_BACKWARD, cur_maxcomment);
	যদি (pos == রিক্ত)
	    ভঙ্গ;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	যদি (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    ভঙ্গ;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	যদি (cur_maxcomment <= 0)
	{
	    pos = রিক্ত;
	    ভঙ্গ;
	}
    }
    প্রত্যাগম pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    স্থির pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    স্থির pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    যদি (comment_pos != রিক্ত)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    যদি (comment_pos == রিক্ত || (rs_pos != রিক্ত
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	যদি (is_raw != রিক্ত && rs_pos != রিক্ত)
	    *is_raw = rs_pos->lnum;
	প্রত্যাগম rs_pos;
    }
    প্রত্যাগম comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    পূর্ণাঙ্ক
cindent_on(শূন্য)
{
    প্রত্যাগম (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    স্থির char_u *
cin_skipcomment(char_u *s)
{
    যাবত্ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	যদি (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    ভঙ্গ;
	}
	যদি (*s != '/')
	    ভঙ্গ;
	++s;
	যদি (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    ভঙ্গ;
	}
	যদি (*s != '*')
	    ভঙ্গ;
	প্রতি (++s; *s; ++s)	// skip slash-star comment
	    যদি (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		ভঙ্গ;
	    }
    }
    প্রত্যাগম s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    স্থির পূর্ণাঙ্ক
cin_nocode(char_u *s)
{
    প্রত্যাগম *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    স্থির পূর্ণাঙ্ক
cin_iscomment(char_u *p)
{
    প্রত্যাগম (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    স্থির পূর্ণাঙ্ক
cin_islinecomment(char_u *p)
{
    প্রত্যাগম (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    স্থির pos_T *
find_সারি_comment(শূন্য) // XXX
{
    স্থির pos_T pos;
    char_u	 *সারি;
    char_u	 *p;

    pos = curwin->w_cursor;
    যাবত্ (--pos.lnum > 0)
    {
	সারি = ml_get(pos.lnum);
	p = skipwhite(সারি);
	যদি (cin_islinecomment(p))
	{
	    pos.col = (পূর্ণাঙ্ক)(p - সারি);
	    প্রত্যাগম &pos;
	}
	যদি (*p != NUL)
	    ভঙ্গ;
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    স্থির পূর্ণাঙ্ক
cin_has_js_চাবি(char_u *text)
{
    char_u *s = skipwhite(text);
    পূর্ণাঙ্ক	    quote = -1;

    যদি (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    যদি (!vim_isIDc(*s))	    // need at least one ID character
	প্রত্যাগম FALSE;

    যাবত্ (vim_isIDc(*s))
	++s;
    যদি (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    প্রত্যাগম (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    স্থির পূর্ণাঙ্ক
cin_islabel_skip(char_u **s)
{
    যদি (!vim_isIDc(**s))	    // need at least one ID character
	প্রত্যাগম FALSE;

    যাবত্ (vim_isIDc(**s))
    {
	যদি (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	অন্যথা
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    প্রত্যাগম (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    স্থির পূর্ণাঙ্ক
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    পূর্ণাঙ্ক	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    যদি (cinsd_buf == রিক্ত)
	প্রত্যাগম FALSE;

    প্রতি (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (পূর্ণাঙ্ক)cinsd_len, ",");
	যদি (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    যদি (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		ভঙ্গ;
	    }
	}
    }

    vim_free(cinsd_buf);
    প্রত্যাগম found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    স্থির পূর্ণাঙ্ক
cin_ispreproc(char_u *s)
{
    যদি (*skipwhite(s) == '#')
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    স্থির পূর্ণাঙ্ক
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, পূর্ণাঙ্ক *amount)
{
    char_u	*সারি = *pp;
    linenr_T	lnum = *lnump;
    পূর্ণাঙ্ক		retval = FALSE;
    পূর্ণাঙ্ক		candidate_amount = *amount;

    যদি (*সারি != NUL && সারি[STRLEN(সারি) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    প্রতি (;;)
    {
	যদি (cin_ispreproc(সারি))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    ভঙ্গ;
	}
	যদি (lnum == 1)
	    ভঙ্গ;
	সারি = ml_get(--lnum);
	যদি (*সারি == NUL || সারি[STRLEN(সারি) - 1] != '\\')
	    ভঙ্গ;
    }

    যদি (lnum != *lnump)
	*pp = ml_get(*lnump);
    যদি (retval)
	*amount = candidate_amount;
    প্রত্যাগম retval;
}

    স্থির পূর্ণাঙ্ক
cin_iselse(
    char_u  *p)
{
    যদি (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    প্রত্যাগম (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    স্থির পূর্ণাঙ্ক
cin_isterminated(
    char_u	*s,
    পূর্ণাঙ্ক		incl_open,	// include '{' at the end as terminator
    পূর্ণাঙ্ক		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    পূর্ণাঙ্ক		is_else = FALSE;

    s = cin_skipcomment(s);

    যদি (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    যদি (!found_start)
	is_else = cin_iselse(s);

    যাবত্ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	যদি (*s == '}' && n_open > 0)
	    --n_open;
	যদি ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    প্রত্যাগম *s;
	অন্যথা যদি (*s == '{')
	{
	    যদি (incl_open && cin_nocode(s + 1))
		প্রত্যাগম *s;
	    অন্যথা
		++n_open;
	}

	যদি (*s)
	    s++;
    }
    প্রত্যাগম found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    স্থির পূর্ণাঙ্ক
cin_starts_with(char_u *s, বর্ণ *word)
{
    পূর্ণাঙ্ক l = (পূর্ণাঙ্ক)STRLEN(word);

    প্রত্যাগম (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    স্থির পূর্ণাঙ্ক
cin_isdefault(char_u *s)
{
    প্রত্যাগম (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    স্থির পূর্ণাঙ্ক
cin_iscase(
    char_u *s,
    পূর্ণাঙ্ক strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    যদি (cin_starts_with(s, "case"))
    {
	প্রতি (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    যদি (*s == NUL)
		ভঙ্গ;
	    যদি (*s == ':')
	    {
		যদি (s[1] == ':')	// skip over "::" for C++
		    ++s;
		অন্যথা
		    প্রত্যাগম TRUE;
	    }
	    যদি (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    অন্যথা যদি (*s == '/' && (s[1] == '*' || s[1] == '/'))
		প্রত্যাগম FALSE;		// stop at comment
	    অন্যথা যদি (*s == '"')
	    {
		// JS etc.
		যদি (strict)
		    প্রত্যাগম FALSE;		// stop at string
		অন্যথা
		    প্রত্যাগম TRUE;
	    }
	}
	প্রত্যাগম FALSE;
    }

    যদি (cin_isdefault(s))
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    স্থির পূর্ণাঙ্ক
cin_islabel(শূন্য)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    যদি (cin_isdefault(s))
	প্রত্যাগম FALSE;
    যদি (cin_isscopedecl(s))
	প্রত্যাগম FALSE;

    যদি (!cin_islabel_skip(&s))
	প্রত্যাগম FALSE;

    যদি (ind_find_start_CORS(রিক্ত))
	প্রত্যাগম FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*সারি;

    cursor_save = curwin->w_cursor;
    যাবত্ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	যদি ((trypos = ind_find_start_CORS(রিক্ত)) != রিক্ত) // XXX
	    curwin->w_cursor = *trypos;

	সারি = ml_get_curline();
	যদি (cin_ispreproc(সারি))	// ignore #defines, #if, etc.
	    অনুবর্ত;
	যদি (*(সারি = cin_skipcomment(সারি)) == NUL)
	    অনুবর্ত;

	curwin->w_cursor = cursor_save;
	যদি (cin_isterminated(সারি, TRUE, FALSE)
		|| cin_isscopedecl(সারি)
		|| cin_iscase(সারি, TRUE)
		|| (cin_islabel_skip(&সারি) && cin_nocode(সারি)))
	    প্রত্যাগম TRUE;
	প্রত্যাগম FALSE;
    }
    curwin->w_cursor = cursor_save;
    প্রত্যাগম TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    স্থির পূর্ণাঙ্ক
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(find);

    যাবত্ (*p != NUL)
    {
	p = cin_skipcomment(p);
	যদি (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    যদি (cin_nocode(r))
		প্রত্যাগম TRUE;
	}
	যদি (*p != NUL)
	    ++p;
    }
    প্রত্যাগম FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    স্থির char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = রিক্ত, *p = s;
    কুরু
    {
	r = p;
	p = cin_skipcomment(p);
	যদি (*p)
	    p = skip_string(p);
    } যাবত্ (p != r);
    প্রত্যাগম p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    স্থির পূর্ণাঙ্ক
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = রিক্ত;

    যাবত্ (*p)
    {
	যদি (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	অন্যথা যদি (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	অন্যথা
	    p = cin_skip_comment_and_string(p + 1);
    }
    যদি (!r)
	প্রত্যাগম FALSE;
    p = r; // p points now after '=' or "return"

    যদি (cin_nocode(p))
	প্রত্যাগম TRUE;

    যদি (*p == '&')
	p = cin_skipcomment(p + 1);

    যদি (*p == '(') // skip a typecast
    {
	পূর্ণাঙ্ক open_গুনতি = 1;
	কুরু
	{
	    p = cin_skip_comment_and_string(p + 1);
	    যদি (cin_nocode(p))
		প্রত্যাগম TRUE;
	    open_গুনতি += (*p == '(') - (*p == ')');
	} যাবত্ (open_গুনতি);
	p = cin_skipcomment(p + 1);
	যদি (cin_nocode(p))
	    প্রত্যাগম TRUE;
    }

    যাবত্ (*p == '{')
	p = cin_skipcomment(p + 1);
    প্রত্যাগম cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    স্থির পূর্ণাঙ্ক
cin_isinit(শূন্য)
{
    char_u	*s;
    স্থির বর্ণ *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    যদি (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    প্রতি (;;)
    {
	পূর্ণাঙ্ক i, l;

	প্রতি (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(skip); ++i)
	{
	    l = (পূর্ণাঙ্ক)strlen(skip[i]);
	    যদি (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		ভঙ্গ;
	    }
	}
	যদি (l != 0)
	    ভঙ্গ;
    }

    যদি (cin_starts_with(s, "enum"))
	প্রত্যাগম TRUE;

    প্রত্যাগম cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    স্থির পূর্ণাঙ্ক
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    পূর্ণাঙ্ক		has_name = FALSE;
    পূর্ণাঙ্ক		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    যাবত্ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    যদি (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	যাবত্ (*p != NUL)
	{
	    যদি (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    অন্যথা যদি (*p == '{')
	    {
		ভঙ্গ;
	    }
	    অন্যথা যদি (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		যদি (has_name)
		    প্রত্যাগম FALSE; // word character after skipping past name
		++p;
	    }
	    অন্যথা যদি (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		যদি (!has_name_start || has_name)
		    প্রত্যাগম FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    অন্যথা
	    {
		প্রত্যাগম FALSE;
	    }
	}
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    স্থির পূর্ণাঙ্ক
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    পূর্ণাঙ্ক		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    যদি (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	যাবত্ (*p != NUL)
	{
	    যদি (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    অন্যথা যদি (*p == '{')
	    {
		ভঙ্গ;
	    }
	    অন্যথা যদি (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		যদি (has_string_literal)
		    প্রত্যাগম FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    অন্যথা যদি (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		যদি (has_string_literal)
		    প্রত্যাগম FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    অন্যথা
	    {
		প্রত্যাগম FALSE;
	    }
	}
	প্রত্যাগম has_string_literal ? TRUE : FALSE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    স্থির char_u *
after_label(char_u *l)
{
    প্রতি ( ; *l; ++l)
    {
	যদি (*l == ':')
	{
	    যদি (l[1] == ':')	    // skip over "::" for C++
		++l;
	    অন্যথা যদি (!cin_iscase(l + 1, FALSE))
		ভঙ্গ;
	}
	অন্যথা যদি (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    যদি (*l == NUL)
	প্রত্যাগম রিক্ত;
    l = cin_skipcomment(l + 1);
    যদি (*l == NUL)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    স্থির পূর্ণাঙ্ক
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    যদি (p == রিক্ত)
	প্রত্যাগম 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, রিক্ত, রিক্ত, 0);
    প্রত্যাগম (পূর্ণাঙ্ক)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    স্থির পূর্ণাঙ্ক
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    পূর্ণাঙ্ক		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    যদি (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	যদি (l == রিক্ত)		// just in case
	    l = ml_get_curline();
    }
    অন্যথা
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    প্রত্যাগম amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    স্থির পূর্ণাঙ্ক
cin_first_id_amount(শূন্য)
{
    char_u	*সারি, *p, *s;
    পূর্ণাঙ্ক		len;
    pos_T	fp;
    colnr_T	col;

    সারি = ml_get_curline();
    p = skipwhite(সারি);
    len = (পূর্ণাঙ্ক)(skiptowhite(p) - p);
    যদি (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (পূর্ণাঙ্ক)(skiptowhite(p) - p);
    }
    যদি (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    অন্যথা যদি (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    অন্যথা যদি ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	যদি ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    প্রতি (len = 0; vim_isIDc(p[len]); ++len)
	;
    যদি (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	প্রত্যাগম 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - সারি);
    getvcol(curwin, &fp, &col, রিক্ত, রিক্ত, 0);
    প্রত্যাগম (পূর্ণাঙ্ক)col;
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
    স্থির পূর্ণাঙ্ক
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*সারি;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    যদি (lnum > 1)
    {
	সারি = ml_get(lnum - 1);
	যদি (*সারি != NUL && সারি[STRLEN(সারি) - 1] == '\\')
	    প্রত্যাগম -1;
    }

    সারি = s = ml_get(lnum);
    যাবত্ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == রিক্ত)
    {
	যদি (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	অন্যথা
	    ++s;
    }
    যদি (*s != '=')
	প্রত্যাগম 0;

    s = skipwhite(s + 1);
    যদি (cin_nocode(s))
	প্রত্যাগম 0;

    যদি (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - সারি);
    getvcol(curwin, &fp, &col, রিক্ত, রিক্ত, 0);
    প্রত্যাগম (পূর্ণাঙ্ক)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    স্থির পূর্ণাঙ্ক
cin_skip2pos(pos_T *trypos)
{
    char_u	*সারি;
    char_u	*p;
    char_u	*new_p;

    p = সারি = ml_get(trypos->lnum);
    যাবত্ (*p && (colnr_T)(p - সারি) < trypos->col)
    {
	যদি (cin_iscomment(p))
	    p = cin_skipcomment(p);
	অন্যথা
	{
	    new_p = skip_string(p);
	    যদি (new_p == p)
		++p;
	    অন্যথা
		p = new_p;
	}
    }
    প্রত্যাগম (পূর্ণাঙ্ক)(p - সারি);
}

    স্থির pos_T *
find_match_char(পূর্ণাঙ্ক c, পূর্ণাঙ্ক ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    স্থির pos_T pos_copy;
    পূর্ণাঙ্ক		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    যদি ((trypos = findmatchlimit(রিক্ত, c, FM_SKIPCOMM, ind_maxp_wk)) != রিক্ত)
    {
	// check if the ( is in a // comment
	যদি ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (পূর্ণাঙ্ক)(cursor_save.lnum - trypos->lnum);
	    যদি (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		গচ্ছ retry;
	    }
	    trypos = রিক্ত;
	}
	অন্যথা
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    যদি ((trypos_wk = ind_find_start_CORS(রিক্ত)) != রিক্ত) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (পূর্ণাঙ্ক)(cursor_save.lnum
			- trypos_wk->lnum);
		যদি (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    গচ্ছ retry;
		}
		trypos = রিক্ত;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    প্রত্যাগম trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    স্থির pos_T *
find_match_paren(পূর্ণাঙ্ক ind_maxparen)	// XXX
{
    প্রত্যাগম find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    স্থির পূর্ণাঙ্ক
find_last_paren(char_u *l, পূর্ণাঙ্ক start, পূর্ণাঙ্ক end)
{
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		retval = FALSE;
    পূর্ণাঙ্ক		open_গুনতি = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    প্রতি (i = 0; l[i] != NUL; i++)
    {
	i = (পূর্ণাঙ্ক)(cin_skipcomment(l + i) - l); // ignore parens in comments
	যদি (l[i] == NUL)
	    ভঙ্গ;
	i = (পূর্ণাঙ্ক)(skip_string(l + i) - l);    // ignore parens in quotes
	যদি (l[i] == start)
	    ++open_গুনতি;
	অন্যথা যদি (l[i] == end)
	{
	    যদি (open_গুনতি > 0)
		--open_গুনতি;
	    অন্যথা
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    প্রত্যাগম retval;
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
    স্থির পূর্ণাঙ্ক
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    পূর্ণাঙ্ক		retval = FALSE;
    pos_T	*trypos;
    পূর্ণাঙ্ক		just_started = TRUE;

    যদি (sp == রিক্ত)
	s = ml_get(lnum);
    অন্যথা
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    যদি (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != রিক্ত)
    {
	lnum = trypos->lnum;
	যদি (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    প্রত্যাগম FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    যদি (cin_ispreproc(s))
	প্রত্যাগম FALSE;

    যাবত্ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	যদি (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	অন্যথা যদি (*s == ':')
	{
	    যদি (*(s + 1) == ':')
		s += 2;
	    অন্যথা
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		প্রত্যাগম FALSE;
	}
	অন্যথা
	    ++s;
    }
    যদি (*s != '(')
	প্রত্যাগম FALSE;		// ';', ' or "  before any () or no '('

    যাবত্ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	যদি (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    যদি (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    গচ্ছ done;
	}
	যদি ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    পূর্ণাঙ্ক comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    প্রতি (;;)
	    {
		যদি (lnum >= curbuf->b_ml.ml_সারি_গুনতি)
		    ভঙ্গ;
		s = ml_get(++lnum);
		যদি (!cin_ispreproc(s))
		    ভঙ্গ;
	    }
	    যদি (lnum >= curbuf->b_ml.ml_সারি_গুনতি)
		ভঙ্গ;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    যদি (!just_started && (!comma && *s != ',' && *s != ')'))
		ভঙ্গ;
	    just_started = FALSE;
	}
	অন্যথা যদি (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	অন্যথা
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    যদি (lnum != first_lnum && sp != রিক্ত)
	*sp = ml_get(first_lnum);

    প্রত্যাগম retval;
}

    স্থির পূর্ণাঙ্ক
cin_isif(char_u *p)
{
    প্রত্যাগম (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    স্থির পূর্ণাঙ্ক
cin_isdo(char_u *p)
{
    প্রত্যাগম (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    স্থির পূর্ণাঙ্ক
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    পূর্ণাঙ্ক		retval = FALSE;

    p = cin_skipcomment(p);
    যদি (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    যদি (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	যাবত্ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	যদি ((trypos = findmatchlimit(রিক্ত, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != রিক্ত
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    প্রত্যাগম retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    স্থির পূর্ণাঙ্ক
cin_is_if_for_while_before_offset(char_u *সারি, পূর্ণাঙ্ক *poffset)
{
    পূর্ণাঙ্ক offset = *poffset;

    যদি (offset-- < 2)
	প্রত্যাগম 0;
    যাবত্ (offset > 2 && VIM_ISWHITE(সারি[offset]))
	--offset;

    offset -= 1;
    যদি (STRNCMP(সারি + offset, "if", 2) == 0)
	গচ্ছ probablyFound;

    যদি (offset >= 1)
    {
	offset -= 1;
	যদি (STRNCMP(সারি + offset, "for", 3) == 0)
	    গচ্ছ probablyFound;

	যদি (offset >= 2)
	{
	    offset -= 2;
	    যদি (STRNCMP(সারি + offset, "while", 5) == 0)
		গচ্ছ probablyFound;
	}
    }
    প্রত্যাগম 0;

probablyFound:
    যদি (!offset || !vim_isIDc(সারি[offset - 1]))
    {
	*poffset = offset;
	প্রত্যাগম 1;
    }
    প্রত্যাগম 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    স্থির পূর্ণাঙ্ক
cin_iswhileofdo_end(পূর্ণাঙ্ক terminated)
{
    char_u	*সারি;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    পূর্ণাঙ্ক		i;

    যদি (terminated != ';')	// there must be a ';' at the end
	প্রত্যাগম FALSE;

    p = সারি = ml_get_curline();
    যাবত্ (*p != NUL)
    {
	p = cin_skipcomment(p);
	যদি (*p == ')')
	{
	    s = skipwhite(p + 1);
	    যদি (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (পূর্ণাঙ্ক)(p - সারি);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		যদি (trypos != রিক্ত)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    যদি (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    যদি (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			প্রত্যাগম TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		সারি = ml_get_curline();
		p = সারি + i;
	    }
	}
	যদি (*p != NUL)
	    ++p;
    }
    প্রত্যাগম FALSE;
}

    স্থির পূর্ণাঙ্ক
cin_isbreak(char_u *p)
{
    প্রত্যাগম (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    স্থির পূর্ণাঙ্ক
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    পূর্ণাঙ্ক		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*সারি = ml_get_curline();

    যদি (pos->lnum <= lnum)
	প্রত্যাগম cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(সারি);
    যদি (*s == '#')		// skip #define FOO x ? (x) : x
	প্রত্যাগম FALSE;
    s = cin_skipcomment(s);
    যদি (*s == NUL)
	প্রত্যাগম FALSE;

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
    যাবত্ (lnum > 1)
    {
	সারি = ml_get(lnum - 1);
	s = skipwhite(সারি);
	যদি (*s == '#' || *s == NUL)
	    ভঙ্গ;
	যাবত্ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    যদি (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		ভঙ্গ;
	    যদি (*s != NUL)
		++s;
	}
	যদি (*s != NUL)
	    ভঙ্গ;
	--lnum;
    }

    pos->lnum = lnum;
    সারি = ml_get(lnum);
    s = সারি;
    প্রতি (;;)
    {
	যদি (*s == NUL)
	{
	    যদি (lnum == curwin->w_cursor.lnum)
		ভঙ্গ;
	    // Continue in the cursor line.
	    সারি = ml_get(++lnum);
	    s = সারি;
	}
	যদি (s == সারি)
	{
	    // don't recognize "case (foo):" as a baseclass
	    যদি (cin_iscase(s, FALSE))
		ভঙ্গ;
	    s = cin_skipcomment(সারি);
	    যদি (*s == NUL)
		অনুবর্ত;
	}

	যদি (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	অন্যথা যদি (s[0] == ':')
	{
	    যদি (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    অন্যথা যদি (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    অন্যথা
		s = cin_skipcomment(s + 1);
	}
	অন্যথা যদি ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    যদি (*s == 'c')
		s = cin_skipcomment(s + 5);
	    অন্যথা
		s = cin_skipcomment(s + 6);
	}
	অন্যথা
	{
	    যদি (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    অন্যথা যদি (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    অন্যথা যদি (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		প্রত্যাগম FALSE;
	    }
	    অন্যথা যদি (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    অন্যথা যদি (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		যদি (cpp_base_class)
		    pos->col = (colnr_T)(s - সারি);
	    }

	    // When the line ends in a comma don't align with it.
	    যদি (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    যদি (cpp_base_class)
	pos->lnum = lnum;
    প্রত্যাগম cpp_base_class;
}

    স্থির পূর্ণাঙ্ক
get_baseclass_amount(পূর্ণাঙ্ক col)
{
    পূর্ণাঙ্ক		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    যদি (col == 0)
    {
	amount = get_indent();
	যদি (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != রিক্ত)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	যদি (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    অন্যথা
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, রিক্ত, রিক্ত, 0);
	amount = (পূর্ণাঙ্ক)vcol;
    }
    যদি (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    প্রত্যাগম amount;
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

    স্থির pos_T *
find_start_brace(শূন্য)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    স্থির pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    যাবত্ ((trypos = findmatchlimit(রিক্ত, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != রিক্ত)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = রিক্ত;
	// ignore the { if it's in a // or / *  * / comment
	যদি ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(রিক্ত)) == রিক্ত) // XXX
	    ভঙ্গ;
	যদি (pos != রিক্ত)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    প্রত্যাগম trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    স্থির pos_T *
find_match_paren_after_brace (পূর্ণাঙ্ক ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    যদি (trypos == রিক্ত)
	প্রত্যাগম রিক্ত;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    যদি (tryposBrace != রিক্ত
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = রিক্ত;
    প্রত্যাগম trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    স্থির পূর্ণাঙ্ক
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    যদি (n > 0 && n < curbuf->b_ind_maxparen / 2)
	প্রত্যাগম curbuf->b_ind_maxparen - (পূর্ণাঙ্ক)n;
    প্রত্যাগম curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    শূন্য
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    পূর্ণাঙ্ক		divider;
    পূর্ণাঙ্ক		fraction = 0;
    পূর্ণাঙ্ক		sw;
    long	t = get_sw_মান(buf);

    // needed for cino-(, it will be multiplied by 2 again
    যদি (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    অন্যথা
	sw = (পূর্ণাঙ্ক)t;

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
    buf->b_ind_open_বাম_imag = 0;

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

    প্রতি (p = buf->b_p_cino; *p; )
    {
	l = p++;
	যদি (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	যদি (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((বর্ণ *)++p);
	    যাবত্ (VIM_ISDIGIT(*p))
	    {
		++p;
		যদি (divider)
		    divider *= 10;
		অন্যথা
		    divider = 10;
	    }
	}
	যদি (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    যদি (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    অন্যথা
	    {
		n *= sw;
		যদি (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	যদি (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	বিকল্প (*l)
	{
	    পক্ষ '>': buf->b_ind_level = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'e': buf->b_ind_open_imag = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'n': buf->b_ind_no_brace = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'f': buf->b_ind_first_open = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '{': buf->b_ind_open_extra = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '}': buf->b_ind_close_extra = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '^': buf->b_ind_open_বাম_imag = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'L': buf->b_ind_jump_label = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ ':': buf->b_ind_case = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '=': buf->b_ind_case_code = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'b': buf->b_ind_case_break = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'p': buf->b_ind_param = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 't': buf->b_ind_func_type = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '/': buf->b_ind_comment = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'c': buf->b_ind_in_comment = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'C': buf->b_ind_in_comment2 = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'i': buf->b_ind_cpp_baseclass = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '+': buf->b_ind_continuation = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '(': buf->b_ind_unclosed = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'u': buf->b_ind_unclosed2 = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'U': buf->b_ind_unclosed_noignore = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'W': buf->b_ind_unclosed_wrapped = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'w': buf->b_ind_unclosed_whiteok = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'm': buf->b_ind_matching_paren = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'M': buf->b_ind_paren_prev = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ ')': buf->b_ind_maxparen = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '*': buf->b_ind_maxcomment = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'g': buf->b_ind_scopedecl = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'h': buf->b_ind_scopedecl_code = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'j': buf->b_ind_java = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'J': buf->b_ind_js = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'l': buf->b_ind_keep_case_label = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ '#': buf->b_ind_hash_comment = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'N': buf->b_ind_cpp_namespace = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'k': buf->b_ind_if_for_while = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'E': buf->b_ind_cpp_extern_c = (পূর্ণাঙ্ক)n; ভঙ্গ;
	    পক্ষ 'P': buf->b_ind_pragma = (পূর্ণাঙ্ক)n; ভঙ্গ;
	}
	যদি (*p == ',')
	    ++p;
    }
}

    স্থির পূর্ণাঙ্ক
find_match(পূর্ণাঙ্ক lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    পূর্ণাঙ্ক		elselevel;
    পূর্ণাঙ্ক		whilelevel;

    যদি (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    অন্যথা
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    যাবত্ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	যদি (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    যদি (theirscope == রিক্ত)
		ভঙ্গ;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    যদি (theirscope->lnum < ourscope)
		ভঙ্গ;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    যদি (theirscope->lnum > ourscope)
		অনুবর্ত;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    যদি (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		যদি (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    যদি (!cin_isif(mightbeif))
			++elselevel;
		    অনুবর্ত;
		}

		// If it's an "if" decrement elselevel
		যদি (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    যদি (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    যদি (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		অনুবর্ত;
	    }

	    // If it's a "do" decrement whilelevel
	    যদি (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    যদি (elselevel <= 0 && whilelevel <= 0)
		প্রত্যাগম OK;
	}
    }
    প্রত্যাগম FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    পূর্ণাঙ্ক
get_c_indent(শূন্য)
{
    pos_T	cur_curpos;
    পূর্ণাঙ্ক		amount;
    পূর্ণাঙ্ক		scope_amount;
    পূর্ণাঙ্ক		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = রিক্ত;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    পূর্ণাঙ্ক		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    পূর্ণাঙ্ক		lookfor;
    পূর্ণাঙ্ক		whilelevel;
    linenr_T	lnum;
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		iscase;
    পূর্ণাঙ্ক		lookfor_break;
    পূর্ণাঙ্ক		lookfor_cpp_namespace = FALSE;
    পূর্ণাঙ্ক		cont_amount = 0;    // amount for continuation line
    পূর্ণাঙ্ক		original_সারি_islabel;
    পূর্ণাঙ্ক		added_to_amount = 0;
    পূর্ণাঙ্ক		js_cur_has_চাবি = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    পূর্ণাঙ্ক		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    যদি (cur_curpos.lnum == 1)
	প্রত্যাগম 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    যদি (linecopy == রিক্ত)
	প্রত্যাগম 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    যদি ((State & MODE_ঢোকাও)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_সারি_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    যদি (comment_pos != রিক্ত)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    যদি (trypos != রিক্ত && (comment_pos == রিক্ত
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	গচ্ছ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    যদি (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	যদি (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    গচ্ছ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    যদি (original_সারি_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	গচ্ছ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    যদি (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_সারি_comment(); // XXX
	যদি (trypos == রিক্ত && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    যদি (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	যদি (trypos  != রিক্ত)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, রিক্ত, রিক্ত, 0);
	    amount = col;
	    গচ্ছ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    যদি (!cin_iscomment(theline) && comment_pos != রিক্ত) // XXX
    {
	পূর্ণাঙ্ক	lead_start_len = 2;
	পূর্ণাঙ্ক	lead_মধ্য_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_মধ্য[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	পূর্ণাঙ্ক	lead_end_len;
	char_u	*p;
	পূর্ণাঙ্ক	start_align = 0;
	পূর্ণাঙ্ক	start_off = 0;
	পূর্ণাঙ্ক	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, রিক্ত, রিক্ত, 0);
	amount = col;
	*lead_start = NUL;
	*lead_মধ্য = NUL;

	p = curbuf->b_p_com;
	যাবত্ (*p != NUL)
	{
	    পূর্ণাঙ্ক	align = 0;
	    পূর্ণাঙ্ক	off = 0;
	    পূর্ণাঙ্ক what = 0;

	    যাবত্ (*p != NUL && *p != ':')
	    {
		যদি (*p == COM_START || *p == COM_END || *p == COM_মধ্য)
		    what = *p++;
		অন্যথা যদি (*p == COM_বাম || *p == COM_ডান)
		    align = *p++;
		অন্যথা যদি (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		অন্যথা
		    ++p;
	    }

	    যদি (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    যদি (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    অন্যথা যদি (what == COM_মধ্য)
	    {
		STRCPY(lead_মধ্য, lead_end);
		lead_মধ্য_len = lead_end_len;
	    }
	    অন্যথা যদি (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		যদি (STRNCMP(theline, lead_মধ্য, lead_মধ্য_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    যদি (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			যদি (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			অন্যথা যদি (STRNCMP(look, lead_মধ্য,
							lead_মধ্য_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    ভঙ্গ;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			অন্যথা যদি (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    অনুবর্ত;
		    }
		    যদি (start_off != 0)
			amount += start_off;
		    অন্যথা যদি (start_align == COM_ডান)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_মধ্য);
		    ভঙ্গ;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		যদি (STRNCMP(theline, lead_মধ্য, lead_মধ্য_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    যদি (off != 0)
			amount += off;
		    অন্যথা যদি (align == COM_ডান)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_মধ্য);
		    done = TRUE;
		    ভঙ্গ;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	যদি (done)
	    ;
	অন্যথা যদি (theline[0] == '*')
	    amount += 1;
	অন্যথা
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    প্রতি (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		যদি (linewhite(lnum))		    // skip blank lines
		    অনুবর্ত;
		amount = get_indent_lnum(lnum);	    // XXX
		ভঙ্গ;
	    }
	    যদি (amount == -1)			    // use the comment opener
	    {
		যদি (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    যদি (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, রিক্ত, রিক্ত, 0);
		amount = col;
		যদি (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	গচ্ছ theend;
    }

    // Are we looking at a ']' that has a match?
    যদি (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != রিক্ত)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	গচ্ছ theend;
    }

    // Are we inside parentheses or braces?  XXX
    যদি (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != রিক্ত
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != রিক্ত
	    || trypos != রিক্ত)
    {
      যদি (trypos != রিক্ত && tryposBrace != রিক্ত)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  যদি (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = রিক্ত;
	  অন্যথা
	      tryposBrace = রিক্ত;
      }

      যদি (trypos != রিক্ত)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	যদি (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	অন্যথা
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    প্রতি (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		যদি (cin_nocode(l))		// skip comment lines
		    অনুবর্ত;
		যদি (cin_ispreproc_cont(&l, &lnum, &amount))
		    অনুবর্ত;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		যদি ((trypos = ind_find_start_CORS(রিক্ত)) != রিক্ত)
		{
		    lnum = trypos->lnum + 1;
		    অনুবর্ত;
		}

		// XXX
		যদি ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != রিক্ত
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    যদি (theline[0] == ')')
		    {
			যদি (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    ভঙ্গ;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	যদি (amount == -1)
	{
	    পূর্ণাঙ্ক	    ignore_paren_col = 0;
	    পূর্ণাঙ্ক	    is_if_for_while = 0;

	    যদি (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *সারি;

		trypos = &our_paren_pos;
		কুরু
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} যাবত্ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		সারি = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(সারি, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    যদি (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *সারি;
		পূর্ণাঙ্ক	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		সারি = ml_get_curline();
		look_col = (পূর্ণাঙ্ক)(look - সারি);
		curwin->w_cursor.col = look_col + 1;
		যদি ((trypos = findmatchlimit(রিক্ত, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != রিক্ত
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    যদি (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		যদি (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    যদি (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			প্রতি (col = 0; col < our_paren_pos.col; ++col)
			{
			    বিকল্প (l[col])
			    {
				পক্ষ '(':
				পক্ষ '{': ++n;
					  ভঙ্গ;

				পক্ষ ')':
				পক্ষ '}': যদি (n > 1)
					      --n;
					  ভঙ্গ;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    অন্যথা যদি (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    অন্যথা
		    {
			col = our_paren_pos.col + 1;
			যাবত্ (VIM_ISWHITE(l[col]))
			    col++;
			যদি (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			অন্যথা
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		যদি (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, রিক্ত, রিক্ত, 0);
		    যদি (cur_amount > (পূর্ণাঙ্ক)col)
			cur_amount = col;
		}
	    }

	    যদি (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    অন্যথা যদি ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		যদি (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    অন্যথা
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		যাবত্ ((পূর্ণাঙ্ক)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    বিকল্প (*ml_get_pos(&our_paren_pos))
		    {
			পক্ষ '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  ভঙ্গ;
			পক্ষ ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  ভঙ্গ;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		যদি (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		অন্যথা
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    যদি (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != রিক্ত)
			amount += curbuf->b_ind_unclosed2;
		    অন্যথা
		    {
			যদি (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			অন্যথা
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		যদি (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	যদি (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      অন্যথা
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
	যদি (*look == '{')
	{
	    getvcol(curwin, trypos, &col, রিক্ত, রিক্ত, 0);
	    amount = col;
	    যদি (*start == '{')
		start_brace = BRACE_IN_COL0;
	    অন্যথা
		start_brace = BRACE_AT_START;
	}
	অন্যথা
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    যদি (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != রিক্ত)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    যদি ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    অন্যথা যদি (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    অন্যথা
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	যদি (curbuf->b_ind_js)
	    js_cur_has_চাবি = cin_has_js_চাবি(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	যদি (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	অন্যথা
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    যদি (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    অন্যথা যদি (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    যদি (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		যদি (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    গচ্ছ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    যদি (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_বাম_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    অন্যথা যদি (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    অন্যথা
	    {
		যদি (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    যদি (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    অন্যথা যদি (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		অন্যথা
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    যদি (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    যদি (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    অন্যথা যদি (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    অন্যথা
	    {
		যদি (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    প্রতি (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		যদি (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    যদি (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			যদি (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    যদি (cont_amount > 0)
				amount = cont_amount;
			    অন্যথা যদি (!curbuf->b_ind_js)
				amount += ind_continuation;
			    ভঙ্গ;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(রিক্ত);
			যদি (trypos != রিক্ত)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    অনুবর্ত;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			যদি (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    অনুবর্ত;

			যদি (cin_nocode(l))
			    অনুবর্ত;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			যদি (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    যদি (terminated == ',')
				ভঙ্গ;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    যদি (terminated != ';' && cin_isinit())
				ভঙ্গ;

			    // nothing useful found
			    যদি (terminated == 0 || terminated == '{')
				অনুবর্ত;
			}

			যদি (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = রিক্ত;
			    যদি (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    যদি (trypos == রিক্ত && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    যদি (trypos != রিক্ত)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				অনুবর্ত;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
		    }
		    অন্যথা যদি (lookfor == LOOKFOR_UNTERM)
		    {
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
		    }
		    অন্যথা
		    {
			যদি (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    যদি (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			যদি (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    যদি (curwin->w_cursor.lnum == ourscope)
				অনুবর্ত;

			    যদি (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				ভঙ্গ;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(রিক্ত);
			    যদি (trypos != রিক্ত)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				অনুবর্ত;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    যদি (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				অনুবর্ত;

			    // Finally the actual check for "namespace".
			    যদি (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				ভঙ্গ;
			    }
			    অন্যথা যদি (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				ভঙ্গ;
			    }

			    যদি (cin_nocode(l))
				অনুবর্ত;
			}
		    }
		    ভঙ্গ;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		যদি ((trypos = ind_find_start_CORS(&raw_string_start)) != রিক্ত)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    অনুবর্ত;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		যদি (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    যদি (lookfor == LOOKFOR_CPP_BASECLASS)
			ভঙ্গ;

		    // When looking for a "do" we are not interested in
		    // labels.
		    যদি (whilelevel > 0)
			অনুবর্ত;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    যদি (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
			ভঙ্গ;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    যদি (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			যদি ((trypos = find_start_brace()) == রিক্ত
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    ভঙ্গ;
			}
			অনুবর্ত;
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
		    যদি (lookfor == LOOKFOR_TERM)
		    {
			যদি (n)
			    amount = n;

			যদি (!lookfor_break)
			    ভঙ্গ;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    যদি (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			যদি (l != রিক্ত && cin_is_cinword(l))
			{
			    যদি (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    অন্যথা
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			ভঙ্গ;
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
		    অনুবর্ত;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		যদি (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    যদি (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != রিক্ত)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    অনুবর্ত;
		}

		// Ignore jump labels with nothing after them.
		যদি (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    যদি (l == রিক্ত || cin_nocode(l))
			অনুবর্ত;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		যদি (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    অনুবর্ত;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		যদি (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		যদি (n)
		{
		    যদি (lookfor == LOOKFOR_UNTERM)
		    {
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
		    }
		    অন্যথা যদি (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			অনুবর্ত;
		    }
		    অন্যথা
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    ভঙ্গ;
		}
		অন্যথা যদি (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    যদি (cin_isterminated(l, TRUE, FALSE))
			ভঙ্গ;
		    অন্যথা
			অনুবর্ত;
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

		যদি (js_cur_has_চাবি)
		{
		    js_cur_has_চাবি = 0; // only check the first line
		    যদি (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_চাবি;
		    }
		}
		যদি (lookfor == LOOKFOR_JS_চাবি && cin_has_js_চাবি(l))
		{
		    amount = get_indent();
		    ভঙ্গ;
		}
		যদি (lookfor == LOOKFOR_COMMA)
		{
		    যদি (tryposBrace != রিক্ত && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			ভঙ্গ;
		    যদি (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			ভঙ্গ;
		    অন্যথা
		    {
			amount = get_indent();
			যদি (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    ভঙ্গ;
		    }
		}

		যদি (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    যদি (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (শূন্য)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    যদি (trypos != রিক্ত && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = রিক্ত;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    যদি (trypos == রিক্ত && terminated == ',')
		    {
			যদি (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    যদি (trypos != রিক্ত)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			যদি (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    অনুবর্ত;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    যদি (terminated == ',')
		    {
			যাবত্ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    যদি (*l == NUL || l[STRLEN(l) - 1] != '\\')
				ভঙ্গ;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    যদি (curbuf->b_ind_js)
			cur_amount = get_indent();
		    অন্যথা
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    যদি (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			যদি (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			যদি (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    অনুবর্ত;
			}
			ভঙ্গ;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    যদি (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			যদি (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    যদি (cont_amount > 0)
				amount = cont_amount;
			    অন্যথা
				amount += ind_continuation;
			    ভঙ্গ;
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
			যদি (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			যদি (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    ভঙ্গ;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			যদি (cin_isdo(l))
			{
			    যদি (whilelevel == 0)
				ভঙ্গ;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			যদি (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    যদি (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    যদি ((trypos = find_start_brace()) == রিক্ত
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ভঙ্গ;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    অন্যথা
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			যদি (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    যদি (terminated == ',')
				amount += ind_continuation;
			    ভঙ্গ;
			}

			যদি (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    যদি (terminated == ',')
			    {
				যদি (curbuf->b_ind_cpp_baseclass == 0)
				    ভঙ্গ;

				lookfor = LOOKFOR_CPP_BASECLASS;
				অনুবর্ত;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    যদি (amount > cur_amount)
				amount = cur_amount;
			}
			অন্যথা
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (পূর্ণাঙ্ক)STRLEN(l);
			    যদি (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				ভঙ্গ;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    যদি (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				যদি (curbuf->b_ind_js)
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
				    যদি (cin_iscomment(skipwhite(l)))
					ভঙ্গ;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    যদি (trypos != রিক্ত)
				    {
					যদি (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    ভঙ্গ;
					}
					ourscope = trypos->lnum;
				    }
				}
				অন্যথা
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    অন্যথা
			    {
				যদি (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				যদি (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_চাবি
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		অন্যথা যদি (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    যদি (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
			ভঙ্গ;
		    }

		    যদি (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			যদি (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		অন্যথা
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    যদি (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			অনুবর্ত;
		    }

		    // Handle "do {" line.
		    যদি (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			যদি (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    অনুবর্ত;
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
		    যদি (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			যদি (cont_amount > 0)
			    amount = cont_amount;
			অন্যথা
			    amount += ind_continuation;
			ভঙ্গ;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    যদি (lookfor == LOOKFOR_TERM)
		    {
			যদি (!lookfor_break && whilelevel == 0)
			    ভঙ্গ;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    অন্যথা
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			যদি (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != রিক্ত)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    যদি (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				অনুবর্ত;
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

			যদি (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			যদি (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			যদি (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    যদি ((trypos = find_start_brace()) == রিক্ত
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				ভঙ্গ;
			    অনুবর্ত;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			যদি (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != রিক্ত)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    যদি (*l == '}' || !cin_iselse(l))
				গচ্ছ term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      যদি (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      যদি (curbuf->b_ind_jump_label > 0 && original_সারি_islabel)
	  amount -= curbuf->b_ind_jump_label;

      গচ্ছ theend;
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

    যদি (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	গচ্ছ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    যদি (cur_curpos.lnum < curbuf->b_ml.ml_সারি_গুনতি
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == রিক্ত
	    && vim_strchr(theline, '}') == রিক্ত
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(রিক্ত, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	গচ্ছ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    যাবত্ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	যদি ((trypos = ind_find_start_CORS(রিক্ত)) != রিক্ত)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    অনুবর্ত;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	যদি (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	যদি (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    ভঙ্গ;
	}

	// Skip preprocessor directives and blank lines.
	যদি (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    অনুবর্ত;

	যদি (cin_nocode(l))
	    অনুবর্ত;

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
	যদি (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    যদি (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != রিক্ত)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    যাবত্ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		যদি (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    ভঙ্গ;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    যদি (amount == 0)
		amount = cin_first_id_amount();
	    যদি (amount == 0)
		amount = ind_continuation;
	    ভঙ্গ;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	যদি (cin_isfuncdecl(রিক্ত, cur_curpos.lnum, 0))  // XXX
	    ভঙ্গ;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	যদি (*skipwhite(l) == '}')
	    ভঙ্গ;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	যদি (cin_ends_in(l, (char_u *)"};"))
	    ভঙ্গ;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	যদি (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    ভঙ্গ;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	যদি (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    যাবত্ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		যদি (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    ভঙ্গ;
	    }
	    যদি (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		ভঙ্গ;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	যদি (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    ভঙ্গ;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	যদি (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    যদি (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		ভঙ্গ;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	যদি ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != রিক্ত)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	ভঙ্গ;
    }

    // add extra indent for a comment
    যদি (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    যদি (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	যদি (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    যদি (cur_amount > 0)
		amount = cur_amount;
	    অন্যথা যদি (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    যদি (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    প্রত্যাগম amount;
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
    পূর্ণাঙ্ক
in_cinkeys(
    পূর্ণাঙ্ক		keytyped,
    পূর্ণাঙ্ক		when,
    পূর্ণাঙ্ক		সারি_is_empty)
{
    char_u	*look;
    পূর্ণাঙ্ক		try_match;
    পূর্ণাঙ্ক		try_match_word;
    char_u	*p;
    char_u	*সারি;
    পূর্ণাঙ্ক		icase;
    পূর্ণাঙ্ক		i;

    যদি (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	প্রত্যাগম FALSE;

#ifdef FEAT_EVAL
    যদি (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    অন্যথা
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    যাবত্ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	বিকল্প (when)
	{
	    পক্ষ '*': try_match = (*look == '*'); ভঙ্গ;
	    পক্ষ '!': try_match = (*look == '!'); ভঙ্গ;
	    স্বভাব: try_match = (*look != '*'); ভঙ্গ;
	}
	যদি (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	যদি (*look == '0')
	{
	    try_match_word = try_match;
	    যদি (!সারি_is_empty)
		try_match = FALSE;
	    ++look;
	}
	অন্যথা
	    try_match_word = FALSE;

	// does it look like a control character?
	যদি (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    যদি (try_match && keytyped == Ctrl_chr(look[1]))
		প্রত্যাগম TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	অন্যথা যদি (*look == 'o')
	{
	    যদি (try_match && keytyped == চাবি_OPEN_FORW)
		প্রত্যাগম TRUE;
	    ++look;
	}
	অন্যথা যদি (*look == 'O')
	{
	    যদি (try_match && keytyped == চাবি_OPEN_BACK)
		প্রত্যাগম TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	অন্যথা যদি (*look == 'e')
	{
	    যদি (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		যদি (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    প্রত্যাগম TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	অন্যথা যদি (*look == ':')
	{
	    যদি (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		যদি (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    প্রত্যাগম TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		যদি (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    যদি (i)
			প্রত্যাগম TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	অন্যথা যদি (*look == '<')
	{
	    যদি (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		যদি (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != রিক্ত
						       && keytyped == look[1])
		    প্রত্যাগম TRUE;

		যদি (keytyped == get_special_চাবি_code(look + 1))
		    প্রত্যাগম TRUE;
	    }
	    যাবত্ (*look && *look != '>')
		look++;
	    যাবত্ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	অন্যথা যদি (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    যদি (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    অন্যথা
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    যদি (p == রিক্ত)
		p = look + STRLEN(look);
	    যদি ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		পূর্ণাঙ্ক		match = FALSE;

		যদি (keytyped == চাবি_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    সারি = ml_get_curline();
		    যদি (has_mbyte)
		    {
			char_u	*n;

			প্রতি (s = সারি + curwin->w_cursor.col; s > সারি; s = n)
			{
			    n = mb_prevptr(সারি, s);
			    যদি (!vim_iswordp(n))
				ভঙ্গ;
			}
		    }
		    অন্যথা
			প্রতি (s = সারি + curwin->w_cursor.col; s > সারি; --s)
			    যদি (!vim_iswordc(s[-1]))
				ভঙ্গ;
		    যদি (s + (p - look) <= সারি + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		অন্যথা
		    // TODO: multi-byte
		    যদি (keytyped == (পূর্ণাঙ্ক)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((পূর্ণাঙ্ক)p[-1])))
		{
		    সারি = ml_get_cursor();
		    যদি ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(সারি[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(সারি - (p - look), look, p - look)
				: STRNCMP(সারি - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		যদি (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    যদি (getwhitecols_curline() !=
				     (পূর্ণাঙ্ক)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		যদি (match)
		    প্রত্যাগম TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	অন্যথা
	{
	    যদি (try_match && *look == keytyped)
		প্রত্যাগম TRUE;
	    যদি (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    প্রত্যাগম FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    শূন্য
do_c_রাশিমালা_indent(শূন্য)
{
#ifdef FEAT_EVAL
    যদি (*curbuf->b_p_inde != NUL)
	fixthisline(get_রাশিমালা_indent);
    অন্যথা
#endif
	fixthisline(get_c_indent);
}

#যদি defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    শূন্য
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    যদি (lnum >= 1 && lnum <= curbuf->b_ml.ml_সারি_গুনতি)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_সংখ্যা = get_c_indent();
	curwin->w_cursor = pos;
    }
    অন্যথা
	rettv->vval.v_সংখ্যা = -1;
}
#endif
