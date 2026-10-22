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

#شامل "vim.h"

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
#define LOOKFOR_JS_کلید		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    صحیح
cin_is_cinword(char_u *سطر)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    صحیح		cinw_len;
    صحیح		retval = FALSE;
    صحیح		len;

    cinw_len = (صحیح)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    اگر (cinw_buf == پوچ)
	بازگشت FALSE;

    سطر = skipwhite(سطر);
    برای (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	اگر (STRNCMP(سطر, cinw_buf, len) == 0
		&& (!vim_iswordc(سطر[len]) || !vim_iswordc(سطر[len - 1])))
	{
	    retval = TRUE;
	    بشکن;
	}
    }
    vim_free(cinw_buf);
    بازگشت retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ایستا char_u *
skip_string(char_u *p)
{
    صحیح	    i;

    // We loop, because strings may be concatenated: "date""time".
    برای ( ; ; ++p)
    {
	اگر (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    اگر (p[1] == NUL)		    // ' at end of line
		بشکن;
	    i = 2;
	    اگر (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		وقتی (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    اگر (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ادامه;
	    }
	}
	وگرنه اگر (p[0] == '"')		    // start of string
	{
	    برای (++p; p[0]; ++p)
	    {
		اگر (p[0] == '\\' && p[1] != NUL)
		    ++p;
		وگرنه اگر (p[0] == '"')	    // end of string
		    بشکن;
	    }
	    اگر (p[0] == '"')
		ادامه; // continue for another string
	}
	وگرنه اگر (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    اگر (paren != پوچ)
	    {
		size_t delim_len = paren - delim;

		برای (p += 3; *p; ++p)
		    اگر (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			بشکن;
		    }
		اگر (p[0] == '"')
		    ادامه; // continue for another string
	    }
	}
	بشکن;				    // no string found
    }
    اگر (!*p)
	--p;				    // backup from NUL
    بازگشت p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    صحیح
is_pos_in_string(char_u *سطر, colnr_T col)
{
    char_u *p;

    برای (p = سطر; *p && (colnr_T)(p - سطر) < col; ++p)
	p = skip_string(p);
    بازگشت !((colnr_T)(p - سطر) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    صحیح
check_linecomment(char_u *سطر)
{
    char_u  *p;

    p = سطر;
    // skip Lispish one-line comments
    اگر (curbuf->b_p_lisp)
    {
	اگر (vim_strchr(p, ';') != پوچ) // there may be comments
	{
	    صحیح in_str = FALSE;	// inside of string

	    p = سطر;		// scan from start
	    وقتی ((p = vim_strpbrk(p, (char_u *)"\";")) != پوچ)
	    {
		اگر (*p == '"')
		{
		    اگر (in_str)
		    {
			اگر (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    وگرنه اگر (p == سطر || ((p - سطر) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		وگرنه اگر (!in_str && ((p - سطر) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(سطر, (colnr_T)(p - سطر)))
		    بشکن;	// found!
		++p;
	    }
	}
	وگرنه
	    p = پوچ;
    }
    وگرنه
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	برای ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    اگر (*p == NUL)
		بشکن;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    اگر (p[0] == '/' && p[1] == '/'
				 && (p == سطر || p[-1] != '*' || p[2] != '*'))
		بشکن;
	}
	اگر (*p == NUL)
	    p = پوچ;
    }

    اگر (p == پوچ)
	بازگشت MAXCOL;
    بازگشت (صحیح)(p - سطر);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ایستا pos_T *
ind_find_start_comment(تهی)	    // XXX
{
    بازگشت find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(صحیح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحیح		cur_maxcomment = ind_maxcomment;

    برای (;;)
    {
	pos = findmatchlimit(پوچ, '*', FM_BACKWARD, cur_maxcomment);
	اگر (pos == پوچ)
	    بشکن;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	اگر (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    بشکن;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	اگر (cur_maxcomment <= 0)
	{
	    pos = پوچ;
	    بشکن;
	}
    }
    بازگشت pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ایستا pos_T *
find_start_rawstring(صحیح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحیح		cur_maxcomment = ind_maxcomment;

    برای (;;)
    {
	pos = findmatchlimit(پوچ, 'R', FM_BACKWARD, cur_maxcomment);
	اگر (pos == پوچ)
	    بشکن;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	اگر (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    بشکن;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	اگر (cur_maxcomment <= 0)
	{
	    pos = پوچ;
	    بشکن;
	}
    }
    بازگشت pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ایستا pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    ایستا pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    اگر (comment_pos != پوچ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    اگر (comment_pos == پوچ || (rs_pos != پوچ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	اگر (is_raw != پوچ && rs_pos != پوچ)
	    *is_raw = rs_pos->lnum;
	بازگشت rs_pos;
    }
    بازگشت comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    صحیح
cindent_on(تهی)
{
    بازگشت (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
تعریفنوع ساختار {
    صحیح	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ایستا char_u *
cin_skipcomment(char_u *s)
{
    وقتی (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	اگر (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    بشکن;
	}
	اگر (*s != '/')
	    بشکن;
	++s;
	اگر (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    بشکن;
	}
	اگر (*s != '*')
	    بشکن;
	برای (++s; *s; ++s)	// skip slash-star comment
	    اگر (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		بشکن;
	    }
    }
    بازگشت s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ایستا صحیح
cin_nocode(char_u *s)
{
    بازگشت *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ایستا صحیح
cin_iscomment(char_u *p)
{
    بازگشت (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ایستا صحیح
cin_islinecomment(char_u *p)
{
    بازگشت (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ایستا pos_T *
find_سطر_comment(تهی) // XXX
{
    ایستا pos_T pos;
    char_u	 *سطر;
    char_u	 *p;

    pos = curwin->w_cursor;
    وقتی (--pos.lnum > 0)
    {
	سطر = ml_get(pos.lnum);
	p = skipwhite(سطر);
	اگر (cin_islinecomment(p))
	{
	    pos.col = (صحیح)(p - سطر);
	    بازگشت &pos;
	}
	اگر (*p != NUL)
	    بشکن;
    }
    بازگشت پوچ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ایستا صحیح
cin_has_js_کلید(char_u *text)
{
    char_u *s = skipwhite(text);
    صحیح	    quote = -1;

    اگر (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    اگر (!vim_isIDc(*s))	    // need at least one ID character
	بازگشت FALSE;

    وقتی (vim_isIDc(*s))
	++s;
    اگر (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    بازگشت (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ایستا صحیح
cin_islabel_skip(char_u **s)
{
    اگر (!vim_isIDc(**s))	    // need at least one ID character
	بازگشت FALSE;

    وقتی (vim_isIDc(**s))
    {
	اگر (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	وگرنه
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    بازگشت (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ایستا صحیح
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    صحیح	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    اگر (cinsd_buf == پوچ)
	بازگشت FALSE;

    برای (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (صحیح)cinsd_len, ",");
	اگر (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    اگر (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		بشکن;
	    }
	}
    }

    vim_free(cinsd_buf);
    بازگشت found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ایستا صحیح
cin_ispreproc(char_u *s)
{
    اگر (*skipwhite(s) == '#')
	بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ایستا صحیح
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, صحیح *amount)
{
    char_u	*سطر = *pp;
    linenr_T	lnum = *lnump;
    صحیح		retval = FALSE;
    صحیح		candidate_amount = *amount;

    اگر (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    برای (;;)
    {
	اگر (cin_ispreproc(سطر))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    بشکن;
	}
	اگر (lnum == 1)
	    بشکن;
	سطر = ml_get(--lnum);
	اگر (*سطر == NUL || سطر[STRLEN(سطر) - 1] != '\\')
	    بشکن;
    }

    اگر (lnum != *lnump)
	*pp = ml_get(*lnump);
    اگر (retval)
	*amount = candidate_amount;
    بازگشت retval;
}

    ایستا صحیح
cin_iselse(
    char_u  *p)
{
    اگر (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    بازگشت (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    ایستا صحیح
cin_isterminated(
    char_u	*s,
    صحیح		incl_open,	// include '{' at the end as terminator
    صحیح		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    صحیح		is_else = FALSE;

    s = cin_skipcomment(s);

    اگر (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    اگر (!found_start)
	is_else = cin_iselse(s);

    وقتی (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	اگر (*s == '}' && n_open > 0)
	    --n_open;
	اگر ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    بازگشت *s;
	وگرنه اگر (*s == '{')
	{
	    اگر (incl_open && cin_nocode(s + 1))
		بازگشت *s;
	    وگرنه
		++n_open;
	}

	اگر (*s)
	    s++;
    }
    بازگشت found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ایستا صحیح
cin_starts_with(char_u *s, نویسه *word)
{
    صحیح l = (صحیح)STRLEN(word);

    بازگشت (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ایستا صحیح
cin_isdefault(char_u *s)
{
    بازگشت (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ایستا صحیح
cin_iscase(
    char_u *s,
    صحیح strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    اگر (cin_starts_with(s, "case"))
    {
	برای (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    اگر (*s == NUL)
		بشکن;
	    اگر (*s == ':')
	    {
		اگر (s[1] == ':')	// skip over "::" for C++
		    ++s;
		وگرنه
		    بازگشت TRUE;
	    }
	    اگر (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    وگرنه اگر (*s == '/' && (s[1] == '*' || s[1] == '/'))
		بازگشت FALSE;		// stop at comment
	    وگرنه اگر (*s == '"')
	    {
		// JS etc.
		اگر (strict)
		    بازگشت FALSE;		// stop at string
		وگرنه
		    بازگشت TRUE;
	    }
	}
	بازگشت FALSE;
    }

    اگر (cin_isdefault(s))
	بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ایستا صحیح
cin_islabel(تهی)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    اگر (cin_isdefault(s))
	بازگشت FALSE;
    اگر (cin_isscopedecl(s))
	بازگشت FALSE;

    اگر (!cin_islabel_skip(&s))
	بازگشت FALSE;

    اگر (ind_find_start_CORS(پوچ))
	بازگشت FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*سطر;

    cursor_save = curwin->w_cursor;
    وقتی (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	اگر ((trypos = ind_find_start_CORS(پوچ)) != پوچ) // XXX
	    curwin->w_cursor = *trypos;

	سطر = ml_get_curline();
	اگر (cin_ispreproc(سطر))	// ignore #defines, #if, etc.
	    ادامه;
	اگر (*(سطر = cin_skipcomment(سطر)) == NUL)
	    ادامه;

	curwin->w_cursor = cursor_save;
	اگر (cin_isterminated(سطر, TRUE, FALSE)
		|| cin_isscopedecl(سطر)
		|| cin_iscase(سطر, TRUE)
		|| (cin_islabel_skip(&سطر) && cin_nocode(سطر)))
	    بازگشت TRUE;
	بازگشت FALSE;
    }
    curwin->w_cursor = cursor_save;
    بازگشت TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ایستا صحیح
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    صحیح		len = (صحیح)STRLEN(find);

    وقتی (*p != NUL)
    {
	p = cin_skipcomment(p);
	اگر (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    اگر (cin_nocode(r))
		بازگشت TRUE;
	}
	اگر (*p != NUL)
	    ++p;
    }
    بازگشت FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ایستا char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = پوچ, *p = s;
    انجامبده
    {
	r = p;
	p = cin_skipcomment(p);
	اگر (*p)
	    p = skip_string(p);
    } وقتی (p != r);
    بازگشت p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ایستا صحیح
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = پوچ;

    وقتی (*p)
    {
	اگر (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	وگرنه اگر (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	وگرنه
	    p = cin_skip_comment_and_string(p + 1);
    }
    اگر (!r)
	بازگشت FALSE;
    p = r; // p points now after '=' or "return"

    اگر (cin_nocode(p))
	بازگشت TRUE;

    اگر (*p == '&')
	p = cin_skipcomment(p + 1);

    اگر (*p == '(') // skip a typecast
    {
	صحیح open_شمار = 1;
	انجامبده
	{
	    p = cin_skip_comment_and_string(p + 1);
	    اگر (cin_nocode(p))
		بازگشت TRUE;
	    open_شمار += (*p == '(') - (*p == ')');
	} وقتی (open_شمار);
	p = cin_skipcomment(p + 1);
	اگر (cin_nocode(p))
	    بازگشت TRUE;
    }

    وقتی (*p == '{')
	p = cin_skipcomment(p + 1);
    بازگشت cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ایستا صحیح
cin_isinit(تهی)
{
    char_u	*s;
    ایستا نویسه *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    اگر (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    برای (;;)
    {
	صحیح i, l;

	برای (i = 0; i < (صحیح)ARRAY_LENGTH(skip); ++i)
	{
	    l = (صحیح)strlen(skip[i]);
	    اگر (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		بشکن;
	    }
	}
	اگر (l != 0)
	    بشکن;
    }

    اگر (cin_starts_with(s, "enum"))
	بازگشت TRUE;

    بازگشت cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ایستا صحیح
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    صحیح		has_name = FALSE;
    صحیح		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    وقتی ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    اگر (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	وقتی (*p != NUL)
	{
	    اگر (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    وگرنه اگر (*p == '{')
	    {
		بشکن;
	    }
	    وگرنه اگر (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		اگر (has_name)
		    بازگشت FALSE; // word character after skipping past name
		++p;
	    }
	    وگرنه اگر (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		اگر (!has_name_start || has_name)
		    بازگشت FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    وگرنه
	    {
		بازگشت FALSE;
	    }
	}
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ایستا صحیح
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    صحیح		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    اگر (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	وقتی (*p != NUL)
	{
	    اگر (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    وگرنه اگر (*p == '{')
	    {
		بشکن;
	    }
	    وگرنه اگر (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		اگر (has_string_literal)
		    بازگشت FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    وگرنه اگر (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		اگر (has_string_literal)
		    بازگشت FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    وگرنه
	    {
		بازگشت FALSE;
	    }
	}
	بازگشت has_string_literal ? TRUE : FALSE;
    }
    بازگشت FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ایستا char_u *
after_label(char_u *l)
{
    برای ( ; *l; ++l)
    {
	اگر (*l == ':')
	{
	    اگر (l[1] == ':')	    // skip over "::" for C++
		++l;
	    وگرنه اگر (!cin_iscase(l + 1, FALSE))
		بشکن;
	}
	وگرنه اگر (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    اگر (*l == NUL)
	بازگشت پوچ;
    l = cin_skipcomment(l + 1);
    اگر (*l == NUL)
	بازگشت پوچ;
    بازگشت l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ایستا صحیح
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    اگر (p == پوچ)
	بازگشت 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, پوچ, پوچ, 0);
    بازگشت (صحیح)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ایستا صحیح
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    صحیح		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    اگر (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	اگر (l == پوچ)		// just in case
	    l = ml_get_curline();
    }
    وگرنه
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    بازگشت amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ایستا صحیح
cin_first_id_amount(تهی)
{
    char_u	*سطر, *p, *s;
    صحیح		len;
    pos_T	fp;
    colnr_T	col;

    سطر = ml_get_curline();
    p = skipwhite(سطر);
    len = (صحیح)(skiptowhite(p) - p);
    اگر (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (صحیح)(skiptowhite(p) - p);
    }
    اگر (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    وگرنه اگر (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    وگرنه اگر ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	اگر ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    برای (len = 0; vim_isIDc(p[len]); ++len)
	;
    اگر (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	بازگشت 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - سطر);
    getvcol(curwin, &fp, &col, پوچ, پوچ, 0);
    بازگشت (صحیح)col;
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
    ایستا صحیح
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*سطر;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    اگر (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	اگر (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	    بازگشت -1;
    }

    سطر = s = ml_get(lnum);
    وقتی (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == پوچ)
    {
	اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وگرنه
	    ++s;
    }
    اگر (*s != '=')
	بازگشت 0;

    s = skipwhite(s + 1);
    اگر (cin_nocode(s))
	بازگشت 0;

    اگر (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - سطر);
    getvcol(curwin, &fp, &col, پوچ, پوچ, 0);
    بازگشت (صحیح)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ایستا صحیح
cin_skip2pos(pos_T *trypos)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*new_p;

    p = سطر = ml_get(trypos->lnum);
    وقتی (*p && (colnr_T)(p - سطر) < trypos->col)
    {
	اگر (cin_iscomment(p))
	    p = cin_skipcomment(p);
	وگرنه
	{
	    new_p = skip_string(p);
	    اگر (new_p == p)
		++p;
	    وگرنه
		p = new_p;
	}
    }
    بازگشت (صحیح)(p - سطر);
}

    ایستا pos_T *
find_match_char(صحیح c, صحیح ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ایستا pos_T pos_copy;
    صحیح		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    اگر ((trypos = findmatchlimit(پوچ, c, FM_SKIPCOMM, ind_maxp_wk)) != پوچ)
    {
	// check if the ( is in a // comment
	اگر ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (صحیح)(cursor_save.lnum - trypos->lnum);
	    اگر (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		برو retry;
	    }
	    trypos = پوچ;
	}
	وگرنه
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    اگر ((trypos_wk = ind_find_start_CORS(پوچ)) != پوچ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (صحیح)(cursor_save.lnum
			- trypos_wk->lnum);
		اگر (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    برو retry;
		}
		trypos = پوچ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    بازگشت trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ایستا pos_T *
find_match_paren(صحیح ind_maxparen)	// XXX
{
    بازگشت find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ایستا صحیح
find_last_paren(char_u *l, صحیح start, صحیح end)
{
    صحیح		i;
    صحیح		retval = FALSE;
    صحیح		open_شمار = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    برای (i = 0; l[i] != NUL; i++)
    {
	i = (صحیح)(cin_skipcomment(l + i) - l); // ignore parens in comments
	اگر (l[i] == NUL)
	    بشکن;
	i = (صحیح)(skip_string(l + i) - l);    // ignore parens in quotes
	اگر (l[i] == start)
	    ++open_شمار;
	وگرنه اگر (l[i] == end)
	{
	    اگر (open_شمار > 0)
		--open_شمار;
	    وگرنه
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    بازگشت retval;
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
    ایستا صحیح
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    صحیح		retval = FALSE;
    pos_T	*trypos;
    صحیح		just_started = TRUE;

    اگر (sp == پوچ)
	s = ml_get(lnum);
    وگرنه
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    اگر (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != پوچ)
    {
	lnum = trypos->lnum;
	اگر (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    بازگشت FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    اگر (cin_ispreproc(s))
	بازگشت FALSE;

    وقتی (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وگرنه اگر (*s == ':')
	{
	    اگر (*(s + 1) == ':')
		s += 2;
	    وگرنه
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		بازگشت FALSE;
	}
	وگرنه
	    ++s;
    }
    اگر (*s != '(')
	بازگشت FALSE;		// ';', ' or "  before any () or no '('

    وقتی (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	اگر (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    اگر (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    برو done;
	}
	اگر ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    صحیح comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    برای (;;)
	    {
		اگر (lnum >= curbuf->b_ml.ml_سطر_شمار)
		    بشکن;
		s = ml_get(++lnum);
		اگر (!cin_ispreproc(s))
		    بشکن;
	    }
	    اگر (lnum >= curbuf->b_ml.ml_سطر_شمار)
		بشکن;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    اگر (!just_started && (!comma && *s != ',' && *s != ')'))
		بشکن;
	    just_started = FALSE;
	}
	وگرنه اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وگرنه
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    اگر (lnum != first_lnum && sp != پوچ)
	*sp = ml_get(first_lnum);

    بازگشت retval;
}

    ایستا صحیح
cin_isif(char_u *p)
{
    بازگشت (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ایستا صحیح
cin_isdo(char_u *p)
{
    بازگشت (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ایستا صحیح
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    صحیح		retval = FALSE;

    p = cin_skipcomment(p);
    اگر (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    اگر (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	وقتی (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	اگر ((trypos = findmatchlimit(پوچ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != پوچ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    بازگشت retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ایستا صحیح
cin_is_if_for_while_before_offset(char_u *سطر, صحیح *poffset)
{
    صحیح offset = *poffset;

    اگر (offset-- < 2)
	بازگشت 0;
    وقتی (offset > 2 && VIM_ISWHITE(سطر[offset]))
	--offset;

    offset -= 1;
    اگر (STRNCMP(سطر + offset, "if", 2) == 0)
	برو probablyFound;

    اگر (offset >= 1)
    {
	offset -= 1;
	اگر (STRNCMP(سطر + offset, "for", 3) == 0)
	    برو probablyFound;

	اگر (offset >= 2)
	{
	    offset -= 2;
	    اگر (STRNCMP(سطر + offset, "while", 5) == 0)
		برو probablyFound;
	}
    }
    بازگشت 0;

probablyFound:
    اگر (!offset || !vim_isIDc(سطر[offset - 1]))
    {
	*poffset = offset;
	بازگشت 1;
    }
    بازگشت 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ایستا صحیح
cin_iswhileofdo_end(صحیح terminated)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    صحیح		i;

    اگر (terminated != ';')	// there must be a ';' at the end
	بازگشت FALSE;

    p = سطر = ml_get_curline();
    وقتی (*p != NUL)
    {
	p = cin_skipcomment(p);
	اگر (*p == ')')
	{
	    s = skipwhite(p + 1);
	    اگر (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (صحیح)(p - سطر);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		اگر (trypos != پوچ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    اگر (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    اگر (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			بازگشت TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		سطر = ml_get_curline();
		p = سطر + i;
	    }
	}
	اگر (*p != NUL)
	    ++p;
    }
    بازگشت FALSE;
}

    ایستا صحیح
cin_isbreak(char_u *p)
{
    بازگشت (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    ایستا صحیح
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    صحیح		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*سطر = ml_get_curline();

    اگر (pos->lnum <= lnum)
	بازگشت cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(سطر);
    اگر (*s == '#')		// skip #define FOO x ? (x) : x
	بازگشت FALSE;
    s = cin_skipcomment(s);
    اگر (*s == NUL)
	بازگشت FALSE;

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
    وقتی (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	s = skipwhite(سطر);
	اگر (*s == '#' || *s == NUL)
	    بشکن;
	وقتی (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    اگر (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		بشکن;
	    اگر (*s != NUL)
		++s;
	}
	اگر (*s != NUL)
	    بشکن;
	--lnum;
    }

    pos->lnum = lnum;
    سطر = ml_get(lnum);
    s = سطر;
    برای (;;)
    {
	اگر (*s == NUL)
	{
	    اگر (lnum == curwin->w_cursor.lnum)
		بشکن;
	    // Continue in the cursor line.
	    سطر = ml_get(++lnum);
	    s = سطر;
	}
	اگر (s == سطر)
	{
	    // don't recognize "case (foo):" as a baseclass
	    اگر (cin_iscase(s, FALSE))
		بشکن;
	    s = cin_skipcomment(سطر);
	    اگر (*s == NUL)
		ادامه;
	}

	اگر (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	وگرنه اگر (s[0] == ':')
	{
	    اگر (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    وگرنه اگر (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    وگرنه
		s = cin_skipcomment(s + 1);
	}
	وگرنه اگر ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    اگر (*s == 'c')
		s = cin_skipcomment(s + 5);
	    وگرنه
		s = cin_skipcomment(s + 6);
	}
	وگرنه
	{
	    اگر (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    وگرنه اگر (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    وگرنه اگر (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		بازگشت FALSE;
	    }
	    وگرنه اگر (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    وگرنه اگر (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		اگر (cpp_base_class)
		    pos->col = (colnr_T)(s - سطر);
	    }

	    // When the line ends in a comma don't align with it.
	    اگر (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    اگر (cpp_base_class)
	pos->lnum = lnum;
    بازگشت cpp_base_class;
}

    ایستا صحیح
get_baseclass_amount(صحیح col)
{
    صحیح		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    اگر (col == 0)
    {
	amount = get_indent();
	اگر (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != پوچ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	اگر (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    وگرنه
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, پوچ, پوچ, 0);
	amount = (صحیح)vcol;
    }
    اگر (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    بازگشت amount;
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

    ایستا pos_T *
find_start_brace(تهی)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ایستا pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    وقتی ((trypos = findmatchlimit(پوچ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != پوچ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = پوچ;
	// ignore the { if it's in a // or / *  * / comment
	اگر ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(پوچ)) == پوچ) // XXX
	    بشکن;
	اگر (pos != پوچ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    بازگشت trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ایستا pos_T *
find_match_paren_after_brace (صحیح ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    اگر (trypos == پوچ)
	بازگشت پوچ;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    اگر (tryposBrace != پوچ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = پوچ;
    بازگشت trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ایستا صحیح
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    اگر (n > 0 && n < curbuf->b_ind_maxparen / 2)
	بازگشت curbuf->b_ind_maxparen - (صحیح)n;
    بازگشت curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    تهی
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    صحیح		divider;
    صحیح		fraction = 0;
    صحیح		sw;
    long	t = get_sw_مقدار(buf);

    // needed for cino-(, it will be multiplied by 2 again
    اگر (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    وگرنه
	sw = (صحیح)t;

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
    buf->b_ind_open_چپ_imag = 0;

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

    برای (p = buf->b_p_cino; *p; )
    {
	l = p++;
	اگر (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	اگر (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((نویسه *)++p);
	    وقتی (VIM_ISDIGIT(*p))
	    {
		++p;
		اگر (divider)
		    divider *= 10;
		وگرنه
		    divider = 10;
	    }
	}
	اگر (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    اگر (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    وگرنه
	    {
		n *= sw;
		اگر (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	اگر (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	انتخاب (*l)
	{
	    حالت '>': buf->b_ind_level = (صحیح)n; بشکن;
	    حالت 'e': buf->b_ind_open_imag = (صحیح)n; بشکن;
	    حالت 'n': buf->b_ind_no_brace = (صحیح)n; بشکن;
	    حالت 'f': buf->b_ind_first_open = (صحیح)n; بشکن;
	    حالت '{': buf->b_ind_open_extra = (صحیح)n; بشکن;
	    حالت '}': buf->b_ind_close_extra = (صحیح)n; بشکن;
	    حالت '^': buf->b_ind_open_چپ_imag = (صحیح)n; بشکن;
	    حالت 'L': buf->b_ind_jump_label = (صحیح)n; بشکن;
	    حالت ':': buf->b_ind_case = (صحیح)n; بشکن;
	    حالت '=': buf->b_ind_case_code = (صحیح)n; بشکن;
	    حالت 'b': buf->b_ind_case_break = (صحیح)n; بشکن;
	    حالت 'p': buf->b_ind_param = (صحیح)n; بشکن;
	    حالت 't': buf->b_ind_func_type = (صحیح)n; بشکن;
	    حالت '/': buf->b_ind_comment = (صحیح)n; بشکن;
	    حالت 'c': buf->b_ind_in_comment = (صحیح)n; بشکن;
	    حالت 'C': buf->b_ind_in_comment2 = (صحیح)n; بشکن;
	    حالت 'i': buf->b_ind_cpp_baseclass = (صحیح)n; بشکن;
	    حالت '+': buf->b_ind_continuation = (صحیح)n; بشکن;
	    حالت '(': buf->b_ind_unclosed = (صحیح)n; بشکن;
	    حالت 'u': buf->b_ind_unclosed2 = (صحیح)n; بشکن;
	    حالت 'U': buf->b_ind_unclosed_noignore = (صحیح)n; بشکن;
	    حالت 'W': buf->b_ind_unclosed_wrapped = (صحیح)n; بشکن;
	    حالت 'w': buf->b_ind_unclosed_whiteok = (صحیح)n; بشکن;
	    حالت 'm': buf->b_ind_matching_paren = (صحیح)n; بشکن;
	    حالت 'M': buf->b_ind_paren_prev = (صحیح)n; بشکن;
	    حالت ')': buf->b_ind_maxparen = (صحیح)n; بشکن;
	    حالت '*': buf->b_ind_maxcomment = (صحیح)n; بشکن;
	    حالت 'g': buf->b_ind_scopedecl = (صحیح)n; بشکن;
	    حالت 'h': buf->b_ind_scopedecl_code = (صحیح)n; بشکن;
	    حالت 'j': buf->b_ind_java = (صحیح)n; بشکن;
	    حالت 'J': buf->b_ind_js = (صحیح)n; بشکن;
	    حالت 'l': buf->b_ind_keep_case_label = (صحیح)n; بشکن;
	    حالت '#': buf->b_ind_hash_comment = (صحیح)n; بشکن;
	    حالت 'N': buf->b_ind_cpp_namespace = (صحیح)n; بشکن;
	    حالت 'k': buf->b_ind_if_for_while = (صحیح)n; بشکن;
	    حالت 'E': buf->b_ind_cpp_extern_c = (صحیح)n; بشکن;
	    حالت 'P': buf->b_ind_pragma = (صحیح)n; بشکن;
	}
	اگر (*p == ',')
	    ++p;
    }
}

    ایستا صحیح
find_match(صحیح lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    صحیح		elselevel;
    صحیح		whilelevel;

    اگر (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    وگرنه
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    وقتی (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	اگر (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    اگر (theirscope == پوچ)
		بشکن;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    اگر (theirscope->lnum < ourscope)
		بشکن;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    اگر (theirscope->lnum > ourscope)
		ادامه;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    اگر (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		اگر (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    اگر (!cin_isif(mightbeif))
			++elselevel;
		    ادامه;
		}

		// If it's an "if" decrement elselevel
		اگر (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    اگر (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    اگر (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ادامه;
	    }

	    // If it's a "do" decrement whilelevel
	    اگر (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    اگر (elselevel <= 0 && whilelevel <= 0)
		بازگشت OK;
	}
    }
    بازگشت FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    صحیح
get_c_indent(تهی)
{
    pos_T	cur_curpos;
    صحیح		amount;
    صحیح		scope_amount;
    صحیح		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = پوچ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    صحیح		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    صحیح		lookfor;
    صحیح		whilelevel;
    linenr_T	lnum;
    صحیح		n;
    صحیح		iscase;
    صحیح		lookfor_break;
    صحیح		lookfor_cpp_namespace = FALSE;
    صحیح		cont_amount = 0;    // amount for continuation line
    صحیح		original_سطر_islabel;
    صحیح		added_to_amount = 0;
    صحیح		js_cur_has_کلید = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    صحیح		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    اگر (cur_curpos.lnum == 1)
	بازگشت 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    اگر (linecopy == پوچ)
	بازگشت 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    اگر ((State & MODE_درج)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_سطر_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    اگر (comment_pos != پوچ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    اگر (trypos != پوچ && (comment_pos == پوچ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	برو laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    اگر (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	اگر (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    برو theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    اگر (original_سطر_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	برو theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    اگر (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_سطر_comment(); // XXX
	اگر (trypos == پوچ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    اگر (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	اگر (trypos  != پوچ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, پوچ, پوچ, 0);
	    amount = col;
	    برو theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    اگر (!cin_iscomment(theline) && comment_pos != پوچ) // XXX
    {
	صحیح	lead_start_len = 2;
	صحیح	lead_میانه_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_میانه[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	صحیح	lead_end_len;
	char_u	*p;
	صحیح	start_align = 0;
	صحیح	start_off = 0;
	صحیح	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, پوچ, پوچ, 0);
	amount = col;
	*lead_start = NUL;
	*lead_میانه = NUL;

	p = curbuf->b_p_com;
	وقتی (*p != NUL)
	{
	    صحیح	align = 0;
	    صحیح	off = 0;
	    صحیح what = 0;

	    وقتی (*p != NUL && *p != ':')
	    {
		اگر (*p == COM_START || *p == COM_END || *p == COM_میانه)
		    what = *p++;
		وگرنه اگر (*p == COM_چپ || *p == COM_راست)
		    align = *p++;
		وگرنه اگر (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		وگرنه
		    ++p;
	    }

	    اگر (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    اگر (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    وگرنه اگر (what == COM_میانه)
	    {
		STRCPY(lead_میانه, lead_end);
		lead_میانه_len = lead_end_len;
	    }
	    وگرنه اگر (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		اگر (STRNCMP(theline, lead_میانه, lead_میانه_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    اگر (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			اگر (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			وگرنه اگر (STRNCMP(look, lead_میانه,
							lead_میانه_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    بشکن;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			وگرنه اگر (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    ادامه;
		    }
		    اگر (start_off != 0)
			amount += start_off;
		    وگرنه اگر (start_align == COM_راست)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_میانه);
		    بشکن;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		اگر (STRNCMP(theline, lead_میانه, lead_میانه_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    اگر (off != 0)
			amount += off;
		    وگرنه اگر (align == COM_راست)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_میانه);
		    done = TRUE;
		    بشکن;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	اگر (done)
	    ;
	وگرنه اگر (theline[0] == '*')
	    amount += 1;
	وگرنه
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    برای (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		اگر (linewhite(lnum))		    // skip blank lines
		    ادامه;
		amount = get_indent_lnum(lnum);	    // XXX
		بشکن;
	    }
	    اگر (amount == -1)			    // use the comment opener
	    {
		اگر (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    اگر (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, پوچ, پوچ, 0);
		amount = col;
		اگر (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	برو theend;
    }

    // Are we looking at a ']' that has a match?
    اگر (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != پوچ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	برو theend;
    }

    // Are we inside parentheses or braces?  XXX
    اگر (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != پوچ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != پوچ
	    || trypos != پوچ)
    {
      اگر (trypos != پوچ && tryposBrace != پوچ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  اگر (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = پوچ;
	  وگرنه
	      tryposBrace = پوچ;
      }

      اگر (trypos != پوچ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	اگر (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	وگرنه
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    برای (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		اگر (cin_nocode(l))		// skip comment lines
		    ادامه;
		اگر (cin_ispreproc_cont(&l, &lnum, &amount))
		    ادامه;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		اگر ((trypos = ind_find_start_CORS(پوچ)) != پوچ)
		{
		    lnum = trypos->lnum + 1;
		    ادامه;
		}

		// XXX
		اگر ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != پوچ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    اگر (theline[0] == ')')
		    {
			اگر (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    بشکن;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	اگر (amount == -1)
	{
	    صحیح	    ignore_paren_col = 0;
	    صحیح	    is_if_for_while = 0;

	    اگر (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *سطر;

		trypos = &our_paren_pos;
		انجامبده
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} وقتی (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		سطر = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(سطر, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    اگر (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *سطر;
		صحیح	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		سطر = ml_get_curline();
		look_col = (صحیح)(look - سطر);
		curwin->w_cursor.col = look_col + 1;
		اگر ((trypos = findmatchlimit(پوچ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != پوچ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    اگر (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		اگر (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    اگر (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			برای (col = 0; col < our_paren_pos.col; ++col)
			{
			    انتخاب (l[col])
			    {
				حالت '(':
				حالت '{': ++n;
					  بشکن;

				حالت ')':
				حالت '}': اگر (n > 1)
					      --n;
					  بشکن;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    وگرنه اگر (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    وگرنه
		    {
			col = our_paren_pos.col + 1;
			وقتی (VIM_ISWHITE(l[col]))
			    col++;
			اگر (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			وگرنه
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		اگر (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, پوچ, پوچ, 0);
		    اگر (cur_amount > (صحیح)col)
			cur_amount = col;
		}
	    }

	    اگر (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    وگرنه اگر ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		اگر (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    وگرنه
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		وقتی ((صحیح)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    انتخاب (*ml_get_pos(&our_paren_pos))
		    {
			حالت '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  بشکن;
			حالت ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  بشکن;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		اگر (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		وگرنه
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    اگر (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != پوچ)
			amount += curbuf->b_ind_unclosed2;
		    وگرنه
		    {
			اگر (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			وگرنه
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		اگر (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	اگر (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      وگرنه
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
	اگر (*look == '{')
	{
	    getvcol(curwin, trypos, &col, پوچ, پوچ, 0);
	    amount = col;
	    اگر (*start == '{')
		start_brace = BRACE_IN_COL0;
	    وگرنه
		start_brace = BRACE_AT_START;
	}
	وگرنه
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    اگر (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != پوچ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    اگر ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    وگرنه اگر (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    وگرنه
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	اگر (curbuf->b_ind_js)
	    js_cur_has_کلید = cin_has_js_کلید(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	اگر (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	وگرنه
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    اگر (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    وگرنه اگر (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    اگر (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		اگر (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    برو theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    اگر (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_چپ_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    وگرنه اگر (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    وگرنه
	    {
		اگر (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    اگر (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    وگرنه اگر (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		وگرنه
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    اگر (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    اگر (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    وگرنه اگر (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    وگرنه
	    {
		اگر (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    برای (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		اگر (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    اگر (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			اگر (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    اگر (cont_amount > 0)
				amount = cont_amount;
			    وگرنه اگر (!curbuf->b_ind_js)
				amount += ind_continuation;
			    بشکن;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(پوچ);
			اگر (trypos != پوچ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    ادامه;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ادامه;

			اگر (cin_nocode(l))
			    ادامه;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			اگر (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    اگر (terminated == ',')
				بشکن;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    اگر (terminated != ';' && cin_isinit())
				بشکن;

			    // nothing useful found
			    اگر (terminated == 0 || terminated == '{')
				ادامه;
			}

			اگر (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = پوچ;
			    اگر (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    اگر (trypos == پوچ && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    اگر (trypos != پوچ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ادامه;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
		    }
		    وگرنه اگر (lookfor == LOOKFOR_UNTERM)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
		    }
		    وگرنه
		    {
			اگر (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    اگر (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			اگر (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    اگر (curwin->w_cursor.lnum == ourscope)
				ادامه;

			    اگر (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				بشکن;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(پوچ);
			    اگر (trypos != پوچ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ادامه;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ادامه;

			    // Finally the actual check for "namespace".
			    اگر (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				بشکن;
			    }
			    وگرنه اگر (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				بشکن;
			    }

			    اگر (cin_nocode(l))
				ادامه;
			}
		    }
		    بشکن;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		اگر ((trypos = ind_find_start_CORS(&raw_string_start)) != پوچ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    ادامه;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		اگر (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    اگر (lookfor == LOOKFOR_CPP_BASECLASS)
			بشکن;

		    // When looking for a "do" we are not interested in
		    // labels.
		    اگر (whilelevel > 0)
			ادامه;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    اگر (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
			بشکن;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    اگر (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			اگر ((trypos = find_start_brace()) == پوچ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    بشکن;
			}
			ادامه;
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
		    اگر (lookfor == LOOKFOR_TERM)
		    {
			اگر (n)
			    amount = n;

			اگر (!lookfor_break)
			    بشکن;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    اگر (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			اگر (l != پوچ && cin_is_cinword(l))
			{
			    اگر (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    وگرنه
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			بشکن;
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
		    ادامه;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		اگر (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    اگر (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != پوچ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    ادامه;
		}

		// Ignore jump labels with nothing after them.
		اگر (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    اگر (l == پوچ || cin_nocode(l))
			ادامه;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ادامه;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		اگر (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		اگر (n)
		{
		    اگر (lookfor == LOOKFOR_UNTERM)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
		    }
		    وگرنه اگر (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ادامه;
		    }
		    وگرنه
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    بشکن;
		}
		وگرنه اگر (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    اگر (cin_isterminated(l, TRUE, FALSE))
			بشکن;
		    وگرنه
			ادامه;
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

		اگر (js_cur_has_کلید)
		{
		    js_cur_has_کلید = 0; // only check the first line
		    اگر (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_کلید;
		    }
		}
		اگر (lookfor == LOOKFOR_JS_کلید && cin_has_js_کلید(l))
		{
		    amount = get_indent();
		    بشکن;
		}
		اگر (lookfor == LOOKFOR_COMMA)
		{
		    اگر (tryposBrace != پوچ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			بشکن;
		    اگر (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			بشکن;
		    وگرنه
		    {
			amount = get_indent();
			اگر (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    بشکن;
		    }
		}

		اگر (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    اگر (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (تهی)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    اگر (trypos != پوچ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = پوچ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    اگر (trypos == پوچ && terminated == ',')
		    {
			اگر (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    اگر (trypos != پوچ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			اگر (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    ادامه;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    اگر (terminated == ',')
		    {
			وقتی (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    اگر (*l == NUL || l[STRLEN(l) - 1] != '\\')
				بشکن;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    اگر (curbuf->b_ind_js)
			cur_amount = get_indent();
		    وگرنه
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    اگر (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			اگر (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			اگر (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ادامه;
			}
			بشکن;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    اگر (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			اگر (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    اگر (cont_amount > 0)
				amount = cont_amount;
			    وگرنه
				amount += ind_continuation;
			    بشکن;
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
			اگر (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			اگر (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    بشکن;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			اگر (cin_isdo(l))
			{
			    اگر (whilelevel == 0)
				بشکن;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			اگر (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    اگر (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    اگر ((trypos = find_start_brace()) == پوچ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				بشکن;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    وگرنه
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			اگر (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    اگر (terminated == ',')
				amount += ind_continuation;
			    بشکن;
			}

			اگر (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    اگر (terminated == ',')
			    {
				اگر (curbuf->b_ind_cpp_baseclass == 0)
				    بشکن;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ادامه;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    اگر (amount > cur_amount)
				amount = cur_amount;
			}
			وگرنه
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (صحیح)STRLEN(l);
			    اگر (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				بشکن;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    اگر (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				اگر (curbuf->b_ind_js)
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
				    اگر (cin_iscomment(skipwhite(l)))
					بشکن;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    اگر (trypos != پوچ)
				    {
					اگر (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    بشکن;
					}
					ourscope = trypos->lnum;
				    }
				}
				وگرنه
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    وگرنه
			    {
				اگر (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				اگر (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_کلید
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		وگرنه اگر (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    اگر (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
			بشکن;
		    }

		    اگر (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			اگر (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		وگرنه
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    اگر (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ادامه;
		    }

		    // Handle "do {" line.
		    اگر (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			اگر (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ادامه;
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
		    اگر (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			وگرنه
			    amount += ind_continuation;
			بشکن;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    اگر (lookfor == LOOKFOR_TERM)
		    {
			اگر (!lookfor_break && whilelevel == 0)
			    بشکن;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    وگرنه
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			اگر (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != پوچ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    اگر (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				ادامه;
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

			اگر (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			اگر (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			اگر (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    اگر ((trypos = find_start_brace()) == پوچ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				بشکن;
			    ادامه;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			اگر (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != پوچ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    اگر (*l == '}' || !cin_iselse(l))
				برو term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      اگر (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      اگر (curbuf->b_ind_jump_label > 0 && original_سطر_islabel)
	  amount -= curbuf->b_ind_jump_label;

      برو theend;
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

    اگر (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	برو theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    اگر (cur_curpos.lnum < curbuf->b_ml.ml_سطر_شمار
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == پوچ
	    && vim_strchr(theline, '}') == پوچ
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(پوچ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	برو theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    وقتی (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	اگر ((trypos = ind_find_start_CORS(پوچ)) != پوچ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    ادامه;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	اگر (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	اگر (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    بشکن;
	}

	// Skip preprocessor directives and blank lines.
	اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ادامه;

	اگر (cin_nocode(l))
	    ادامه;

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
	اگر (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    اگر (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != پوچ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    وقتی (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		اگر (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    بشکن;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    اگر (amount == 0)
		amount = cin_first_id_amount();
	    اگر (amount == 0)
		amount = ind_continuation;
	    بشکن;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	اگر (cin_isfuncdecl(پوچ, cur_curpos.lnum, 0))  // XXX
	    بشکن;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	اگر (*skipwhite(l) == '}')
	    بشکن;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	اگر (cin_ends_in(l, (char_u *)"};"))
	    بشکن;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	اگر (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    بشکن;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	اگر (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    وقتی (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		اگر (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    بشکن;
	    }
	    اگر (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		بشکن;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	اگر (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    بشکن;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	اگر (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    اگر (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		بشکن;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	اگر ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != پوچ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	بشکن;
    }

    // add extra indent for a comment
    اگر (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    اگر (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	اگر (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    اگر (cur_amount > 0)
		amount = cur_amount;
	    وگرنه اگر (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    اگر (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    بازگشت amount;
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
    صحیح
in_cinkeys(
    صحیح		keytyped,
    صحیح		when,
    صحیح		سطر_is_empty)
{
    char_u	*look;
    صحیح		try_match;
    صحیح		try_match_word;
    char_u	*p;
    char_u	*سطر;
    صحیح		icase;
    صحیح		i;

    اگر (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	بازگشت FALSE;

#ifdef FEAT_EVAL
    اگر (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    وگرنه
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    وقتی (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	انتخاب (when)
	{
	    حالت '*': try_match = (*look == '*'); بشکن;
	    حالت '!': try_match = (*look == '!'); بشکن;
	    پیشفرض: try_match = (*look != '*'); بشکن;
	}
	اگر (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	اگر (*look == '0')
	{
	    try_match_word = try_match;
	    اگر (!سطر_is_empty)
		try_match = FALSE;
	    ++look;
	}
	وگرنه
	    try_match_word = FALSE;

	// does it look like a control character?
	اگر (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    اگر (try_match && keytyped == Ctrl_chr(look[1]))
		بازگشت TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	وگرنه اگر (*look == 'o')
	{
	    اگر (try_match && keytyped == کلید_OPEN_FORW)
		بازگشت TRUE;
	    ++look;
	}
	وگرنه اگر (*look == 'O')
	{
	    اگر (try_match && keytyped == کلید_OPEN_BACK)
		بازگشت TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	وگرنه اگر (*look == 'e')
	{
	    اگر (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		اگر (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    بازگشت TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	وگرنه اگر (*look == ':')
	{
	    اگر (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		اگر (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    بازگشت TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		اگر (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    اگر (i)
			بازگشت TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	وگرنه اگر (*look == '<')
	{
	    اگر (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		اگر (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != پوچ
						       && keytyped == look[1])
		    بازگشت TRUE;

		اگر (keytyped == get_special_کلید_code(look + 1))
		    بازگشت TRUE;
	    }
	    وقتی (*look && *look != '>')
		look++;
	    وقتی (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	وگرنه اگر (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    اگر (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    وگرنه
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    اگر (p == پوچ)
		p = look + STRLEN(look);
	    اگر ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		صحیح		match = FALSE;

		اگر (keytyped == کلید_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    سطر = ml_get_curline();
		    اگر (has_mbyte)
		    {
			char_u	*n;

			برای (s = سطر + curwin->w_cursor.col; s > سطر; s = n)
			{
			    n = mb_prevptr(سطر, s);
			    اگر (!vim_iswordp(n))
				بشکن;
			}
		    }
		    وگرنه
			برای (s = سطر + curwin->w_cursor.col; s > سطر; --s)
			    اگر (!vim_iswordc(s[-1]))
				بشکن;
		    اگر (s + (p - look) <= سطر + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		وگرنه
		    // TODO: multi-byte
		    اگر (keytyped == (صحیح)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((صحیح)p[-1])))
		{
		    سطر = ml_get_cursor();
		    اگر ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(سطر[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(سطر - (p - look), look, p - look)
				: STRNCMP(سطر - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		اگر (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    اگر (getwhitecols_curline() !=
				     (صحیح)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		اگر (match)
		    بازگشت TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	وگرنه
	{
	    اگر (try_match && *look == keytyped)
		بازگشت TRUE;
	    اگر (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    بازگشت FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    تهی
do_c_عبارت_indent(تهی)
{
#ifdef FEAT_EVAL
    اگر (*curbuf->b_p_inde != NUL)
	fixthisline(get_عبارت_indent);
    وگرنه
#endif
	fixthisline(get_c_indent);
}

#اگر defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    تهی
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    اگر (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	بازگشت;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_شمار)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_عدد = get_c_indent();
	curwin->w_cursor = pos;
    }
    وگرنه
	rettv->vval.v_عدد = -1;
}
#endif
