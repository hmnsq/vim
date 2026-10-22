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

#ضمن "vim.h"

// values for the "lookfor" state
#تعریف_کرو LOOKFOR_INITIAL		0
#تعریف_کرو LOOKFOR_IF		1
#تعریف_کرو LOOKFOR_DO		2
#تعریف_کرو LOOKFOR_CASE		3
#تعریف_کرو LOOKFOR_ANY		4
#تعریف_کرو LOOKFOR_رکن		5
#تعریف_کرو LOOKFOR_UNTERM		6
#تعریف_کرو LOOKFOR_SCOPEDECL	7
#تعریف_کرو LOOKFOR_NOBREAK		8
#تعریف_کرو LOOKFOR_CPP_BASECLASS	9
#تعریف_کرو LOOKFOR_ENUM_OR_INIT	10
#تعریف_کرو LOOKFOR_JS_کلید		11
#تعریف_کرو LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    صحيح
cin_is_cinword(char_u *سطر)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    صحيح		cinw_len;
    صحيح		retval = FALSE;
    صحيح		len;

    cinw_len = (صحيح)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    إذا (cinw_buf == عدم)
	أرجع FALSE;

    سطر = skipwhite(سطر);
    لكل (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	إذا (STRNCMP(سطر, cinw_buf, len) == 0
		&& (!vim_iswordc(سطر[len]) || !vim_iswordc(سطر[len - 1])))
	{
	    retval = TRUE;
	    اكسر;
	}
    }
    vim_free(cinw_buf);
    أرجع retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ساكن char_u *
