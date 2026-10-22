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
    اگر (cinw_buf == تہی)
	واپس FALSE;

    سطر = skipwhite(سطر);
    برائے (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	اگر (STRNCMP(سطر, cinw_buf, len) == 0
		&& (!vim_iswordc(سطر[len]) || !vim_iswordc(سطر[len - 1])))
	{
	    retval = TRUE;
	    توڑو;
	}
    }
    vim_free(cinw_buf);
    واپس retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    ساکن char_u *
skip_string(char_u *p)
{
    صحیح	    i;

    // We loop, because strings may be concatenated: "date""time".
    برائے ( ; ; ++p)
    {
	اگر (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    اگر (p[1] == NUL)		    // ' at end of line
		توڑو;
	    i = 2;
	    اگر (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		جب (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    اگر (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		جاری;
	    }
	}
	ورنہ اگر (p[0] == '"')		    // start of string
	{
	    برائے (++p; p[0]; ++p)
	    {
		اگر (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ورنہ اگر (p[0] == '"')	    // end of string
		    توڑو;
	    }
	    اگر (p[0] == '"')
		جاری; // continue for another string
	}
	ورنہ اگر (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    اگر (paren != تہی)
	    {
		size_t delim_len = paren - delim;

		برائے (p += 3; *p; ++p)
		    اگر (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			توڑو;
		    }
		اگر (p[0] == '"')
		    جاری; // continue for another string
	    }
	}
	توڑو;				    // no string found
    }
    اگر (!*p)
	--p;				    // backup from NUL
    واپس p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    صحیح
is_pos_اندر_string(char_u *سطر, colnr_T ستون)
{
    char_u *p;

    برائے (p = سطر; *p && (colnr_T)(p - سطر) < ستون; ++p)
	p = skip_string(p);
    واپس !((colnr_T)(p - سطر) <= ستون);
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
	اگر (vim_strchr(p, ';') != تہی) // there may be comments
	{
	    صحیح اندر_str = FALSE;	// inside of string

	    p = سطر;		// scan from start
	    جب ((p = vim_strpbrk(p, (char_u *)"\";")) != تہی)
	    {
		اگر (*p == '"')
		{
		    اگر (اندر_str)
		    {
			اگر (*(p - 1) != '\\') // skip escaped quote
			    اندر_str = FALSE;
		    }
		    ورنہ اگر (p == سطر || ((p - سطر) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			اندر_str = TRUE;
		}
		ورنہ اگر (!اندر_str && ((p - سطر) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_اندر_string(سطر, (colnr_T)(p - سطر)))
		    توڑو;	// found!
		++p;
	    }
	}
	ورنہ
	    p = تہی;
    }
    ورنہ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	برائے ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    اگر (*p == NUL)
		توڑو;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    اگر (p[0] == '/' && p[1] == '/'
				 && (p == سطر || p[-1] != '*' || p[2] != '*'))
		توڑو;
	}
	اگر (*p == NUL)
	    p = تہی;
    }

    اگر (p == تہی)
	واپس MAXCOL;
    واپس (صحیح)(p - سطر);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    ساکن pos_T *
ind_ڈھونڈو_آغاز_comment(خالی)	    // XXX
{
    واپس ڈھونڈو_آغاز_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
ڈھونڈو_آغاز_comment(صحیح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحیح		cur_maxcomment = ind_maxcomment;

    برائے (;;)
    {
	pos = findmatchlimit(تہی, '*', FM_BACKWARD, cur_maxcomment);
	اگر (pos == تہی)
	    توڑو;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	اگر (!is_pos_اندر_string(ml_get(pos->lnum), pos->ستون))
	    توڑو;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	اگر (cur_maxcomment <= 0)
	{
	    pos = تہی;
	    توڑو;
	}
    }
    واپس pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    ساکن pos_T *
ڈھونڈو_آغاز_rawstring(صحیح ind_maxcomment)	// XXX
{
    pos_T	*pos;
    صحیح		cur_maxcomment = ind_maxcomment;

    برائے (;;)
    {
	pos = findmatchlimit(تہی, 'R', FM_BACKWARD, cur_maxcomment);
	اگر (pos == تہی)
	    توڑو;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	اگر (!is_pos_اندر_string(ml_get(pos->lnum), pos->ستون))
	    توڑو;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	اگر (cur_maxcomment <= 0)
	{
	    pos = تہی;
	    توڑو;
	}
    }
    واپس pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    ساکن pos_T *
ind_ڈھونڈو_آغاز_CORS(linenr_T *is_raw)	    // XXX
{
    ساکن pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = ڈھونڈو_آغاز_comment(curbuf->b_ind_maxcomment);
    اگر (comment_pos != تہی)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = ڈھونڈو_آغاز_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    اگر (comment_pos == تہی || (rs_pos != تہی
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	اگر (is_raw != تہی && rs_pos != تہی)
	    *is_raw = rs_pos->lnum;
	واپس rs_pos;
    }
    واپس comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    صحیح
cindent_on(خالی)
{
    واپس (!p_paste && (curbuf->b_p_cin
#اگر_تعریف_شدہ FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#اگر_ختم
		    ));
}

// Find result cache for cpp_baseclass
قسمتعریف ساخت {
    صحیح	    ملا;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    ساکن char_u *
cin_skipcomment(char_u *s)
{
    جب (*s)
    {
	char_u *پچھلا_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	اگر (curbuf->b_ind_hash_comment != 0 && s != پچھلا_s && *s == '#')
	{
	    s += STRLEN(s);
	    توڑو;
	}
	اگر (*s != '/')
	    توڑو;
	++s;
	اگر (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    توڑو;
	}
	اگر (*s != '*')
	    توڑو;
	برائے (++s; *s; ++s)	// skip slash-star comment
	    اگر (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		توڑو;
	    }
    }
    واپس s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    ساکن صحیح
cin_nocode(char_u *s)
{
    واپس *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    ساکن صحیح
cin_iscomment(char_u *p)
{
    واپس (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    ساکن صحیح
cin_islinecomment(char_u *p)
{
    واپس (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    ساکن pos_T *
ڈھونڈو_سطر_comment(خالی) // XXX
{
    ساکن pos_T pos;
    char_u	 *سطر;
    char_u	 *p;

    pos = curwin->w_cursor;
    جب (--pos.lnum > 0)
    {
	سطر = ml_get(pos.lnum);
	p = skipwhite(سطر);
	اگر (cin_islinecomment(p))
	{
	    pos.ستون = (صحیح)(p - سطر);
	    واپس &pos;
	}
	اگر (*p != NUL)
	    توڑو;
    }
    واپس تہی;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    ساکن صحیح
cin_has_js_کلید(char_u *متن)
{
    char_u *s = skipwhite(متن);
    صحیح	    quote = -1;

    اگر (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    اگر (!vim_isIDc(*s))	    // need at least one ID character
	واپس FALSE;

    جب (vim_isIDc(*s))
	++s;
    اگر (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    واپس (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    ساکن صحیح
cin_islabel_skip(char_u **s)
{
    اگر (!vim_isIDc(**s))	    // need at least one ID character
	واپس FALSE;

    جب (vim_isIDc(**s))
    {
	اگر (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ورنہ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    واپس (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    ساکن صحیح
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    صحیح	    ملا = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    اگر (cinsd_buf == تہی)
	واپس FALSE;

    برائے (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (صحیح)cinsd_len, ",");
	اگر (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    اگر (*skip == ':' && skip[1] != ':')
	    {
		ملا = TRUE;
		توڑو;
	    }
	}
    }

    vim_free(cinsd_buf);
    واپس ملا;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    ساکن صحیح
cin_ispreproc(char_u *s)
{
    اگر (*skipwhite(s) == '#')
	واپس TRUE;
    واپس FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    ساکن صحیح
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, صحیح *amount)
{
    char_u	*سطر = *pp;
    linenr_T	lnum = *lnump;
    صحیح		retval = FALSE;
    صحیح		candidate_amount = *amount;

    اگر (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    برائے (;;)
    {
	اگر (cin_ispreproc(سطر))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    توڑو;
	}
	اگر (lnum == 1)
	    توڑو;
	سطر = ml_get(--lnum);
	اگر (*سطر == NUL || سطر[STRLEN(سطر) - 1] != '\\')
	    توڑو;
    }

    اگر (lnum != *lnump)
	*pp = ml_get(*lnump);
    اگر (retval)
	*amount = candidate_amount;
    واپس retval;
}

    ساکن صحیح
cin_iselse(
    char_u  *p)
{
    اگر (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    واپس (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    ساکن صحیح
cin_isterminated(
    char_u	*s,
    صحیح		incl_open,	// include '{' at the end as terminator
    صحیح		incl_comma)	// recognize a trailing comma
{
    char_u	ملا_آغاز = 0;
    غیرعلامتی	n_open = 0;
    صحیح		is_else = FALSE;

    s = cin_skipcomment(s);

    اگر (*s == '{' || (*s == '}' && !cin_iselse(s)))
	ملا_آغاز = *s;

    اگر (!ملا_آغاز)
	is_else = cin_iselse(s);

    جب (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	اگر (*s == '}' && n_open > 0)
	    --n_open;
	اگر ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    واپس *s;
	ورنہ اگر (*s == '{')
	{
	    اگر (incl_open && cin_nocode(s + 1))
		واپس *s;
	    ورنہ
		++n_open;
	}

	اگر (*s)
	    s++;
    }
    واپس ملا_آغاز;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    ساکن صحیح
cin_starts_with(char_u *s, حرف *لفظ)
{
    صحیح l = (صحیح)STRLEN(لفظ);

    واپس (STRNCMP(s, لفظ, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    ساکن صحیح
cin_isdefault(char_u *s)
{
    واپس (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    ساکن صحیح
cin_iscase(
    char_u *s,
    صحیح strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    اگر (cin_starts_with(s, "case"))
    {
	برائے (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    اگر (*s == NUL)
		توڑو;
	    اگر (*s == ':')
	    {
		اگر (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ورنہ
		    واپس TRUE;
	    }
	    اگر (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ورنہ اگر (*s == '/' && (s[1] == '*' || s[1] == '/'))
		واپس FALSE;		// stop at comment
	    ورنہ اگر (*s == '"')
	    {
		// JS etc.
		اگر (strict)
		    واپس FALSE;		// stop at string
		ورنہ
		    واپس TRUE;
	    }
	}
	واپس FALSE;
    }

    اگر (cin_isdefault(s))
	واپس TRUE;
    واپس FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    ساکن صحیح
cin_islabel(خالی)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    اگر (cin_isdefault(s))
	واپس FALSE;
    اگر (cin_isscopedecl(s))
	واپس FALSE;

    اگر (!cin_islabel_skip(&s))
	واپس FALSE;

    اگر (ind_ڈھونڈو_آغاز_CORS(تہی))
	واپس FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*سطر;

    cursor_save = curwin->w_cursor;
    جب (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.ستون = 0;
	اگر ((trypos = ind_ڈھونڈو_آغاز_CORS(تہی)) != تہی) // XXX
	    curwin->w_cursor = *trypos;

	سطر = ml_get_curline();
	اگر (cin_ispreproc(سطر))	// ignore #defines, #if, etc.
	    جاری;
	اگر (*(سطر = cin_skipcomment(سطر)) == NUL)
	    جاری;

	curwin->w_cursor = cursor_save;
	اگر (cin_isterminated(سطر, TRUE, FALSE)
		|| cin_isscopedecl(سطر)
		|| cin_iscase(سطر, TRUE)
		|| (cin_islabel_skip(&سطر) && cin_nocode(سطر)))
	    واپس TRUE;
	واپس FALSE;
    }
    curwin->w_cursor = cursor_save;
    واپس TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    ساکن صحیح
cin_سرے_اندر(char_u *s, char_u *ڈھونڈو)
{
    char_u	*p = s;
    char_u	*r;
    صحیح		len = (صحیح)STRLEN(ڈھونڈو);

    جب (*p != NUL)
    {
	p = cin_skipcomment(p);
	اگر (STRNCMP(p, ڈھونڈو, len) == 0)
	{
	    r = skipwhite(p + len);
	    اگر (cin_nocode(r))
		واپس TRUE;
	}
	اگر (*p != NUL)
	    ++p;
    }
    واپس FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    ساکن char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = تہی, *p = s;
    کرو
    {
	r = p;
	p = cin_skipcomment(p);
	اگر (*p)
	    p = skip_string(p);
    } جب (p != r);
    واپس p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    ساکن صحیح
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = تہی;

    جب (*p)
    {
	اگر (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ورنہ اگر (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ورنہ
	    p = cin_skip_comment_and_string(p + 1);
    }
    اگر (!r)
	واپس FALSE;
    p = r; // p points now after '=' or "return"

    اگر (cin_nocode(p))
	واپس TRUE;

    اگر (*p == '&')
	p = cin_skipcomment(p + 1);

    اگر (*p == '(') // skip a typecast
    {
	صحیح open_گنتی = 1;
	کرو
	{
	    p = cin_skip_comment_and_string(p + 1);
	    اگر (cin_nocode(p))
		واپس TRUE;
	    open_گنتی += (*p == '(') - (*p == ')');
	} جب (open_گنتی);
	p = cin_skipcomment(p + 1);
	اگر (cin_nocode(p))
	    واپس TRUE;
    }

    جب (*p == '{')
	p = cin_skipcomment(p + 1);
    واپس cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    ساکن صحیح
cin_isinit(خالی)
{
    char_u	*s;
    ساکن حرف *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    اگر (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    برائے (;;)
    {
	صحیح i, l;

	برائے (i = 0; i < (صحیح)ARRAY_لمبائی(skip); ++i)
	{
	    l = (صحیح)لڑی_لمبائی(skip[i]);
	    اگر (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		توڑو;
	    }
	}
	اگر (l != 0)
	    توڑو;
    }

    اگر (cin_starts_with(s, "enum"))
	واپس TRUE;

    واپس cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#تعریف_کرو ڈھونڈو_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    ساکن صحیح
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    صحیح		has_name = FALSE;
    صحیح		has_name_آغاز = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    جب ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    اگر (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	جب (*p != NUL)
	{
	    اگر (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ورنہ اگر (*p == '{')
	    {
		توڑو;
	    }
	    ورنہ اگر (vim_iswordc(*p))
	    {
		has_name_آغاز = TRUE;
		اگر (has_name)
		    واپس FALSE; // word character after skipping past name
		++p;
	    }
	    ورنہ اگر (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		اگر (!has_name_آغاز || has_name)
		    واپس FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ورنہ
	    {
		واپس FALSE;
	    }
	}
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    ساکن صحیح
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    صحیح		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    اگر (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	جب (*p != NUL)
	{
	    اگر (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ورنہ اگر (*p == '{')
	    {
		توڑو;
	    }
	    ورنہ اگر (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		اگر (has_string_literal)
		    واپس FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ورنہ اگر (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		اگر (has_string_literal)
		    واپس FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ورنہ
	    {
		واپس FALSE;
	    }
	}
	واپس has_string_literal ? TRUE : FALSE;
    }
    واپس FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    ساکن char_u *
after_label(char_u *l)
{
    برائے ( ; *l; ++l)
    {
	اگر (*l == ':')
	{
	    اگر (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ورنہ اگر (!cin_iscase(l + 1, FALSE))
		توڑو;
	}
	ورنہ اگر (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    اگر (*l == NUL)
	واپس تہی;
    l = cin_skipcomment(l + 1);
    اگر (*l == NUL)
	واپس تہی;
    واپس l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    ساکن صحیح
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	ستون;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    اگر (p == تہی)
	واپس 0;

    fp.ستون = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &ستون, تہی, تہی, 0);
    واپس (صحیح)ستون;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    ساکن صحیح
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
	اگر (l == تہی)		// just in case
	    l = ml_get_curline();
    }
    ورنہ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    واپس amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    ساکن صحیح
cin_پہلا_id_amount(خالی)
{
    char_u	*سطر, *p, *s;
    صحیح		len;
    pos_T	fp;
    colnr_T	ستون;

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
    ورنہ اگر (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ورنہ اگر ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	اگر ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    برائے (len = 0; vim_isIDc(p[len]); ++len)
	;
    اگر (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	واپس 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.ستون = (colnr_T)(p - سطر);
    getvcol(curwin, &fp, &ستون, تہی, تہی, 0);
    واپس (صحیح)ستون;
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
    ساکن صحیح
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*سطر;
    char_u	*s;
    colnr_T	ستون;
    pos_T	fp;

    اگر (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	اگر (*سطر != NUL && سطر[STRLEN(سطر) - 1] == '\\')
	    واپس -1;
    }

    سطر = s = ml_get(lnum);
    جب (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == تہی)
    {
	اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ورنہ
	    ++s;
    }
    اگر (*s != '=')
	واپس 0;

    s = skipwhite(s + 1);
    اگر (cin_nocode(s))
	واپس 0;

    اگر (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.ستون = (colnr_T)(s - سطر);
    getvcol(curwin, &fp, &ستون, تہی, تہی, 0);
    واپس (صحیح)ستون;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    ساکن صحیح
cin_skip2pos(pos_T *trypos)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*new_p;

    p = سطر = ml_get(trypos->lnum);
    جب (*p && (colnr_T)(p - سطر) < trypos->ستون)
    {
	اگر (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ورنہ
	{
	    new_p = skip_string(p);
	    اگر (new_p == p)
		++p;
	    ورنہ
		p = new_p;
	}
    }
    واپس (صحیح)(p - سطر);
}

    ساکن pos_T *
ڈھونڈو_مطابقت_char(صحیح c, صحیح ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    ساکن pos_T pos_copy;
    صحیح		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    اگر ((trypos = findmatchlimit(تہی, c, FM_SKIPCOMM, ind_maxp_wk)) != تہی)
    {
	// check if the ( is in a // comment
	اگر ((colnr_T)cin_skip2pos(trypos) > trypos->ستون)
	{
	    ind_maxp_wk = ind_maxparen - (صحیح)(cursor_save.lnum - trypos->lnum);
	    اگر (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.ستون = 0;	// XXX
		جاؤ retry;
	    }
	    trypos = تہی;
	}
	ورنہ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    اگر ((trypos_wk = ind_ڈھونڈو_آغاز_CORS(تہی)) != تہی) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (صحیح)(cursor_save.lnum
			- trypos_wk->lnum);
		اگر (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    جاؤ retry;
		}
		trypos = تہی;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    واپس trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    ساکن pos_T *
ڈھونڈو_مطابقت_paren(صحیح ind_maxparen)	// XXX
{
    واپس ڈھونڈو_مطابقت_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    ساکن صحیح
ڈھونڈو_آخری_paren(char_u *l, صحیح آغاز, صحیح انتہا)
{
    صحیح		i;
    صحیح		retval = FALSE;
    صحیح		open_گنتی = 0;

    curwin->w_cursor.ستون = 0;		    // default is start of line

    برائے (i = 0; l[i] != NUL; i++)
    {
	i = (صحیح)(cin_skipcomment(l + i) - l); // ignore parens in comments
	اگر (l[i] == NUL)
	    توڑو;
	i = (صحیح)(skip_string(l + i) - l);    // ignore parens in quotes
	اگر (l[i] == آغاز)
	    ++open_گنتی;
	ورنہ اگر (l[i] == انتہا)
	{
	    اگر (open_گنتی > 0)
		--open_گنتی;
	    ورنہ
	    {
		curwin->w_cursor.ستون = i;
		retval = TRUE;
	    }
	}
    }
    واپس retval;
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
    ساکن صحیح
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	پہلا_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = پہلا_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    صحیح		retval = FALSE;
    pos_T	*trypos;
    صحیح		just_started = TRUE;

    اگر (sp == تہی)
	s = ml_get(lnum);
    ورنہ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    اگر (ڈھونڈو_آخری_paren(s, '(', ')')
	&& (trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen)) != تہی)
    {
	lnum = trypos->lnum;
	اگر (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    واپس FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    اگر (cin_ispreproc(s))
	واپس FALSE;

    جب (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ورنہ اگر (*s == ':')
	{
	    اگر (*(s + 1) == ':')
		s += 2;
	    ورنہ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		واپس FALSE;
	}
	ورنہ
	    ++s;
    }
    اگر (*s != '(')
	واپس FALSE;		// ';', ' or "  before any () or no '('

    جب (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	اگر (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = پہلا_lnum - 1;
	    s = ml_get(lnum);
	    اگر (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    جاؤ مکمل;
	}
	اگر ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    صحیح comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    برائے (;;)
	    {
		اگر (lnum >= curbuf->b_ml.ml_سطر_گنتی)
		    توڑو;
		s = ml_get(++lnum);
		اگر (!cin_ispreproc(s))
		    توڑو;
	    }
	    اگر (lnum >= curbuf->b_ml.ml_سطر_گنتی)
		توڑو;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    اگر (!just_started && (!comma && *s != ',' && *s != ')'))
		توڑو;
	    just_started = FALSE;
	}
	ورنہ اگر (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ورنہ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

مکمل:
    اگر (lnum != پہلا_lnum && sp != تہی)
	*sp = ml_get(پہلا_lnum);

    واپس retval;
}

    ساکن صحیح
cin_isif(char_u *p)
{
    واپس (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    ساکن صحیح
cin_isdo(char_u *p)
{
    واپس (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    ساکن صحیح
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
	curwin->w_cursor.ستون = 0;
	p = ml_get_curline();
	جب (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.ستون;
	}
	اگر ((trypos = findmatchlimit(تہی, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != تہی
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    واپس retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    ساکن صحیح
cin_is_if_for_while_before_انحراف(char_u *سطر, صحیح *poffset)
{
    صحیح انحراف = *poffset;

    اگر (انحراف-- < 2)
	واپس 0;
    جب (انحراف > 2 && VIM_ISWHITE(سطر[انحراف]))
	--انحراف;

    انحراف -= 1;
    اگر (STRNCMP(سطر + انحراف, "if", 2) == 0)
	جاؤ probably_ملا;

    اگر (انحراف >= 1)
    {
	انحراف -= 1;
	اگر (STRNCMP(سطر + انحراف, "for", 3) == 0)
	    جاؤ probably_ملا;

	اگر (انحراف >= 2)
	{
	    انحراف -= 2;
	    اگر (STRNCMP(سطر + انحراف, "while", 5) == 0)
		جاؤ probably_ملا;
	}
    }
    واپس 0;

probably_ملا:
    اگر (!انحراف || !vim_isIDc(سطر[انحراف - 1]))
    {
	*poffset = انحراف;
	واپس 1;
    }
    واپس 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    ساکن صحیح
cin_iswhileofdo_انتہا(صحیح terminated)
{
    char_u	*سطر;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    صحیح		i;

    اگر (terminated != ';')	// there must be a ';' at the end
	واپس FALSE;

    p = سطر = ml_get_curline();
    جب (*p != NUL)
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
		curwin->w_cursor.ستون = i;
		trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen);
		اگر (trypos != تہی)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    اگر (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    اگر (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			واپس TRUE;
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
    واپس FALSE;
}

    ساکن صحیح
cin_isbreak(char_u *p)
{
    واپس (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    ساکن صحیح
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    صحیح		class_or_struct, lookfor_ctor_init, cpp_بنیاد_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*سطر = ml_get_curline();

    اگر (pos->lnum <= lnum)
	واپس cached->ملا;	// Use the cached result

    pos->ستون = 0;

    s = skipwhite(سطر);
    اگر (*s == '#')		// skip #define FOO x ? (x) : x
	واپس FALSE;
    s = cin_skipcomment(s);
    اگر (*s == NUL)
	واپس FALSE;

    cpp_بنیاد_class = lookfor_ctor_init = class_or_struct = FALSE;

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
    جب (lnum > 1)
    {
	سطر = ml_get(lnum - 1);
	s = skipwhite(سطر);
	اگر (*s == '#' || *s == NUL)
	    توڑو;
	جب (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    اگر (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		توڑو;
	    اگر (*s != NUL)
		++s;
	}
	اگر (*s != NUL)
	    توڑو;
	--lnum;
    }

    pos->lnum = lnum;
    سطر = ml_get(lnum);
    s = سطر;
    برائے (;;)
    {
	اگر (*s == NUL)
	{
	    اگر (lnum == curwin->w_cursor.lnum)
		توڑو;
	    // Continue in the cursor line.
	    سطر = ml_get(++lnum);
	    s = سطر;
	}
	اگر (s == سطر)
	{
	    // don't recognize "case (foo):" as a baseclass
	    اگر (cin_iscase(s, FALSE))
		توڑو;
	    s = cin_skipcomment(سطر);
	    اگر (*s == NUL)
		جاری;
	}

	اگر (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ورنہ اگر (s[0] == ':')
	{
	    اگر (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ورنہ اگر (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_بنیاد_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->ستون = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ورنہ
		s = cin_skipcomment(s + 1);
	}
	ورنہ اگر ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    اگر (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ورنہ
		s = cin_skipcomment(s + 6);
	}
	ورنہ
	{
	    اگر (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_بنیاد_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ورنہ اگر (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ورنہ اگر (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		واپس FALSE;
	    }
	    ورنہ اگر (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ورنہ اگر (pos->ستون == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		اگر (cpp_بنیاد_class)
		    pos->ستون = (colnr_T)(s - سطر);
	    }

	    // When the line ends in a comma don't align with it.
	    اگر (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->ستون = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->ملا = cpp_بنیاد_class;
    اگر (cpp_بنیاد_class)
	pos->lnum = lnum;
    واپس cpp_بنیاد_class;
}

    ساکن صحیح
get_baseclass_amount(صحیح ستون)
{
    صحیح		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    اگر (ستون == 0)
    {
	amount = get_indent();
	اگر (ڈھونڈو_آخری_paren(ml_get_curline(), '(', ')')
		&& (trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen)) != تہی)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	اگر (!cin_سرے_اندر(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ورنہ
    {
	curwin->w_cursor.ستون = ستون;
	getvcol(curwin, &curwin->w_cursor, &vcol, تہی, تہی, 0);
	amount = (صحیح)vcol;
    }
    اگر (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    واپس amount;
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

    ساکن pos_T *
ڈھونڈو_آغاز_brace(خالی)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    ساکن pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    جب ((trypos = findmatchlimit(تہی, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != تہی)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = تہی;
	// ignore the { if it's in a // or / *  * / comment
	اگر ((colnr_T)cin_skip2pos(trypos) == trypos->ستون
		       && (pos = ind_ڈھونڈو_آغاز_CORS(تہی)) == تہی) // XXX
	    توڑو;
	اگر (pos != تہی)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    واپس trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    ساکن pos_T *
ڈھونڈو_مطابقت_paren_after_brace (صحیح ind_maxparen)	    // XXX
{
    pos_T	*trypos = ڈھونڈو_مطابقت_paren(ind_maxparen);

    اگر (trypos == تہی)
	واپس تہی;

    pos_T	*tryposBrace = ڈھونڈو_آغاز_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    اگر (tryposBrace != تہی
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->ستون < tryposBrace->ستون))
	trypos = تہی;
    واپس trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    ساکن صحیح
corr_ind_maxparen(pos_T *startpos)
{
    طویل	n = (طویل)startpos->lnum - (طویل)curwin->w_cursor.lnum;

    اگر (n > 0 && n < curbuf->b_ind_maxparen / 2)
	واپس curbuf->b_ind_maxparen - (صحیح)n;
    واپس curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    خالی
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    صحیح		divider;
    صحیح		fraction = 0;
    صحیح		sw;
    طویل	t = get_sw_قدر(buf);

    // needed for cino-(, it will be multiplied by 2 again
    اگر (t > زیادہ_سے_زیادہ_عدد / 2)
	sw = زیادہ_سے_زیادہ_عدد / 2;
    ورنہ
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
    buf->b_ind_case_رمز = sw;

    // Lineup break at end of case in switch() with case label.
    buf->b_ind_case_break = 0;

    // Spaces from the class declaration indent a scope declaration label
    // should be located.
    buf->b_ind_scopedecl = sw;

    // Spaces from the scope declaration label code should be located.
    buf->b_ind_scopedecl_رمز = sw;

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
    buf->b_ind_paren_پچھلا = 0;

    // Extra indent for comments.
    buf->b_ind_comment = 0;

    // Spaces from the comment opener when there is nothing after it.
    buf->b_ind_اندر_comment = 3;

    // Boolean: if non-zero, use b_ind_in_comment even if there is something
    // after the comment opener.
    buf->b_ind_اندر_comment2 = 0;

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

    برائے (p = buf->b_p_cino; *p; )
    {
	l = p++;
	اگر (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	اگر (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((حرف *)++p);
	    جب (VIM_ISDIGIT(*p))
	    {
		++p;
		اگر (divider)
		    divider *= 10;
		ورنہ
		    divider = 10;
	    }
	}
	اگر (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    اگر (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ورنہ
	    {
		n *= sw;
		اگر (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	اگر (l[1] == '-')
	    n = -n;

	n = trim_مقصد_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	انتخاب (*l)
	{
	    حالت '>': buf->b_ind_level = (صحیح)n; توڑو;
	    حالت 'e': buf->b_ind_open_imag = (صحیح)n; توڑو;
	    حالت 'n': buf->b_ind_no_brace = (صحیح)n; توڑو;
	    حالت 'f': buf->b_ind_پہلا_open = (صحیح)n; توڑو;
	    حالت '{': buf->b_ind_open_extra = (صحیح)n; توڑو;
	    حالت '}': buf->b_ind_close_extra = (صحیح)n; توڑو;
	    حالت '^': buf->b_ind_open_بایاں_imag = (صحیح)n; توڑو;
	    حالت 'L': buf->b_ind_jump_label = (صحیح)n; توڑو;
	    حالت ':': buf->b_ind_case = (صحیح)n; توڑو;
	    حالت '=': buf->b_ind_case_رمز = (صحیح)n; توڑو;
	    حالت 'b': buf->b_ind_case_break = (صحیح)n; توڑو;
	    حالت 'p': buf->b_ind_param = (صحیح)n; توڑو;
	    حالت 't': buf->b_ind_func_type = (صحیح)n; توڑو;
	    حالت '/': buf->b_ind_comment = (صحیح)n; توڑو;
	    حالت 'c': buf->b_ind_اندر_comment = (صحیح)n; توڑو;
	    حالت 'C': buf->b_ind_اندر_comment2 = (صحیح)n; توڑو;
	    حالت 'i': buf->b_ind_cpp_baseclass = (صحیح)n; توڑو;
	    حالت '+': buf->b_ind_continuation = (صحیح)n; توڑو;
	    حالت '(': buf->b_ind_unclosed = (صحیح)n; توڑو;
	    حالت 'u': buf->b_ind_unclosed2 = (صحیح)n; توڑو;
	    حالت 'U': buf->b_ind_unclosed_noignore = (صحیح)n; توڑو;
	    حالت 'W': buf->b_ind_unclosed_wrapped = (صحیح)n; توڑو;
	    حالت 'w': buf->b_ind_unclosed_whiteok = (صحیح)n; توڑو;
	    حالت 'm': buf->b_ind_matching_paren = (صحیح)n; توڑو;
	    حالت 'M': buf->b_ind_paren_پچھلا = (صحیح)n; توڑو;
	    حالت ')': buf->b_ind_maxparen = (صحیح)n; توڑو;
	    حالت '*': buf->b_ind_maxcomment = (صحیح)n; توڑو;
	    حالت 'g': buf->b_ind_scopedecl = (صحیح)n; توڑو;
	    حالت 'h': buf->b_ind_scopedecl_رمز = (صحیح)n; توڑو;
	    حالت 'j': buf->b_ind_java = (صحیح)n; توڑو;
	    حالت 'J': buf->b_ind_js = (صحیح)n; توڑو;
	    حالت 'l': buf->b_ind_keep_case_label = (صحیح)n; توڑو;
	    حالت '#': buf->b_ind_hash_comment = (صحیح)n; توڑو;
	    حالت 'N': buf->b_ind_cpp_namespace = (صحیح)n; توڑو;
	    حالت 'k': buf->b_ind_if_for_while = (صحیح)n; توڑو;
	    حالت 'E': buf->b_ind_cpp_extern_c = (صحیح)n; توڑو;
	    حالت 'P': buf->b_ind_pragma = (صحیح)n; توڑو;
	}
	اگر (*p == ',')
	    ++p;
    }
}

    ساکن صحیح
ڈھونڈو_مطابقت(صحیح lookfor, linenr_T ourscope)
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
    ورنہ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.ستون = 0;

    جب (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ستون = 0;

	look = cin_skipcomment(ml_get_curline());
	اگر (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = ڈھونڈو_آغاز_brace();  // XXX
	    اگر (theirscope == تہی)
		توڑو;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    اگر (theirscope->lnum < ourscope)
		توڑو;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    اگر (theirscope->lnum > ourscope)
		جاری;

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
		    جاری;
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
		جاری;
	    }

	    // If it's a "do" decrement whilelevel
	    اگر (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    اگر (elselevel <= 0 && whilelevel <= 0)
		واپس OK;
	}
    }
    واپس FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    صحیح
get_c_indent(خالی)
{
    pos_T	cur_curpos;
    صحیح		amount;
    صحیح		scope_amount;
    صحیح		cur_amount = MAXCOL;
    colnr_T	ستون;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = تہی;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*آغاز;
    صحیح		آغاز_brace;
#تعریف_کرو BRACE_اندر_COL0		1	    // '{' is in column 0
#تعریف_کرو BRACE_AT_آغاز		2	    // '{' is at start of line
#تعریف_کرو BRACE_AT_انتہا		3	    // '{' is at end of line
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
    صحیح		added_مقصد_amount = 0;
    صحیح		js_cur_has_کلید = 0;
    linenr_T	raw_string_آغاز = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    صحیح		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    اگر (cur_curpos.lnum == 1)
	واپس 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    اگر (linecopy == تہی)
	واپس 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    اگر ((State & منوال_داخل_کرو)
	    && curwin->w_cursor.ستون < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.ستون] == ')')
	linecopy[curwin->w_cursor.ستون] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.ستون = 0;

    original_سطر_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_ڈھونڈو_آغاز_comment();
    اگر (comment_pos != تہی)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = ڈھونڈو_آغاز_rawstring(curbuf->b_ind_maxcomment);
    اگر (trypos != تہی && (comment_pos == تہی
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	جاؤ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    اگر (*theline == '#' && (*linecopy == '#' || اندر_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	اگر (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    جاؤ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    اگر (original_سطر_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	جاؤ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    اگر (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = ڈھونڈو_سطر_comment(); // XXX
	اگر (trypos == تہی && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.ستون =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    اگر (linecomment_pos.ستون != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	اگر (trypos  != تہی)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &ستون, تہی, تہی, 0);
	    amount = ستون;
	    جاؤ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    اگر (!cin_iscomment(theline) && comment_pos != تہی) // XXX
    {
	صحیح	lead_آغاز_len = 2;
	صحیح	lead_درمیان_len = 1;
	char_u	lead_آغاز[COM_MAX_LEN];	// start-comment string
	char_u	lead_درمیان[COM_MAX_LEN];	// middle-comment string
	char_u	lead_انتہا[COM_MAX_LEN];		// end-comment string
	صحیح	lead_انتہا_len;
	char_u	*p;
	صحیح	آغاز_align = 0;
	صحیح	آغاز_off = 0;
	صحیح	مکمل = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &ستون, تہی, تہی, 0);
	amount = ستون;
	*lead_آغاز = NUL;
	*lead_درمیان = NUL;

	p = curbuf->b_p_com;
	جب (*p != NUL)
	{
	    صحیح	align = 0;
	    صحیح	off = 0;
	    صحیح what = 0;

	    جب (*p != NUL && *p != ':')
	    {
		اگر (*p == COM_آغاز || *p == COM_انتہا || *p == COM_درمیان)
		    what = *p++;
		ورنہ اگر (*p == COM_بایاں || *p == COM_دایاں)
		    align = *p++;
		ورنہ اگر (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ورنہ
		    ++p;
	    }

	    اگر (*p == ':')
		++p;
	    lead_انتہا_len = copy_option_part(&p, lead_انتہا, COM_MAX_LEN, ",");
	    اگر (what == COM_آغاز)
	    {
		STRCPY(lead_آغاز, lead_انتہا);
		lead_آغاز_len = lead_انتہا_len;
		آغاز_off = off;
		آغاز_align = align;
	    }
	    ورنہ اگر (what == COM_درمیان)
	    {
		STRCPY(lead_درمیان, lead_انتہا);
		lead_درمیان_len = lead_انتہا_len;
	    }
	    ورنہ اگر (what == COM_انتہا)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		اگر (STRNCMP(theline, lead_درمیان, lead_درمیان_len) == 0
			&& STRNCMP(theline, lead_انتہا, lead_انتہا_len) != 0)
		{
		    مکمل = TRUE;
		    اگر (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			اگر (STRNCMP(look, lead_آغاز, lead_آغاز_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ورنہ اگر (STRNCMP(look, lead_درمیان,
							lead_درمیان_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    توڑو;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ورنہ اگر (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->ستون,
					     lead_آغاز, lead_آغاز_len) != 0)
			    جاری;
		    }
		    اگر (آغاز_off != 0)
			amount += آغاز_off;
		    ورنہ اگر (آغاز_align == COM_دایاں)
			amount += vim_strsize(lead_آغاز)
						   - vim_strsize(lead_درمیان);
		    توڑو;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		اگر (STRNCMP(theline, lead_درمیان, lead_درمیان_len) != 0
			&& STRNCMP(theline, lead_انتہا, lead_انتہا_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    اگر (off != 0)
			amount += off;
		    ورنہ اگر (align == COM_دایاں)
			amount += vim_strsize(lead_آغاز)
						   - vim_strsize(lead_درمیان);
		    مکمل = TRUE;
		    توڑو;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	اگر (مکمل)
	    ;
	ورنہ اگر (theline[0] == '*')
	    amount += 1;
	ورنہ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    برائے (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		اگر (linewhite(lnum))		    // skip blank lines
		    جاری;
		amount = get_indent_lnum(lnum);	    // XXX
		توڑو;
	    }
	    اگر (amount == -1)			    // use the comment opener
	    {
		اگر (!curbuf->b_ind_اندر_comment2)
		{
		    آغاز = ml_get(comment_pos->lnum);
		    look = آغاز + comment_pos->ستون + 2; // skip / and *
		    اگر (*look != NUL)		    // if something after it
			comment_pos->ستون = (colnr_T)(skipwhite(look) - آغاز);
		}
		getvcol(curwin, comment_pos, &ستون, تہی, تہی, 0);
		amount = ستون;
		اگر (curbuf->b_ind_اندر_comment2 || *look == NUL)
		    amount += curbuf->b_ind_اندر_comment;
	    }
	}
	جاؤ theend;
    }

    // Are we looking at a ']' that has a match?
    اگر (*skipwhite(theline) == ']'
	    && (trypos = ڈھونڈو_مطابقت_char('[', curbuf->b_ind_maxparen)) != تہی)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	جاؤ theend;
    }

    // Are we inside parentheses or braces?  XXX
    اگر (((trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen)) != تہی
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = ڈھونڈو_آغاز_brace()) != تہی
	    || trypos != تہی)
    {
      اگر (trypos != تہی && tryposBrace != تہی)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  اگر (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->ستون < tryposBrace->ستون)
	      trypos = تہی;
	  ورنہ
	      tryposBrace = تہی;
      }

      اگر (trypos != تہی)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	اگر (theline[0] == ')' && curbuf->b_ind_paren_پچھلا)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ورنہ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    برائے (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		اگر (cin_nocode(l))		// skip comment lines
		    جاری;
		اگر (cin_ispreproc_cont(&l, &lnum, &amount))
		    جاری;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		اگر ((trypos = ind_ڈھونڈو_آغاز_CORS(تہی)) != تہی)
		{
		    lnum = trypos->lnum + 1;
		    جاری;
		}

		// XXX
		اگر ((trypos = ڈھونڈو_مطابقت_paren(
			corr_ind_maxparen(&cur_curpos))) != تہی
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->ستون == our_paren_pos.ستون)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    اگر (theline[0] == ')')
		    {
			اگر (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    توڑو;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	اگر (amount == -1)
	{
	    صحیح	    ignore_paren_ستون = 0;
	    صحیح	    is_if_for_while = 0;

	    اگر (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *سطر;

		trypos = &our_paren_pos;
		کرو
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.ستون = outermost.ستون;

		    trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen);
		} جب (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		سطر = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_انحراف(سطر, &outermost.ستون);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    اگر (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *سطر;
		صحیح	    look_ستون;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		سطر = ml_get_curline();
		look_ستون = (صحیح)(look - سطر);
		curwin->w_cursor.ستون = look_ستون + 1;
		اگر ((trypos = findmatchlimit(تہی, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != تہی
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->ستون < our_paren_pos.ستون)
		    ignore_paren_ستون = trypos->ستون + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_ستون;
	    }
	    اگر (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
						      && is_if_for_while == 0)
		    || (!curbuf->b_ind_unclosed_noignore && *look == '('
						    && ignore_paren_ستون == 0))
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
				       && cin_سرے_اندر(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			برائے (ستون = 0; ستون < our_paren_pos.ستون; ++ستون)
			{
			    انتخاب (l[ستون])
			    {
				حالت '(':
				حالت '{': ++n;
					  توڑو;

				حالت ')':
				حالت '}': اگر (n > 1)
					      --n;
					  توڑو;
			    }
			}

			our_paren_pos.ستون = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ورنہ اگر (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.ستون++;
		    ورنہ
		    {
			ستون = our_paren_pos.ستون + 1;
			جب (VIM_ISWHITE(l[ستون]))
			    ستون++;
			اگر (l[ستون] != NUL)	// In case of trailing space
			    our_paren_pos.ستون = ستون;
			ورنہ
			    our_paren_pos.ستون++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		اگر (our_paren_pos.ستون > 0)
		{
		    getvcol(curwin, &our_paren_pos, &ستون, تہی, تہی, 0);
		    اگر (cur_amount > (صحیح)ستون)
			cur_amount = ستون;
		}
	    }

	    اگر (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ورنہ اگر ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_ستون == 0))
	    {
		اگر (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ورنہ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		ستون = our_paren_pos.ستون;
		جب ((صحیح)our_paren_pos.ستون > ignore_paren_ستون)
		{
		    --our_paren_pos.ستون;
		    انتخاب (*ml_get_pos(&our_paren_pos))
		    {
			حالت '(': amount += curbuf->b_ind_unclosed2;
				  ستون = our_paren_pos.ستون;
				  توڑو;
			حالت ')': amount -= curbuf->b_ind_unclosed2;
				  ستون = MAXCOL;
				  توڑو;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		اگر (ستون == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ورنہ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.ستون = ستون;
		    اگر (ڈھونڈو_مطابقت_paren_after_brace(curbuf->b_ind_maxparen)
								      != تہی)
			amount += curbuf->b_ind_unclosed2;
		    ورنہ
		    {
			اگر (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ورنہ
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
      ورنہ
      {
	// We are inside braces, there is a { before this line at the position
	// stored in tryposBrace.
	// Make a copy of tryposBrace, it may point to pos_copy inside
	// find_start_brace(), which may be changed somewhere.
	tryposCopy = *tryposBrace;
	tryposBrace = &tryposCopy;
	trypos = tryposBrace;
	ourscope = trypos->lnum;
	آغاز = ml_get(ourscope);

	// Now figure out how indented the line is in general.
	// If the brace was at the start of the line, we use that;
	// otherwise, check out the indentation of the line as
	// a whole and then add the "imaginary indent" to that.
	look = skipwhite(آغاز);
	اگر (*look == '{')
	{
	    getvcol(curwin, trypos, &ستون, تہی, تہی, 0);
	    amount = ستون;
	    اگر (*آغاز == '{')
		آغاز_brace = BRACE_اندر_COL0;
	    ورنہ
		آغاز_brace = BRACE_AT_آغاز;
	}
	ورنہ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    اگر (ڈھونڈو_آخری_paren(آغاز, '(', ')')
			&& (trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen))
								      != تہی)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    اگر ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ورنہ اگر (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ورنہ
		amount = skip_label(lnum, &l);

	    آغاز_brace = BRACE_AT_انتہا;
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
	ورنہ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    اگر (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ورنہ اگر (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    اگر (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		اگر (ڈھونڈو_مطابقت(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    جاؤ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    اگر (آغاز_brace == BRACE_اندر_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_بایاں_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ورنہ اگر (آغاز_brace == BRACE_AT_آغاز &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ورنہ
	    {
		اگر (آغاز_brace == BRACE_AT_انتہا)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    اگر (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ورنہ اگر (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ورنہ
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
	    ورنہ اگر (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ورنہ
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
	    برائے (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.ستون = 0;

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
			    ورنہ اگر (!curbuf->b_ind_js)
				amount += ind_continuation;
			    توڑو;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_ڈھونڈو_آغاز_CORS(تہی);
			اگر (trypos != تہی)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.ستون = 0;
			    جاری;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    جاری;

			اگر (cin_nocode(l))
			    جاری;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			اگر (آغاز_brace != BRACE_اندر_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    اگر (terminated == ',')
				توڑو;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    اگر (terminated != ';' && cin_isinit())
				توڑو;

			    // nothing useful found
			    اگر (terminated == 0 || terminated == '{')
				جاری;
			}

			اگر (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = تہی;
			    اگر (ڈھونڈو_آخری_paren(l, '(', ')'))
				trypos = ڈھونڈو_مطابقت_paren(
						      curbuf->b_ind_maxparen);

			    اگر (trypos == تہی && ڈھونڈو_آخری_paren(l, '{', '}'))
				trypos = ڈھونڈو_آغاز_brace();

			    اگر (trypos != تہی)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ستون = 0;
				جاری;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			اگر (cont_amount > 0)
			    amount = cont_amount;
			ورنہ
			    amount += ind_continuation;
		    }
		    ورنہ اگر (lookfor == LOOKFOR_UNTERM)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			ورنہ
			    amount += ind_continuation;
		    }
		    ورنہ
		    {
			اگر (lookfor != LOOKFOR_رکن
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    اگر (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_مقصد_amount = curbuf->b_ind_open_extra;
			    }
			}

			اگر (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    اگر (curwin->w_cursor.lnum == ourscope)
				جاری;

			    اگر (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - ڈھونڈو_NAMESPACE_LIM)
				توڑو;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_ڈھونڈو_آغاز_CORS(تہی);
			    اگر (trypos != تہی)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.ستون = 0;
				جاری;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				جاری;

			    // Finally the actual check for "namespace".
			    اگر (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_مقصد_amount;
				توڑو;
			    }
			    ورنہ اگر (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_مقصد_amount;
				توڑو;
			    }

			    اگر (cin_nocode(l))
				جاری;
			}
		    }
		    توڑو;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		اگر ((trypos = ind_ڈھونڈو_آغاز_CORS(&raw_string_آغاز)) != تہی)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.ستون = 0;
		    جاری;
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
			توڑو;

		    // When looking for a "do" we are not interested in
		    // labels.
		    اگر (whilelevel > 0)
			جاری;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    اگر (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			اگر (cont_amount > 0)
			    amount = cont_amount;
			ورنہ
			    amount += ind_continuation;
			توڑو;
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
			اگر ((trypos = ڈھونڈو_آغاز_brace()) == تہی
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    توڑو;
			}
			جاری;
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
		    اگر (lookfor == LOOKFOR_رکن)
		    {
			اگر (n)
			    amount = n;

			اگر (!lookfor_break)
			    توڑو;
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
			اگر (l != تہی && cin_is_cinword(l))
			{
			    اگر (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ورنہ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			توڑو;
		    }

		    // Try to get the indent of a statement before the switch
		    // label.  If nothing is found, line up relative to the
		    // switch label.
		    //	    break;		<- may line up with this line
		    //	 case xx:
		    // ->   y = 1;
		    scope_amount = get_indent() + (iscase    // XXX
					? curbuf->b_ind_case_رمز
					: curbuf->b_ind_scopedecl_رمز);
		    lookfor = curbuf->b_ind_case_break
					      ? LOOKFOR_NOBREAK : LOOKFOR_ANY;
		    جاری;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		اگر (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    اگر (ڈھونڈو_آخری_paren(l, '{', '}')
				     && (trypos = ڈھونڈو_آغاز_brace()) != تہی)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.ستون = 0;
		    }
		    جاری;
		}

		// Ignore jump labels with nothing after them.
		اگر (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    اگر (l == تہی || cin_nocode(l))
			جاری;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    جاری;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		اگر (lookfor != LOOKFOR_رکن && curbuf->b_ind_cpp_baseclass > 0)
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
			ورنہ
			    amount += ind_continuation;
		    }
		    ورنہ اگر (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			جاری;
		    }
		    ورنہ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.ستون);
		    توڑو;
		}
		ورنہ اگر (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    اگر (cin_isterminated(l, TRUE, FALSE))
			توڑو;
		    ورنہ
			جاری;
		}

		// What happens next depends on the line being terminated.
		// If terminated with a ',' only consider it terminating if
		// there is another unterminated statement behind, eg:
		//   123,
		//   sizeof
		//یہاں
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
		    توڑو;
		}
		اگر (lookfor == LOOKFOR_COMMA)
		{
		    اگر (tryposBrace != تہی && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			توڑو;
		    اگر (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			توڑو;
		    ورنہ
		    {
			amount = get_indent();
			اگر (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    توڑو;
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
		    (خالی)ڈھونڈو_آخری_paren(l, '(', ')');
		    trypos = ڈھونڈو_مطابقت_paren(corr_ind_maxparen(&cur_curpos));
		    اگر (trypos != تہی && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->ستون < tryposBrace->ستون)))
			trypos = تہی;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    اگر (trypos == تہی && terminated == ',')
		    {
			اگر (ڈھونڈو_آخری_paren(l, '{', '}'))
			    trypos = ڈھونڈو_آغاز_brace();
			l = ml_get_curline();
		    }

		    اگر (trypos != تہی)
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
			    curwin->w_cursor.ستون = 0;
			    جاری;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    اگر (terminated == ',')
		    {
			جب (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    اگر (*l == NUL || l[STRLEN(l) - 1] != '\\')
				توڑو;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.ستون = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    اگر (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ورنہ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    اگر (terminated != ',' && lookfor != LOOKFOR_رکن
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
			    جاری;
			}
			توڑو;
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
			    ورنہ
				amount += ind_continuation;
			    توڑو;
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
			اگر (lookfor != LOOKFOR_رکن)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    توڑو;
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
				توڑو;
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
				curwin->w_cursor.ستون =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    اگر ((trypos = ڈھونڈو_آغاز_brace()) == تہی
				       || ڈھونڈو_مطابقت(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				توڑو;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ورنہ
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
			    توڑو;
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
				    توڑو;

				lookfor = LOOKFOR_CPP_BASECLASS;
				جاری;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    اگر (amount > cur_amount)
				amount = cur_amount;
			}
			ورنہ
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
				توڑو;

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
					توڑو;
				    lookfor = LOOKFOR_COMMA;
				    trypos = ڈھونڈو_مطابقت_char('[',
						      curbuf->b_ind_maxparen);
				    اگر (trypos != تہی)
				    {
					اگر (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    توڑو;
					}
					ourscope = trypos->lnum;
				    }
				}
				ورنہ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_پہلا_id_amount();
				}
			    }
			    ورنہ
			    {
				اگر (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				اگر (lookfor != LOOKFOR_رکن
						&& lookfor != LOOKFOR_JS_کلید
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_آغاز != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ورنہ اگر (cin_iswhileofdo_انتہا(terminated)) // XXX
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
			ورنہ
			    amount += ind_continuation;
			توڑو;
		    }

		    اگر (whilelevel == 0)
		    {
			lookfor = LOOKFOR_رکن;
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
		ورنہ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    اگر (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			جاری;
		    }

		    // Handle "do {" line.
		    اگر (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			اگر (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    جاری;
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
			ورنہ
			    amount += ind_continuation;
			توڑو;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    اگر (lookfor == LOOKFOR_رکن)
		    {
			اگر (!lookfor_break && whilelevel == 0)
			    توڑو;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ورنہ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
رکن_again:
			l = ml_get_curline();
			اگر (ڈھونڈو_آخری_paren(l, '(', ')')
				&& (trypos = ڈھونڈو_مطابقت_paren(
					   curbuf->b_ind_maxparen)) != تہی)
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
				curwin->w_cursor.ستون = 0;
				جاری;
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
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_رکن;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			اگر (lookfor == LOOKFOR_رکن
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    اگر ((trypos = ڈھونڈو_آغاز_brace()) == تہی
				       || ڈھونڈو_مطابقت(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				توڑو;
			    جاری;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			اگر (ڈھونڈو_آخری_paren(l, '{', '}') // XXX
				     && (trypos = ڈھونڈو_آغاز_brace()) != تہی)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    اگر (*l == '}' || !cin_iselse(l))
				جاؤ رکن_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.ستون = 0;
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

      جاؤ theend;
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
	amount = curbuf->b_ind_پہلا_open;
	جاؤ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    اگر (cur_curpos.lnum < curbuf->b_ml.ml_سطر_گنتی
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == تہی
	    && vim_strchr(theline, '}') == تہی
	    && !cin_سرے_اندر(theline, (char_u *)":")
	    && !cin_سرے_اندر(theline, (char_u *)",")
	    && cin_isfuncdecl(تہی, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	جاؤ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    جب (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.ستون = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	اگر ((trypos = ind_ڈھونڈو_آغاز_CORS(تہی)) != تہی)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.ستون = 0;
	    جاری;
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
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.ستون);
	    توڑو;
	}

	// Skip preprocessor directives and blank lines.
	اگر (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    جاری;

	اگر (cin_nocode(l))
	    جاری;

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
	اگر (cin_سرے_اندر(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    اگر (ڈھونڈو_آخری_paren(l, '(', ')')
		    && (trypos = ڈھونڈو_مطابقت_paren(
				     curbuf->b_ind_maxparen)) != تہی)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    جب (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		اگر (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    توڑو;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.ستون = 0;
	    }

	    amount = get_indent();	    // XXX

	    اگر (amount == 0)
		amount = cin_پہلا_id_amount();
	    اگر (amount == 0)
		amount = ind_continuation;
	    توڑو;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	اگر (cin_isfuncdecl(تہی, cur_curpos.lnum, 0))  // XXX
	    توڑو;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	اگر (*skipwhite(l) == '}')
	    توڑو;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	اگر (cin_سرے_اندر(l, (char_u *)"};"))
	    توڑو;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	اگر (cin_سرے_اندر(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    توڑو;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	اگر (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    جب (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		اگر (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    توڑو;
	    }
	    اگر (curwin->w_cursor.lnum > 0
			    && cin_سرے_اندر(look, (char_u *)"}"))
		توڑو;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	اگر (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    توڑو;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	اگر (cin_سرے_اندر(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    اگر (cin_سرے_اندر(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		توڑو;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	ڈھونڈو_آخری_paren(l, '(', ')');

	اگر ((trypos = ڈھونڈو_مطابقت_paren(curbuf->b_ind_maxparen)) != تہی)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	توڑو;
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
	    ورنہ اگر (cur_amount == 0)
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

    واپس amount;
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
اندر_cinkeys(
    صحیح		keytyped,
    صحیح		when,
    صحیح		سطر_is_خالی_خانے)
{
    char_u	*look;
    صحیح		try_مطابقت;
    صحیح		try_مطابقت_لفظ;
    char_u	*p;
    char_u	*سطر;
    صحیح		icase;
    صحیح		i;

    اگر (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	واپس FALSE;

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ورنہ
#اگر_ختم
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    جب (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	انتخاب (when)
	{
	    حالت '*': try_مطابقت = (*look == '*'); توڑو;
	    حالت '!': try_مطابقت = (*look == '!'); توڑو;
	    طےشدہ: try_مطابقت = (*look != '*'); توڑو;
	}
	اگر (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	اگر (*look == '0')
	{
	    try_مطابقت_لفظ = try_مطابقت;
	    اگر (!سطر_is_خالی_خانے)
		try_مطابقت = FALSE;
	    ++look;
	}
	ورنہ
	    try_مطابقت_لفظ = FALSE;

	// does it look like a control character?
	اگر (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    اگر (try_مطابقت && keytyped == Ctrl_chr(look[1]))
		واپس TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ورنہ اگر (*look == 'o')
	{
	    اگر (try_مطابقت && keytyped == کلید_OPEN_FORW)
		واپس TRUE;
	    ++look;
	}
	ورنہ اگر (*look == 'O')
	{
	    اگر (try_مطابقت && keytyped == کلید_OPEN_عقب)
		واپس TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ورنہ اگر (*look == 'e')
	{
	    اگر (try_مطابقت && keytyped == 'e' && curwin->w_cursor.ستون >= 4)
	    {
		p = ml_get_curline();
		اگر (skipwhite(p) == p + curwin->w_cursor.ستون - 4 &&
			STRNCMP(p + curwin->w_cursor.ستون - 4, "else", 4) == 0)
		    واپس TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ورنہ اگر (*look == ':')
	{
	    اگر (try_مطابقت && keytyped == ':')
	    {
		p = ml_get_curline();
		اگر (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    واپس TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		اگر (curwin->w_cursor.ستون > 2
			&& p[curwin->w_cursor.ستون - 1] == ':'
			&& p[curwin->w_cursor.ستون - 2] == ':')
		{
		    p[curwin->w_cursor.ستون - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.ستون - 1] = ':';
		    اگر (i)
			واپس TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ورنہ اگر (*look == '<')
	{
	    اگر (try_مطابقت)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		اگر (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != تہی
						       && keytyped == look[1])
		    واپس TRUE;

		اگر (keytyped == get_special_کلید_رمز(look + 1))
		    واپس TRUE;
	    }
	    جب (*look && *look != '>')
		look++;
	    جب (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ورنہ اگر (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    اگر (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ورنہ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    اگر (p == تہی)
		p = look + STRLEN(look);
	    اگر ((try_مطابقت || try_مطابقت_لفظ)
		    && curwin->w_cursor.ستون >= (colnr_T)(p - look))
	    {
		صحیح		مطابقت = FALSE;

		اگر (keytyped == کلید_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    سطر = ml_get_curline();
		    اگر (has_mbyte)
		    {
			char_u	*n;

			برائے (s = سطر + curwin->w_cursor.ستون; s > سطر; s = n)
			{
			    n = mb_prevptr(سطر, s);
			    اگر (!vim_iswordp(n))
				توڑو;
			}
		    }
		    ورنہ
			برائے (s = سطر + curwin->w_cursor.ستون; s > سطر; --s)
			    اگر (!vim_iswordc(s[-1]))
				توڑو;
		    اگر (s + (p - look) <= سطر + curwin->w_cursor.ستون
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			مطابقت = TRUE;
		}
		ورنہ
		    // TODO: multi-byte
		    اگر (keytyped == (صحیح)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((صحیح)p[-1])))
		{
		    سطر = ml_get_cursor();
		    اگر ((curwin->w_cursor.ستون == (colnr_T)(p - look)
				|| !vim_iswordc(سطر[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(سطر - (p - look), look, p - look)
				: STRNCMP(سطر - (p - look), look, p - look))
									 == 0)
			مطابقت = TRUE;
		}
		اگر (مطابقت && try_مطابقت_لفظ && !try_مطابقت)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    اگر (getwhitecols_curline() !=
				     (صحیح)(curwin->w_cursor.ستون - (p - look)))
			مطابقت = FALSE;
		}
		اگر (مطابقت)
		    واپس TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ورنہ
	{
	    اگر (try_مطابقت && *look == keytyped)
		واپس TRUE;
	    اگر (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_مقصد_option_part(look);
    }
    واپس FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    خالی
do_c_اظہاریہ_indent(خالی)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (*curbuf->b_p_inde != NUL)
	fixthisline(get_اظہاریہ_indent);
    ورنہ
#اگر_ختم
	fixthisline(get_c_indent);
}

#اگر defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    خالی
f_cindent(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    اگر (lnum >= 1 && lnum <= curbuf->b_ml.ml_سطر_گنتی)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_نمبر = get_c_indent();
	curwin->w_cursor = pos;
    }
    ورنہ
	rettv->vval.v_نمبر = -1;
}
#اگر_ختم