skip_string(char_u *p)
{
    صحيح	    i;

    // We loop, because strings may be concatenated: "date""time".
    لكل ( ; ; ++p)
    {
	إذا (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    إذا (p[1] == NUL)		    // ' at end of line
		اكسر;
	    i = 2;
	    إذا (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		طالما (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    إذا (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		استمر;
	    }
	}
	وإلا إذا (p[0] == '"')		    // start of string
	{
	    لكل (++p; p[0]; ++p)
	    {
		إذا (p[0] == '\\' && p[1] != NUL)
		    ++p;
		وإلا إذا (p[0] == '"')	    // end of string
		    اكسر;
	    }
	    إذا (p[0] == '"')
		استمر; // continue for another string
	}
	وإلا إذا (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    إذا (paren != عدم)
	    {
		size_t delim_len = paren - delim;

		لكل (p += 3; *p; ++p)
		    إذا (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			اكسر;
		    }
		إذا (p[0] == '"')
		    استمر; // continue for another string
	    }
	}
	اكسر;				    // no string found
    }
    إذا (!*p)
	--p;				    // backup from NUL
    أرجع p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    صحيح
is_pos_وچ_string(char_u *سطر, colnr_T خانہ)
{
    char_u *p;

    لكل (p = سطر; *p && (colnr_T)(p - سطر) < خانہ; ++p)
	p = skip_string(p);
    أرجع !((colnr_T)(p - سطر) <= خانہ);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    صحيح
check_linecomment(char_u *سطر)
{
    char_u  *p;

    p = سطر;
    // skip Lispish one-line comments
    إذا (curbuf->b_p_lisp)
    {
	إذا (vim_strchr(p, ';') != عدم) // there may be comments
	{
	    صحيح وچ_str = FALSE;	// inside of string

	    p = سطر;		// scan from start
	    طالما ((p = vim_strpbrk(p, (char_u *)"\";")) != عدم)
	    {
		إذا (*p == '"')
		{
		    إذا (وچ_str)
		    {
			إذا (*(p - 1) != '\\') // skip escaped quote
			    وچ_str = FALSE;
		    }
		    وإلا إذا (p == سطر || ((p - سطر) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			وچ_str = TRUE;
		}
		وإلا إذا (!وچ_str && ((p - سطر) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_وچ_string(سطر, (colnr_T)(p - سطر)))
		    اكسر;	// found!
		++p;
	    }
	}
	وإلا
	    p = عدم;
    }
    وإلا
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	لكل ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    إذا (*p == NUL)
		اكسر;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    إذا (p[0] == '/' && p[1] == '/'
				 && (p == سطر || p[-1] != '*' || p[2] != '*'))
		اكسر;
	}
	إذا (*p == NUL)
	    p = عدم;
    }

    إذا (p == عدم)
	أرجع MAXCOL;
    أرجع (صحيح)(p - سطر);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ساكن pos_T *
ind_لبھو_شروع_comment(فراغ)	    // XXX
{
    أرجع لبھو_شروع_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
لبھو_شروع_comment(صحيح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحيح		cur_maxcomment = ind_maxcomment;

    لكل (;;)
    {
	pos = findmatchlimit(عدم, '*', FM_BACKWARD, cur_maxcomment);
	إذا (pos == عدم)
	    اكسر;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	إذا (!is_pos_وچ_string(ml_get(pos->lnum), pos->خانہ))
	    اكسر;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	إذا (cur_maxcomment <= 0)
	{
	    pos = عدم;
	    اكسر;
	}
    }
    أرجع pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ساكن pos_T *
لبھو_شروع_rawstring(صحيح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحيح		cur_maxcomment = ind_maxcomment;

    لكل (;;)
    {
	pos = findmatchlimit(عدم, 'R', FM_BACKWARD, cur_maxcomment);
	إذا (pos == عدم)
	    اكسر;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	إذا (!is_pos_وچ_string(ml_get(pos->lnum), pos->خانہ))
	    اكسر;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	إذا (cur_maxcomment <= 0)
	{
	    pos = عدم;
	    اكسر;
	}
    }
    أرجع pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ساكن pos_T *
ind_لبھو_شروع_CORS(linenr_T *is_raw)	    // XXX
{
    ساكن pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = لبھو_شروع_comment(curbuf->b_ind_maxcomment);
    إذا (comment_pos != عدم)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = لبھو_شروع_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    إذا (comment_pos == عدم || (rs_pos != عدم
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	إذا (is_raw != عدم && rs_pos != عدم)
	    *is_raw = rs_pos->lnum;
	أرجع rs_pos;
    }
    أرجع comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    صحيح
cindent_on(فراغ)
{
    أرجع (!p_paste && (curbuf->b_p_cin
#جے_تعریف_شدہ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#جے_انت
		    ));
}

// Find result cache for cpp_baseclass
تعريفنوع بنية {
    صحيح	    لبھیا;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ساكن char_u *
cin_skipcomment(char_u *s)
{
    طالما (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	إذا (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    اكسر;
	}
	إذا (*s != '/')
	    اكسر;
	++s;
	إذا (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    اكسر;
	}
	إذا (*s != '*')
	    اكسر;
	لكل (++s; *s; ++s)	// skip slash-star comment
	    إذا (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		اكسر;
	    }
    }
    أرجع s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ساكن صحيح
cin_nocode(char_u *s)
{
    أرجع *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ساكن صحيح
cin_iscomment(char_u *p)
{
    أرجع (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ساكن صحيح
cin_islinecomment(char_u *p)
{
    أرجع (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ساكن pos_T *
لبھو_سطر_comment(فراغ) // XXX
{
    ساكن pos_T pos;
    char_u	 *سطر;
    char_u	 *p;

    pos = curwin->w_cursor;
    طالما (--pos.lnum > 0)
    {
	سطر = ml_get(pos.lnum);
	p = skipwhite(سطر);
	إذا (cin_islinecomment(p))
	{
	    pos.خانہ = (صحيح)(p - سطر);
	    أرجع &pos;
	}
	إذا (*p != NUL)
	    اكسر;
    }
    أرجع عدم;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ساكن صحيح
cin_has_js_کلید(char_u *متن)
{
    char_u *s = skipwhite(متن);
    صحيح	    quote = -1;

    إذا (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    إذا (!vim_isIDc(*s))	    // need at least one ID character
	أرجع FALSE;

    طالما (vim_isIDc(*s))
	++s;
    إذا (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    أرجع (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ساكن صحيح
cin_islabel_skip(char_u **s)
{
    إذا (!vim_isIDc(**s))	    // need at least one ID character
	أرجع FALSE;

    طالما (vim_isIDc(**s))
    {
	إذا (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	وإلا
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    أرجع (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ساكن صحيح
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    صحيح	    لبھیا = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    إذا (cinsd_buf == عدم)
	أرجع FALSE;

    لكل (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (صحيح)cinsd_len, ",");
	إذا (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    إذا (*skip == ':' && skip[1] != ':')
	    {
		لبھیا = TRUE;
		اكسر;
	    }
	}
    }

    vim_free(cinsd_buf);
    أرجع لبھیا;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ساكن صحيح
cin_ispreproc(char_u *s)
{
    إذا (*skipwhite(s) == '#')
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ساكن صحيح
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, صحيح *amount)
{
    char_u	*سطر = *pp;
    linenr_T	lnum = *lnump;
    صحيح		retval = FALSE;
    صحيح		candidate_amount = *amount;

    إذا (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    لكل (;;)
    {
	إذا (cin_ispreproc(سطر))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    اكسر;
	}
	إذا (lnum == 1)
	    اكسر;
	سطر = ml_get(--lnum);
	إذا (*سطر == NUL || سطر[STRLEN(سطر) - 1] != '\\')
	    اكسر;
    }

    إذا (lnum != *lnump)
	*pp = ml_get(*lnump);
    إذا (retval)
	*amount = candidate_amount;
    أرجع retval;
}

    ساكن صحيح
cin_iselse(
    char_u  *p)
{
    إذا (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    أرجع (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    ساكن صحيح
cin_isterminated(
    char_u	*s,
    صحيح		incl_open,	// include '{' at the end as terminator
    صحيح		incl_comma)	// recognize a trailing comma
{
    char_u	لبھیا_شروع = 0;
    بنانشانی	n_open = 0;
    صحيح		is_else = FALSE;

    s = cin_skipcomment(s);

    إذا (*s == '{' || (*s == '}' && !cin_iselse(s)))
	لبھیا_شروع = *s;

    إذا (!لبھیا_شروع)
	is_else = cin_iselse(s);

    طالما (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	إذا (*s == '}' && n_open > 0)
	    --n_open;
	إذا ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    أرجع *s;
	وإلا إذا (*s == '{')
	{
	    إذا (incl_open && cin_nocode(s + 1))
		أرجع *s;
	    وإلا
		++n_open;
	}

	إذا (*s)
	    s++;
    }
    أرجع لبھیا_شروع;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ساكن صحيح
cin_starts_with(char_u *s, حرف *لفظ)
{
    صحيح l = (صحيح)STRLEN(لفظ);

    أرجع (STRNCMP(s, لفظ, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ساكن صحيح
cin_isdefault(char_u *s)
{
    أرجع (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ساكن صحيح
cin_iscase(
    char_u *s,
    صحيح strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    إذا (cin_starts_with(s, "case"))
    {
	لكل (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    إذا (*s == NUL)
		اكسر;
	    إذا (*s == ':')
	    {
		إذا (s[1] == ':')	// skip over "::" for C++
		    ++s;
		وإلا
		    أرجع TRUE;
	    }
	    إذا (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    وإلا إذا (*s == '/' && (s[1] == '*' || s[1] == '/'))
		أرجع FALSE;		// stop at comment
	    وإلا إذا (*s == '"')
	    {
		// JS etc.
		إذا (strict)
		    أرجع FALSE;		// stop at string
		وإلا
		    أرجع TRUE;
	    }
	}
	أرجع FALSE;
    }

    إذا (cin_isdefault(s))
	أرجع TRUE;
    أرجع FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ساكن صحيح
cin_islabel(فراغ)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    إذا (cin_isdefault(s))
	أرجع FALSE;
    إذا (cin_isscopedecl(s))
	أرجع FALSE;

    إذا (!cin_islabel_skip(&s))
	أرجع FALSE;

    إذا (ind_لبھو_شروع_CORS(عدم))
	أرجع FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*سطر;

    cursor_save = curwin->w_cursor;
    طالما (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.خانہ = 0;
	إذا ((trypos = ind_لبھو_شروع_CORS(عدم)) != عدم) // XXX
	    curwin->w_cursor = *trypos;

	سطر = ml_get_curline();
	إذا (cin_ispreproc(سطر))	// ignore #defines, #if, etc.
	    استمر;
	إذا (*(سطر = cin_skipcomment(سطر)) == NUL)
	    استمر;

	curwin->w_cursor = cursor_save;
	إذا (cin_isterminated(سطر, TRUE, FALSE)
		|| cin_isscopedecl(سطر)
		|| cin_iscase(سطر, TRUE)
		|| (cin_islabel_skip(&سطر) && cin_nocode(سطر)))
	    أرجع TRUE;
	أرجع FALSE;
    }
    curwin->w_cursor = cursor_save;
    أرجع TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ساكن صحيح
cin_سرے_وچ(char_u *s, char_u *لبھو)
{
    char_u	*p = s;
    char_u	*r;
    صحيح		len = (صحيح)STRLEN(لبھو);

    طالما (*p != NUL)
    {
	p = cin_skipcomment(p);
	إذا (STRNCMP(p, لبھو, len) == 0)
	{
	    r = skipwhite(p + len);
	    إذا (cin_nocode(r))
		أرجع TRUE;
	}
	إذا (*p != NUL)
	    ++p;
    }
    أرجع FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ساكن char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = عدم, *p = s;
    افعل
    {
	r = p;
	p = cin_skipcomment(p);
	إذا (*p)
	    p = skip_string(p);
    } طالما (p != r);
    أرجع p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ساكن صحيح
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = عدم;

    طالما (*p)
    {
	إذا (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	وإلا إذا (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	وإلا
	    p = cin_skip_comment_and_string(p + 1);
    }
    إذا (!r)
	أرجع FALSE;
    p = r; // p points now after '=' or "return"

    إذا (cin_nocode(p))
	أرجع TRUE;

    إذا (*p == '&')
	p = cin_skipcomment(p + 1);

    إذا (*p == '(') // skip a typecast
    {
	صحيح open_گنتی = 1;
	افعل
	{
	    p = cin_skip_comment_and_string(p + 1);
	    إذا (cin_nocode(p))
		أرجع TRUE;
	    open_گنتی += (*p == '(') - (*p == ')');
	} طالما (open_گنتی);
	p = cin_skipcomment(p + 1);
	إذا (cin_nocode(p))
	    أرجع TRUE;
    }

    طالما (*p == '{')
	p = cin_skipcomment(p + 1);
    أرجع cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ساكن صحيح
cin_isinit(فراغ)
{
    char_u	*s;
    ساكن حرف *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    إذا (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    لكل (;;)
    {
	صحيح i, l;

	لكل (i = 0; i < (صحيح)ARRAY_لمائی(skip); ++i)
	{
	    l = (صحيح)لڑی_لمبائی(skip[i]);
	    إذا (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		اكسر;
	    }
	}
	إذا (l != 0)
	    اكسر;
    }

    إذا (cin_starts_with(s, "enum"))
	أرجع TRUE;

    أرجع cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#تعریف_کرو لبھو_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ساكن صحيح
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    صحيح		has_name = FALSE;
    صحيح		has_name_شروع = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    طالما ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    إذا (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	طالما (*p != NUL)
	{
	    إذا (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    وإلا إذا (*p == '{')
	    {
		اكسر;
	    }
	    وإلا إذا (vim_iswordc(*p))
	    {
		has_name_شروع = TRUE;
		إذا (has_name)
		    أرجع FALSE; // word character after skipping past name
		++p;
	    }
	    وإلا إذا (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		إذا (!has_name_شروع || has_name)
		    أرجع FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    وإلا
	    {
		أرجع FALSE;
	    }
	}
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ساكن صحيح
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    صحيح		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    إذا (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	طالما (*p != NUL)
	{
	    إذا (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    وإلا إذا (*p == '{')
	    {
		اكسر;
	    }
	    وإلا إذا (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		إذا (has_string_literal)
		    أرجع FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    وإلا إذا (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		إذا (has_string_literal)
		    أرجع FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    وإلا
	    {
		أرجع FALSE;
	    }
	}
	أرجع has_string_literal ? TRUE : FALSE;
    }
    أرجع FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ساكن char_u *
after_label(char_u *l)
{
    لكل ( ; *l; ++l)
    {
	إذا (*l == ':')
	{
	    إذا (l[1] == ':')	    // skip over "::" for C++
		++l;
	    وإلا إذا (!cin_iscase(l + 1, FALSE))
		اكسر;
	}
	وإلا إذا (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    إذا (*l == NUL)
	أرجع عدم;
    l = cin_skipcomment(l + 1);
    إذا (*l == NUL)
	أرجع عدم;
    أرجع l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ساكن صحيح
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	خانہ;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    إذا (p == عدم)
	أرجع 0;

    fp.خانہ = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &خانہ, عدم, عدم, 0);
    أرجع (صحيح)خانہ;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ساكن صحيح
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    صحيح		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    إذا (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	إذا (l == عدم)		// just in case
	    l = ml_get_curline();
    }
    وإلا
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    أرجع amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ساكن صحيح
cin_پہلا_id_amount(فراغ)
{
    char_u	*سطر, *p, *s;
    صحيح		len;
    pos_T	fp;
    colnr_T	خانہ;

    سطر = ml_get_curline();
    p = skipwhite(سطر);
    len = (صحيح)(skiptowhite(p) - p);
    إذا (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (صحيح)(skiptowhite(p) - p);
    }
    إذا (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    وإلا إذا (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    وإلا إذا ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	إذا ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    لكل (len = 0; vim_isIDc(p[len]); ++len)
	;
    إذا (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	أرجع 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.خانہ = (colnr_T)(p - سطر);
    getvcol(curwin, &fp, &خانہ, عدم, عدم, 0);
    أرجع (صحيح)خانہ;
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
    ساكن صحيح
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*سطر;
    char_u	*s;
    colnr_T	خانہ;
    pos_T	fp;

    إذا (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	إذا (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	    أرجع -1;
    }

    سطر = s = ml_get(lnum);
    طالما (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == عدم)
    {
	إذا (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وإلا
	    ++s;
    }
    إذا (*s != '=')
	أرجع 0;

    s = skipwhite(s + 1);
    إذا (cin_nocode(s))
	أرجع 0;

    إذا (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.خانہ = (colnr_T)(s - سطر);
    getvcol(curwin, &fp, &خانہ, عدم, عدم, 0);
    أرجع (صحيح)خانہ;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ساكن صحيح
cin_skip2pos(pos_T *trypos)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*new_p;

    p = سطر = ml_get(trypos->lnum);
    طالما (*p && (colnr_T)(p - سطر) < trypos->خانہ)
    {
	إذا (cin_iscomment(p))
	    p = cin_skipcomment(p);
	وإلا
	{
	    new_p = skip_string(p);
	    إذا (new_p == p)
		++p;
	    وإلا
		p = new_p;
	}
    }
    أرجع (صحيح)(p - سطر);
}

    ساكن pos_T *
لبھو_میل_char(صحيح c, صحيح ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ساكن pos_T pos_copy;
    صحيح		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    إذا ((trypos = findmatchlimit(عدم, c, FM_SKIPCOMM, ind_maxp_wk)) != عدم)
    {
	// check if the ( is in a // comment
	إذا ((colnr_T)cin_skip2pos(trypos) > trypos->خانہ)
	{
	    ind_maxp_wk = ind_maxparen - (صحيح)(cursor_save.lnum - trypos->lnum);
	    إذا (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.خانہ = 0;	// XXX
		اذهب retry;
	    }
	    trypos = عدم;
	}
	وإلا
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    إذا ((trypos_wk = ind_لبھو_شروع_CORS(عدم)) != عدم) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (صحيح)(cursor_save.lnum
			- trypos_wk->lnum);
		إذا (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    اذهب retry;
		}
		trypos = عدم;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    أرجع trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ساكن pos_T *
لبھو_میل_paren(صحيح ind_maxparen)	// XXX
{
    أرجع لبھو_میل_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ساكن صحيح
لبھو_آخری_paren(char_u *l, صحيح شروع, صحيح سرا)
{
    صحيح		i;
    صحيح		retval = FALSE;
    صحيح		open_گنتی = 0;

    curwin->w_cursor.خانہ = 0;		    // default is start of line

    لكل (i = 0; l[i] != NUL; i++)
    {
	i = (صحيح)(cin_skipcomment(l + i) - l); // ignore parens in comments
	إذا (l[i] == NUL)
	    اكسر;
	i = (صحيح)(skip_string(l + i) - l);    // ignore parens in quotes
	إذا (l[i] == شروع)
	    ++open_گنتی;
	وإلا إذا (l[i] == سرا)
	{
	    إذا (open_گنتی > 0)
		--open_گنتی;
	    وإلا
	    {
		curwin->w_cursor.خانہ = i;
		retval = TRUE;
	    }
	}
    }
    أرجع retval;
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
    ساكن صحيح
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	پہلا_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = پہلا_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    صحيح		retval = FALSE;
    pos_T	*trypos;
    صحيح		just_started = TRUE;

    إذا (sp == عدم)
	s = ml_get(lnum);
    وإلا
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    إذا (لبھو_آخری_paren(s, '(', ')')
	&& (trypos = لبھو_میل_paren(curbuf->b_ind_maxparen)) != عدم)
    {
	lnum = trypos->lnum;
	إذا (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    أرجع FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    إذا (cin_ispreproc(s))
	أرجع FALSE;

    طالما (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	إذا (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وإلا إذا (*s == ':')
	{
	    إذا (*(s + 1) == ':')
		s += 2;
	    وإلا
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		أرجع FALSE;
	}
	وإلا
	    ++s;
    }
    إذا (*s != '(')
	أرجع FALSE;		// ';', ' or "  before any () or no '('

    طالما (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	إذا (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = پہلا_lnum - 1;
	    s = ml_get(lnum);
	    إذا (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    اذهب پورا;
	}
	إذا ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    صحيح comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    لكل (;;)
	    {
		إذا (lnum >= curbuf->b_ml.ml_سطر_گنتی)
		    اكسر;
		s = ml_get(++lnum);
		إذا (!cin_ispreproc(s))
		    اكسر;
	    }
	    إذا (lnum >= curbuf->b_ml.ml_سطر_گنتی)
		اكسر;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    إذا (!just_started && (!comma && *s != ',' && *s != ')'))
		اكسر;
	    just_started = FALSE;
	}
	وإلا إذا (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	وإلا
	{
	    ++s;
	    just_started = FALSE;
	}
    }

پورا:
    إذا (lnum != پہلا_lnum && sp != عدم)
	*sp = ml_get(پہلا_lnum);

    أرجع retval;
}

    ساكن صحيح
cin_isif(char_u *p)
{
    أرجع (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ساكن صحيح
cin_isdo(char_u *p)
{
    أرجع (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ساكن صحيح
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    صحيح		retval = FALSE;

    p = cin_skipcomment(p);
    إذا (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    إذا (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.خانہ = 0;
	p = ml_get_curline();
	طالما (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.خانہ;
	}
	إذا ((trypos = findmatchlimit(عدم, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != عدم
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    أرجع retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ساكن صحيح
cin_is_if_for_while_before_وتھ(char_u *سطر, صحيح *poffset)
{
    صحيح وتھ = *poffset;

    إذا (وتھ-- < 2)
	أرجع 0;
    طالما (وتھ > 2 && VIM_ISWHITE(سطر[وتھ]))
	--وتھ;

    وتھ -= 1;
    إذا (STRNCMP(سطر + وتھ, "if", 2) == 0)
	اذهب probably_لبھیا;

    إذا (وتھ >= 1)
    {
	وتھ -= 1;
	إذا (STRNCMP(سطر + وتھ, "for", 3) == 0)
	    اذهب probably_لبھیا;

	إذا (وتھ >= 2)
	{
	    وتھ -= 2;
	    إذا (STRNCMP(سطر + وتھ, "while", 5) == 0)
		اذهب probably_لبھیا;
	}
    }
    أرجع 0;

probably_لبھیا:
    إذا (!وتھ || !vim_isIDc(سطر[وتھ - 1]))
    {
	*poffset = وتھ;
	أرجع 1;
    }
    أرجع 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ساكن صحيح
cin_iswhileofdo_سرا(صحيح terminated)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    صحيح		i;

    إذا (terminated != ';')	// there must be a ';' at the end
	أرجع FALSE;

    p = سطر = ml_get_curline();
    طالما (*p != NUL)
    {
	p = cin_skipcomment(p);
	إذا (*p == ')')
	{
	    s = skipwhite(p + 1);
	    إذا (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (صحيح)(p - سطر);
		curwin->w_cursor.خانہ = i;
		trypos = لبھو_میل_paren(curbuf->b_ind_maxparen);
		إذا (trypos != عدم)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    إذا (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    إذا (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			أرجع TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		سطر = ml_get_curline();
		p = سطر + i;
	    }
	}
	إذا (*p != NUL)
	    ++p;
    }
    أرجع FALSE;
}

    ساكن صحيح
cin_isbreak(char_u *p)
{
    أرجع (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    ساكن صحيح
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    صحيح		class_or_struct, lookfor_ctor_init, cpp_ادھار_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*سطر = ml_get_curline();

    إذا (pos->lnum <= lnum)
	أرجع cached->لبھیا;	// Use the cached result

    pos->خانہ = 0;

    s = skipwhite(سطر);
    إذا (*s == '#')		// skip #define FOO x ? (x) : x
	أرجع FALSE;
    s = cin_skipcomment(s);
    إذا (*s == NUL)
	أرجع FALSE;

    cpp_ادھار_class = lookfor_ctor_init = class_or_struct = FALSE;

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
    طالما (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	s = skipwhite(سطر);
	إذا (*s == '#' || *s == NUL)
	    اكسر;
	طالما (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    إذا (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		اكسر;
	    إذا (*s != NUL)
		++s;
	}
	إذا (*s != NUL)
	    اكسر;
	--lnum;
    }

    pos->lnum = lnum;
    سطر = ml_get(lnum);
    s = سطر;
    لكل (;;)
    {
	إذا (*s == NUL)
	{
	    إذا (lnum == curwin->w_cursor.lnum)
		اكسر;
	    // Continue in the cursor line.
	    سطر = ml_get(++lnum);
	    s = سطر;
	}
	إذا (s == سطر)
	{
	    // don't recognize "case (foo):" as a baseclass
	    إذا (cin_iscase(s, FALSE))
		اكسر;
	    s = cin_skipcomment(سطر);
	    إذا (*s == NUL)
		استمر;
	}

	إذا (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	وإلا إذا (s[0] == ':')
	{
	    إذا (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    وإلا إذا (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_ادھار_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->خانہ = 0;
		s = cin_skipcomment(s + 1);
	    }
	    وإلا
		s = cin_skipcomment(s + 1);
	}
	وإلا إذا ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    إذا (*s == 'c')
		s = cin_skipcomment(s + 5);
	    وإلا
		s = cin_skipcomment(s + 6);
	}
	وإلا
	{
	    إذا (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_ادھار_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    وإلا إذا (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    وإلا إذا (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		أرجع FALSE;
	    }
	    وإلا إذا (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    وإلا إذا (pos->خانہ == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		إذا (cpp_ادھار_class)
		    pos->خانہ = (colnr_T)(s - سطر);
	    }

	    // When the line ends in a comma don't align with it.
	    إذا (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->خانہ = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->لبھیا = cpp_ادھار_class;
    إذا (cpp_ادھار_class)
	pos->lnum = lnum;
    أرجع cpp_ادھار_class;
}

    ساكن صحيح
get_baseclass_amount(صحيح خانہ)
{
    صحيح		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    إذا (خانہ == 0)
    {
	amount = get_indent();
	إذا (لبھو_آخری_paren(ml_get_curline(), '(', ')')
		&& (trypos = لبھو_میل_paren(curbuf->b_ind_maxparen)) != عدم)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	إذا (!cin_سرے_وچ(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    وإلا
    {
	curwin->w_cursor.خانہ = خانہ;
	getvcol(curwin, &curwin->w_cursor, &vcol, عدم, عدم, 0);
	amount = (صحيح)vcol;
    }
    إذا (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    أرجع amount;
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

    ساكن pos_T *
لبھو_شروع_brace(فراغ)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ساكن pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    طالما ((trypos = findmatchlimit(عدم, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != عدم)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = عدم;
	// ignore the { if it's in a // or / *  * / comment
	إذا ((colnr_T)cin_skip2pos(trypos) == trypos->خانہ
		       && (pos = ind_لبھو_شروع_CORS(عدم)) == عدم) // XXX
	    اكسر;
	إذا (pos != عدم)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    أرجع trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ساكن pos_T *
لبھو_میل_paren_after_brace (صحيح ind_maxparen)	    // XXX
{
    pos_T	*trypos = لبھو_میل_paren(ind_maxparen);

    إذا (trypos == عدم)
	أرجع عدم;

    pos_T	*tryposBrace = لبھو_شروع_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    إذا (tryposBrace != عدم
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->خانہ < tryposBrace->خانہ))
	trypos = عدم;
    أرجع trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ساكن صحيح
corr_ind_maxparen(pos_T *startpos)
{
    لما	n = (لما)startpos->lnum - (لما)curwin->w_cursor.lnum;

    إذا (n > 0 && n < curbuf->b_ind_maxparen / 2)
	أرجع curbuf->b_ind_maxparen - (صحيح)n;
    أرجع curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    فراغ
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    صحيح		divider;
    صحيح		fraction = 0;
    صحيح		sw;
    لما	t = get_sw_قدر(buf);

    // needed for cino-(, it will be multiplied by 2 again
    إذا (t > ودھ_توں_ودھ_عدد / 2)
	sw = ودھ_توں_ودھ_عدد / 2;
    وإلا
	sw = (صحيح)t;

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
    buf->b_ind_پہلا_open = 0;

    // Spaces from the prevailing indent a leftmost open brace should be
    // located.
    buf->b_ind_open_extra = 0;

    // Spaces from the matching open brace (real location for one at the left
    // edge; imaginary location from one that ends a line) the matching close
    // brace should be located.
    buf->b_ind_close_extra = 0;

    // Spaces from the edge of the line an open brace sitting in the leftmost
    // column is imagined to be.
    buf->b_ind_open_بایاں_imag = 0;

    // Spaces jump labels should be shifted to the left if N is non-negative,
    // otherwise the jump label will be put to column 1.
    buf->b_ind_jump_label = -1;

    // Spaces from the switch() indent a "case xx" label should be located.
    buf->b_ind_case = sw;

    // Spaces from the "case xx:" code after a switch() should be located.
    buf->b_ind_case_کوڈ = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_کوڈ = sw;

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
    buf->b_ind_وچ_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_وچ_comment2 = 0;

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

    لكل (p = buf->b_p_cino; *p; )
    {
	l = p++;
	إذا (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	إذا (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((حرف *)++p);
	    طالما (VIM_ISDIGIT(*p))
	    {
		++p;
		إذا (divider)
		    divider *= 10;
		وإلا
		    divider = 10;
	    }
	}
	إذا (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    إذا (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    وإلا
	    {
		n *= sw;
		إذا (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	إذا (l[1] == '-')
	    n = -n;

	n = trim_منزل_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	بدل (*l)
	{
	    حالة '>': buf->b_ind_level = (صحيح)n; اكسر;
	    حالة 'e': buf->b_ind_open_imag = (صحيح)n; اكسر;
	    حالة 'n': buf->b_ind_no_brace = (صحيح)n; اكسر;
	    حالة 'f': buf->b_ind_پہلا_open = (صحيح)n; اكسر;
	    حالة '{': buf->b_ind_open_extra = (صحيح)n; اكسر;
	    حالة '}': buf->b_ind_close_extra = (صحيح)n; اكسر;
	    حالة '^': buf->b_ind_open_بایاں_imag = (صحيح)n; اكسر;
	    حالة 'L': buf->b_ind_jump_label = (صحيح)n; اكسر;
	    حالة ':': buf->b_ind_case = (صحيح)n; اكسر;
	    حالة '=': buf->b_ind_case_کوڈ = (صحيح)n; اكسر;
	    حالة 'b': buf->b_ind_case_break = (صحيح)n; اكسر;
	    حالة 'p': buf->b_ind_param = (صحيح)n; اكسر;
	    حالة 't': buf->b_ind_func_type = (صحيح)n; اكسر;
	    حالة '/': buf->b_ind_comment = (صحيح)n; اكسر;
	    حالة 'c': buf->b_ind_وچ_comment = (صحيح)n; اكسر;
	    حالة 'C': buf->b_ind_وچ_comment2 = (صحيح)n; اكسر;
	    حالة 'i': buf->b_ind_cpp_baseclass = (صحيح)n; اكسر;
	    حالة '+': buf->b_ind_continuation = (صحيح)n; اكسر;
	    حالة '(': buf->b_ind_unclosed = (صحيح)n; اكسر;
	    حالة 'u': buf->b_ind_unclosed2 = (صحيح)n; اكسر;
	    حالة 'U': buf->b_ind_unclosed_noignore = (صحيح)n; اكسر;
	    حالة 'W': buf->b_ind_unclosed_wrapped = (صحيح)n; اكسر;
	    حالة 'w': buf->b_ind_unclosed_whiteok = (صحيح)n; اكسر;
	    حالة 'm': buf->b_ind_matching_paren = (صحيح)n; اكسر;
	    حالة 'M': buf->b_ind_paren_prev = (صحيح)n; اكسر;
	    حالة ')': buf->b_ind_maxparen = (صحيح)n; اكسر;
	    حالة '*': buf->b_ind_maxcomment = (صحيح)n; اكسر;
	    حالة 'g': buf->b_ind_scopedecl = (صحيح)n; اكسر;
	    حالة 'h': buf->b_ind_scopedecl_کوڈ = (صحيح)n; اكسر;
	    حالة 'j': buf->b_ind_java = (صحيح)n; اكسر;
	    حالة 'J': buf->b_ind_js = (صحيح)n; اكسر;
	    حالة 'l': buf->b_ind_keep_case_label = (صحيح)n; اكسر;
	    حالة '#': buf->b_ind_hash_comment = (صحيح)n; اكسر;
	    حالة 'N': buf->b_ind_cpp_namespace = (صحيح)n; اكسر;
	    حالة 'k': buf->b_ind_if_for_while = (صحيح)n; اكسر;
	    حالة 'E': buf->b_ind_cpp_extern_c = (صحيح)n; اكسر;
	    حالة 'P': buf->b_ind_pragma = (صحيح)n; اكسر;
	}
	إذا (*p == ',')
	    ++p;
    }
}

    ساكن صحيح
لبھو_میل(صحيح lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    صحيح		elselevel;
    صحيح		whilelevel;

    إذا (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    وإلا
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.خانہ = 0;

    طالما (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.خانہ = 0;

	look = cin_skipcomment(ml_get_curline());
	إذا (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = لبھو_شروع_brace();  // XXX
	    إذا (theirscope == عدم)
		اكسر;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    إذا (theirscope->lnum < ourscope)
		اكسر;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    إذا (theirscope->lnum > ourscope)
		استمر;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    إذا (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		إذا (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    إذا (!cin_isif(mightbeif))
			++elselevel;
		    استمر;
		}

		// If it's an "if" decrement elselevel
		إذا (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    إذا (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    إذا (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		استمر;
	    }

	    // If it's a "do" decrement whilelevel
	    إذا (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    إذا (elselevel <= 0 && whilelevel <= 0)
		أرجع OK;
	}
    }
    أرجع FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    صحيح
get_c_indent(فراغ)
{
    pos_T	cur_curpos;
    صحيح		amount;
    صحيح		scope_amount;
    صحيح		cur_amount = MAXCOL;
    colnr_T	خانہ;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = عدم;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*شروع;
    صحيح		شروع_brace;
#تعریف_کرو BRACE_وچ_COL0		1	    // '{' is in column 0
#تعریف_کرو BRACE_AT_شروع		2	    // '{' is at start of line
#تعریف_کرو BRACE_AT_سرا		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    صحيح		lookfor;
    صحيح		whilelevel;
    linenr_T	lnum;
    صحيح		n;
    صحيح		iscase;
    صحيح		lookfor_break;
    صحيح		lookfor_cpp_namespace = FALSE;
    صحيح		cont_amount = 0;    // amount for continuation line
    صحيح		original_سطر_islabel;
    صحيح		added_منزل_amount = 0;
    صحيح		js_cur_has_کلید = 0;
    linenr_T	raw_string_شروع = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    صحيح		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    إذا (cur_curpos.lnum == 1)
	أرجع 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    إذا (linecopy == عدم)
	أرجع 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    إذا ((State & منوال_داخل_کرو)
	    && curwin->w_cursor.خانہ < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.خانہ] == ')')
	linecopy[curwin->w_cursor.خانہ] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.خانہ = 0;

    original_سطر_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_لبھو_شروع_comment();
    إذا (comment_pos != عدم)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = لبھو_شروع_rawstring(curbuf->b_ind_maxcomment);
    إذا (trypos != عدم && (comment_pos == عدم
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	اذهب laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    إذا (*theline == '#' && (*linecopy == '#' || وچ_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	إذا (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    اذهب theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    إذا (original_سطر_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	اذهب theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    إذا (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = لبھو_سطر_comment(); // XXX
	إذا (trypos == عدم && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.خانہ =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    إذا (linecomment_pos.خانہ != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	إذا (trypos  != عدم)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &خانہ, عدم, عدم, 0);
	    amount = خانہ;
	    اذهب theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    إذا (!cin_iscomment(theline) && comment_pos != عدم) // XXX
    {
	صحيح	lead_شروع_len = 2;
	صحيح	lead_درمیان_len = 1;
	char_u	lead_شروع[COM_MAX_LEN];	// start-comment string
	char_u	lead_درمیان[COM_MAX_LEN];	// middle-comment string
	char_u	lead_سرا[COM_MAX_LEN];		// end-comment string
	صحيح	lead_سرا_len;
	char_u	*p;
	صحيح	شروع_align = 0;
	صحيح	شروع_off = 0;
	صحيح	پورا = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &خانہ, عدم, عدم, 0);
	amount = خانہ;
	*lead_شروع = NUL;
	*lead_درمیان = NUL;

	p = curbuf->b_p_com;
	طالما (*p != NUL)
	{
	    صحيح	align = 0;
	    صحيح	off = 0;
	    صحيح what = 0;

	    طالما (*p != NUL && *p != ':')
	    {
		إذا (*p == COM_شروع || *p == COM_سرا || *p == COM_درمیان)
		    what = *p++;
		وإلا إذا (*p == COM_بایاں || *p == COM_دایاں)
		    align = *p++;
		وإلا إذا (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		وإلا
		    ++p;
	    }

	    إذا (*p == ':')
		++p;
	    lead_سرا_len = copy_option_part(&p, lead_سرا, COM_MAX_LEN, ",");
	    إذا (what == COM_شروع)
	    {
		STRCPY(lead_شروع, lead_سرا);
		lead_شروع_len = lead_سرا_len;
		شروع_off = off;
		شروع_align = align;
	    }
	    وإلا إذا (what == COM_درمیان)
	    {
		STRCPY(lead_درمیان, lead_سرا);
		lead_درمیان_len = lead_سرا_len;
	    }
	    وإلا إذا (what == COM_سرا)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		إذا (STRNCMP(theline, lead_درمیان, lead_درمیان_len) == 0
			&& STRNCMP(theline, lead_سرا, lead_سرا_len) != 0)
		{
		    پورا = TRUE;
		    إذا (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			إذا (STRNCMP(look, lead_شروع, lead_شروع_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			وإلا إذا (STRNCMP(look, lead_درمیان,
							lead_درمیان_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    اكسر;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			وإلا إذا (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->خانہ,
					     lead_شروع, lead_شروع_len) != 0)
			    استمر;
		    }
		    إذا (شروع_off != 0)
			amount += شروع_off;
		    وإلا إذا (شروع_align == COM_دایاں)
			amount += vim_strsize(lead_شروع)
						   - vim_strsize(lead_درمیان);
		    اكسر;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		إذا (STRNCMP(theline, lead_درمیان, lead_درمیان_len) != 0
			&& STRNCMP(theline, lead_سرا, lead_سرا_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    إذا (off != 0)
			amount += off;
		    وإلا إذا (align == COM_دایاں)
			amount += vim_strsize(lead_شروع)
						   - vim_strsize(lead_درمیان);
		    پورا = TRUE;
		    اكسر;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	إذا (پورا)
	    ;
	وإلا إذا (theline[0] == '*')
	    amount += 1;
	وإلا
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    لكل (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		إذا (linewhite(lnum))		    // skip blank lines
		    استمر;
		amount = get_indent_lnum(lnum);	    // XXX
		اكسر;
	    }
	    إذا (amount == -1)			    // use the comment opener
	    {
		إذا (!curbuf->b_ind_وچ_comment2)
		{
		    شروع = ml_get(comment_pos->lnum);
		    look = شروع + comment_pos->خانہ + 2; // skip / and *
		    إذا (*look != NUL)		    // if something after it
			comment_pos->خانہ = (colnr_T)(skipwhite(look) - شروع);
		}
		getvcol(curwin, comment_pos, &خانہ, عدم, عدم, 0);
		amount = خانہ;
		إذا (curbuf->b_ind_وچ_comment2 || *look == NUL)
		    amount += curbuf->b_ind_وچ_comment;
	    }
	}
	اذهب theend;
    }

    // Are we looking at a ']' that has a match?
    إذا (*skipwhite(theline) == ']'
	    && (trypos = لبھو_میل_char('[', curbuf->b_ind_maxparen)) != عدم)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	اذهب theend;
    }

    // Are we inside parentheses or braces?  XXX
    إذا (((trypos = لبھو_میل_paren(curbuf->b_ind_maxparen)) != عدم
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = لبھو_شروع_brace()) != عدم
	    || trypos != عدم)
    {
      إذا (trypos != عدم && tryposBrace != عدم)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  إذا (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->خانہ < tryposBrace->خانہ)
	      trypos = عدم;
	  وإلا
	      tryposBrace = عدم;
      }

      إذا (trypos != عدم)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	إذا (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	وإلا
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    لكل (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		إذا (cin_nocode(l))		// skip comment lines
		    استمر;
		إذا (cin_ispreproc_cont(&l, &lnum, &amount))
		    استمر;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		إذا ((trypos = ind_لبھو_شروع_CORS(عدم)) != عدم)
		{
		    lnum = trypos->lnum + 1;
		    استمر;
		}

		// XXX
		إذا ((trypos = لبھو_میل_paren(
			corr_ind_maxparen(&cur_curpos))) != عدم
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->خانہ == our_paren_pos.خانہ)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    إذا (theline[0] == ')')
		    {
			إذا (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    اكسر;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	إذا (amount == -1)
	{
	    صحيح	    ignore_paren_خانہ = 0;
	    صحيح	    is_if_for_while = 0;

	    إذا (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *سطر;

		trypos = &our_paren_pos;
		افعل
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.خانہ = outermost.خانہ;

		    trypos = لبھو_میل_paren(curbuf->b_ind_maxparen);
		} طالما (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		سطر = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_وتھ(سطر, &outermost.خانہ);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    إذا (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *سطر;
		صحيح	    look_خانہ;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		سطر = ml_get_curline();
		look_خانہ = (صحيح)(look - سطر);
		curwin->w_cursor.خانہ = look_خانہ + 1;
		إذا ((trypos = findmatchlimit(عدم, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != عدم
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->خانہ < our_paren_pos.خانہ)
		    ignore_paren_خانہ = trypos->خانہ + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_خانہ;
	    }
	    إذا (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_خانہ == 0))
	    {
		// If we're looking at a close paren, line up right there;
		// otherwise, line up with the next (non-white) character.
		// When b_ind_unclosed_wrapped is set and the matching paren is
		// the last nonwhite character of the line, use either the
		// indent of the current line or the indentation of the next
		// outer paren and add b_ind_unclosed_wrapped (for very long
		// lines).
		إذا (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    إذا (curbuf->b_ind_unclosed_wrapped
				       && cin_سرے_وچ(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			لكل (خانہ = 0; خانہ < our_paren_pos.خانہ; ++خانہ)
			{
			    بدل (l[خانہ])
			    {
				حالة '(':
				حالة '{': ++n;
					  اكسر;

				حالة ')':
				حالة '}': إذا (n > 1)
					      --n;
					  اكسر;
			    }
			}

			our_paren_pos.خانہ = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    وإلا إذا (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.خانہ++;
		    وإلا
		    {
			خانہ = our_paren_pos.خانہ + 1;
			طالما (VIM_ISWHITE(l[خانہ]))
			    خانہ++;
			إذا (l[خانہ] != NUL)	// In case of trailing space
			    our_paren_pos.خانہ = خانہ;
			وإلا
			    our_paren_pos.خانہ++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		إذا (our_paren_pos.خانہ > 0)
		{
		    getvcol(curwin, &our_paren_pos, &خانہ, عدم, عدم, 0);
		    إذا (cur_amount > (صحيح)خانہ)
			cur_amount = خانہ;
		}
	    }

	    إذا (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    وإلا إذا ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_خانہ == 0))
	    {
		إذا (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    وإلا
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		خانہ = our_paren_pos.خانہ;
		طالما ((صحيح)our_paren_pos.خانہ > ignore_paren_خانہ)
		{
		    --our_paren_pos.خانہ;
		    بدل (*ml_get_pos(&our_paren_pos))
		    {
			حالة '(': amount += curbuf->b_ind_unclosed2;
				  خانہ = our_paren_pos.خانہ;
				  اكسر;
			حالة ')': amount -= curbuf->b_ind_unclosed2;
				  خانہ = MAXCOL;
				  اكسر;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		إذا (خانہ == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		وإلا
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.خانہ = خانہ;
		    إذا (لبھو_میل_paren_after_brace(curbuf->b_ind_maxparen)
								      != عدم)
			amount += curbuf->b_ind_unclosed2;
		    وإلا
		    {
			إذا (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			وإلا
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		إذا (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	إذا (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      وإلا
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	شروع = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(شروع);
	إذا (*look == '{')
	{
	    getvcol(curwin, trypos, &خانہ, عدم, عدم, 0);
	    amount = خانہ;
	    إذا (*شروع == '{')
		شروع_brace = BRACE_وچ_COL0;
	    وإلا
		شروع_brace = BRACE_AT_شروع;
	}
	وإلا
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    إذا (لبھو_آخری_paren(شروع, '(', ')')
			&& (trypos = لبھو_میل_paren(curbuf->b_ind_maxparen))
								      != عدم)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    إذا ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    وإلا إذا (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    وإلا
		amount = skip_label(lnum, &l);

	    شروع_brace = BRACE_AT_سرا;
	}

	// For Javascript check if the line starts with "key:".
	إذا (curbuf->b_ind_js)
	    js_cur_has_کلید = cin_has_js_کلید(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	إذا (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	وإلا
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    إذا (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    وإلا إذا (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    إذا (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		إذا (لبھو_میل(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    اذهب theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    إذا (شروع_brace == BRACE_وچ_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_بایاں_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    وإلا إذا (شروع_brace == BRACE_AT_شروع &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    وإلا
	    {
		إذا (شروع_brace == BRACE_AT_سرا)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    إذا (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    وإلا إذا (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		وإلا
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    إذا (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    إذا (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    وإلا إذا (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    وإلا
	    {
		إذا (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    لكل (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.خانہ = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		إذا (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    إذا (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			إذا (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    إذا (cont_amount > 0)
				amount = cont_amount;
			    وإلا إذا (!curbuf->b_ind_js)
				amount += ind_continuation;
			    اكسر;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_لبھو_شروع_CORS(عدم);
			إذا (trypos != عدم)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.خانہ = 0;
			    استمر;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			إذا (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    استمر;

			إذا (cin_nocode(l))
			    استمر;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			إذا (شروع_brace != BRACE_وچ_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    إذا (terminated == ',')
				اكسر;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    إذا (terminated != ';' && cin_isinit())
				اكسر;

			    // nothing useful found
			    إذا (terminated == 0 || terminated == '{')
				استمر;
			}

			إذا (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = عدم;
			    إذا (لبھو_آخری_paren(l, '(', ')'))
				trypos = لبھو_میل_paren(
						      curbuf->b_ind_maxparen);

			    إذا (trypos == عدم && لبھو_آخری_paren(l, '{', '}'))
				trypos = لبھو_شروع_brace();

			    إذا (trypos != عدم)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.خانہ = 0;
				استمر;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
		    }
		    وإلا إذا (lookfor == LOOKFOR_UNTERM)
		    {
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
		    }
		    وإلا
		    {
			إذا (lookfor != LOOKFOR_رکن
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    إذا (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_منزل_amount = curbuf->b_ind_open_extra;
			    }
			}

			إذا (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    إذا (curwin->w_cursor.lnum == ourscope)
				استمر;

			    إذا (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - لبھو_NAMESPACE_LIM)
				اكسر;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_لبھو_شروع_CORS(عدم);
			    إذا (trypos != عدم)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.خانہ = 0;
				استمر;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    إذا (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				استمر;

			    // Finally the actual check for "namespace".
			    إذا (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_منزل_amount;
				اكسر;
			    }
			    وإلا إذا (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_منزل_amount;
				اكسر;
			    }

			    إذا (cin_nocode(l))
				استمر;
			}
		    }
		    اكسر;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		إذا ((trypos = ind_لبھو_شروع_CORS(&raw_string_شروع)) != عدم)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.خانہ = 0;
		    استمر;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		إذا (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    إذا (lookfor == LOOKFOR_CPP_BASECLASS)
			اكسر;

		    // When looking for a "do" we are not interested in
		    // labels.
		    إذا (whilelevel > 0)
			استمر;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    إذا (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
			اكسر;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    إذا (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			إذا ((trypos = لبھو_شروع_brace()) == عدم
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    اكسر;
			}
			استمر;
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
		    إذا (lookfor == LOOKFOR_رکن)
		    {
			إذا (n)
			    amount = n;

			إذا (!lookfor_break)
			    اكسر;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    إذا (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			إذا (l != عدم && cin_is_cinword(l))
			{
			    إذا (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    وإلا
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			اكسر;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_کوڈ
					: curbuf->b_ind_scopedecl_کوڈ);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    استمر;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		إذا (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    إذا (لبھو_آخری_paren(l, '{', '}')
				     && (trypos = لبھو_شروع_brace()) != عدم)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.خانہ = 0;
		    }
		    استمر;
		}

		// Ignore jump labels with nothing after them.
		إذا (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    إذا (l == عدم || cin_nocode(l))
			استمر;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		إذا (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    استمر;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		إذا (lookfor != LOOKFOR_رکن && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		إذا (n)
		{
		    إذا (lookfor == LOOKFOR_UNTERM)
		    {
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
		    }
		    وإلا إذا (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			استمر;
		    }
		    وإلا
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.خانہ);
		    اكسر;
		}
		وإلا إذا (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    إذا (cin_isterminated(l, TRUE, FALSE))
			اكسر;
		    وإلا
			استمر;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//ایتھے
		// Otherwise check whether it is an enumeration or structure
		// initialisation (not indented) or a variable declaration
		// (indented).
		terminated = cin_isterminated(l, FALSE, TRUE);

		إذا (js_cur_has_کلید)
		{
		    js_cur_has_کلید = 0; // only check the first line
		    إذا (curbuf->b_ind_js && terminated == ',')
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
		إذا (lookfor == LOOKFOR_JS_کلید && cin_has_js_کلید(l))
		{
		    amount = get_indent();
		    اكسر;
		}
		إذا (lookfor == LOOKFOR_COMMA)
		{
		    إذا (tryposBrace != عدم && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			اكسر;
		    إذا (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			اكسر;
		    وإلا
		    {
			amount = get_indent();
			إذا (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    اكسر;
		    }
		}

		إذا (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    إذا (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (فراغ)لبھو_آخری_paren(l, '(', ')');
		    trypos = لبھو_میل_paren(corr_ind_maxparen(&cur_curpos));
		    إذا (trypos != عدم && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->خانہ < tryposBrace->خانہ)))
			trypos = عدم;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    إذا (trypos == عدم && terminated == ',')
		    {
			إذا (لبھو_آخری_paren(l, '{', '}'))
			    trypos = لبھو_شروع_brace();
			l = ml_get_curline();
		    }

		    إذا (trypos != عدم)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			إذا (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.خانہ = 0;
			    استمر;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    إذا (terminated == ',')
		    {
			طالما (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    إذا (*l == NUL || l[STRLEN(l) - 1] != '\\')
				اكسر;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.خانہ = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    إذا (curbuf->b_ind_js)
			cur_amount = get_indent();
		    وإلا
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    إذا (terminated != ',' && lookfor != LOOKFOR_رکن
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			إذا (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			إذا (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    استمر;
			}
			اكسر;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    إذا (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			إذا (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    إذا (cont_amount > 0)
				amount = cont_amount;
			    وإلا
				amount += ind_continuation;
			    اكسر;
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
			إذا (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			إذا (lookfor != LOOKFOR_رکن)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    اكسر;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			إذا (cin_isdo(l))
			{
			    إذا (whilelevel == 0)
				اكسر;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			إذا (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    إذا (*l == '}')
				curwin->w_cursor.خانہ =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    إذا ((trypos = لبھو_شروع_brace()) == عدم
				       || لبھو_میل(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				اكسر;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    وإلا
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			إذا (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    إذا (terminated == ',')
				amount += ind_continuation;
			    اكسر;
			}

			إذا (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    إذا (terminated == ',')
			    {
				إذا (curbuf->b_ind_cpp_baseclass == 0)
				    اكسر;

				lookfor = LOOKFOR_CPP_BASECLASS;
				استمر;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    إذا (amount > cur_amount)
				amount = cur_amount;
			}
			وإلا
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (صحيح)STRLEN(l);
			    إذا (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				اكسر;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    إذا (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				إذا (curbuf->b_ind_js)
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
				    إذا (cin_iscomment(skipwhite(l)))
					اكسر;
				    lookfor = LOOKFOR_COMMA;
				    trypos = لبھو_میل_char('[',
						      curbuf->b_ind_maxparen);
				    إذا (trypos != عدم)
				    {
					إذا (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    اكسر;
					}
					ourscope = trypos->lnum;
				    }
				}
				وإلا
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_پہلا_id_amount();
				}
			    }
			    وإلا
			    {
				إذا (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				إذا (lookfor != LOOKFOR_رکن
						&& lookfor != LOOKFOR_JS_کلید
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_شروع != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		وإلا إذا (cin_iswhileofdo_سرا(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    إذا (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
			اكسر;
		    }

		    إذا (whilelevel == 0)
		    {
			lookfor = LOOKFOR_رکن;
			amount = get_indent();	    // XXX
			إذا (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		وإلا
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    إذا (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			استمر;
		    }

		    // Handle "do {" line.
		    إذا (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			إذا (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    استمر;
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
		    إذا (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			إذا (cont_amount > 0)
			    amount = cont_amount;
			وإلا
			    amount += ind_continuation;
			اكسر;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    إذا (lookfor == LOOKFOR_رکن)
		    {
			إذا (!lookfor_break && whilelevel == 0)
			    اكسر;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    وإلا
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
رکن_again:
			l = ml_get_curline();
			إذا (لبھو_آخری_paren(l, '(', ')')
				&& (trypos = لبھو_میل_paren(
					   curbuf->b_ind_maxparen)) != عدم)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    إذا (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.خانہ = 0;
				استمر;
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

			إذا (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			إذا (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_رکن;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			إذا (lookfor == LOOKFOR_رکن
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    إذا ((trypos = لبھو_شروع_brace()) == عدم
				       || لبھو_میل(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				اكسر;
			    استمر;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			إذا (لبھو_آخری_paren(l, '{', '}') // XXX
				     && (trypos = لبھو_شروع_brace()) != عدم)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    إذا (*l == '}' || !cin_iselse(l))
				اذهب رکن_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.خانہ = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      إذا (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      إذا (curbuf->b_ind_jump_label > 0 && original_سطر_islabel)
	  amount -= curbuf->b_ind_jump_label;

      اذهب theend;
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

    إذا (theline[0] == '{')
    {
	amount = curbuf->b_ind_پہلا_open;
	اذهب theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    إذا (cur_curpos.lnum < curbuf->b_ml.ml_سطر_گنتی
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == عدم
	    && vim_strchr(theline, '}') == عدم
	    && !cin_سرے_وچ(theline, (char_u *)":")
	    && !cin_سرے_وچ(theline, (char_u *)",")
	    && cin_isfuncdecl(عدم, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	اذهب theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    طالما (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.خانہ = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	إذا ((trypos = ind_لبھو_شروع_CORS(عدم)) != عدم)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.خانہ = 0;
	    استمر;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	إذا (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	إذا (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.خانہ);
	    اكسر;
	}

	// Skip preprocessor directives and blank lines.
	إذا (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    استمر;

	إذا (cin_nocode(l))
	    استمر;

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
	إذا (cin_سرے_وچ(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    إذا (لبھو_آخری_paren(l, '(', ')')
		    && (trypos = لبھو_میل_paren(
				     curbuf->b_ind_maxparen)) != عدم)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    طالما (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		إذا (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    اكسر;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.خانہ = 0;
	    }

	    amount = get_indent();	    // XXX

	    إذا (amount == 0)
		amount = cin_پہلا_id_amount();
	    إذا (amount == 0)
		amount = ind_continuation;
	    اكسر;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	إذا (cin_isfuncdecl(عدم, cur_curpos.lnum, 0))  // XXX
	    اكسر;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	إذا (*skipwhite(l) == '}')
	    اكسر;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	إذا (cin_سرے_وچ(l, (char_u *)"};"))
	    اكسر;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	إذا (cin_سرے_وچ(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    اكسر;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	إذا (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    طالما (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		إذا (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    اكسر;
	    }
	    إذا (curwin->w_cursor.lnum > 0
			    && cin_سرے_وچ(look, (char_u *)"}"))
		اكسر;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	إذا (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    اكسر;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	إذا (cin_سرے_وچ(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    إذا (cin_سرے_وچ(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		اكسر;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	لبھو_آخری_paren(l, '(', ')');

	إذا ((trypos = لبھو_میل_paren(curbuf->b_ind_maxparen)) != عدم)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	اكسر;
    }

    // add extra indent for a comment
    إذا (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    إذا (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	إذا (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    إذا (cur_amount > 0)
		amount = cur_amount;
	    وإلا إذا (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    إذا (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    أرجع amount;
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
    صحيح
وچ_cinkeys(
    صحيح		keytyped,
    صحيح		when,
    صحيح		سطر_is_خالی_تھاواں)
{
    char_u	*look;
    صحيح		try_میل;
    صحيح		try_میل_لفظ;
    char_u	*p;
    char_u	*سطر;
    صحيح		icase;
    صحيح		i;

    إذا (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	أرجع FALSE;

#جے_تعریف_شدہ FEAT_EVAL
    إذا (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    وإلا
#جے_انت
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    طالما (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	بدل (when)
	{
	    حالة '*': try_میل = (*look == '*'); اكسر;
	    حالة '!': try_میل = (*look == '!'); اكسر;
	    افتراضي: try_میل = (*look != '*'); اكسر;
	}
	إذا (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	إذا (*look == '0')
	{
	    try_میل_لفظ = try_میل;
	    إذا (!سطر_is_خالی_تھاواں)
		try_میل = FALSE;
	    ++look;
	}
	وإلا
	    try_میل_لفظ = FALSE;

	// does it look like a control character?
	إذا (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    إذا (try_میل && keytyped == Ctrl_chr(look[1]))
		أرجع TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	وإلا إذا (*look == 'o')
	{
	    إذا (try_میل && keytyped == کلید_OPEN_FORW)
		أرجع TRUE;
	    ++look;
	}
	وإلا إذا (*look == 'O')
	{
	    إذا (try_میل && keytyped == کلید_OPEN_پٹھ)
		أرجع TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	وإلا إذا (*look == 'e')
	{
	    إذا (try_میل && keytyped == 'e' && curwin->w_cursor.خانہ >= 4)
	    {
		p = ml_get_curline();
		إذا (skipwhite(p) == p + curwin->w_cursor.خانہ - 4 &&
			STRNCMP(p + curwin->w_cursor.خانہ - 4, "else", 4) == 0)
		    أرجع TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	وإلا إذا (*look == ':')
	{
	    إذا (try_میل && keytyped == ':')
	    {
		p = ml_get_curline();
		إذا (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    أرجع TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		إذا (curwin->w_cursor.خانہ > 2
			&& p[curwin->w_cursor.خانہ - 1] == ':'
			&& p[curwin->w_cursor.خانہ - 2] == ':')
		{
		    p[curwin->w_cursor.خانہ - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.خانہ - 1] = ':';
		    إذا (i)
			أرجع TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	وإلا إذا (*look == '<')
	{
	    إذا (try_میل)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		إذا (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != عدم
						       && keytyped == look[1])
		    أرجع TRUE;

		إذا (keytyped == get_special_کلید_کوڈ(look + 1))
		    أرجع TRUE;
	    }
	    طالما (*look && *look != '>')
		look++;
	    طالما (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	وإلا إذا (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    إذا (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    وإلا
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    إذا (p == عدم)
		p = look + STRLEN(look);
	    إذا ((try_میل || try_میل_لفظ)
		    && curwin->w_cursor.خانہ >= (colnr_T)(p - look))
	    {
		صحيح		میل = FALSE;

		إذا (keytyped == کلید_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    سطر = ml_get_curline();
		    إذا (has_mbyte)
		    {
			char_u	*n;

			لكل (s = سطر + curwin->w_cursor.خانہ; s > سطر; s = n)
			{
			    n = mb_prevptr(سطر, s);
			    إذا (!vim_iswordp(n))
				اكسر;
			}
		    }
		    وإلا
			لكل (s = سطر + curwin->w_cursor.خانہ; s > سطر; --s)
			    إذا (!vim_iswordc(s[-1]))
				اكسر;
		    إذا (s + (p - look) <= سطر + curwin->w_cursor.خانہ
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			میل = TRUE;
		}
		وإلا
		    // TODO: multi-byte
		    إذا (keytyped == (صحيح)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((صحيح)p[-1])))
		{
		    سطر = ml_get_cursor();
		    إذا ((curwin->w_cursor.خانہ == (colnr_T)(p - look)
				|| !vim_iswordc(سطر[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(سطر - (p - look), look, p - look)
				: STRNCMP(سطر - (p - look), look, p - look))
									 == 0)
			میل = TRUE;
		}
		إذا (میل && try_میل_لفظ && !try_میل)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    إذا (getwhitecols_curline() !=
				     (صحيح)(curwin->w_cursor.خانہ - (p - look)))
			میل = FALSE;
		}
		إذا (میل)
		    أرجع TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	وإلا
	{
	    إذا (try_میل && *look == keytyped)
		أرجع TRUE;
	    إذا (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_منزل_option_part(look);
    }
    أرجع FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    فراغ
do_c_اظہاریہ_indent(فراغ)
{
#جے_تعریف_شدہ FEAT_EVAL
    إذا (*curbuf->b_p_inde != NUL)
	fixthisline(get_اظہاریہ_indent);
    وإلا
#جے_انت
	fixthisline(get_c_indent);
}

#إذا defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    فراغ
f_cindent(typval_T *argvars بن_ورتیا, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    إذا (وچ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    إذا (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_نمبر = get_c_indent();
	curwin->w_cursor = pos;
    }
    وإلا
	rettv->vval.v_نمبر = -1;
}
#جے_انت
