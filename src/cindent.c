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

#अन्तर्भाव "vim.h"

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
#define LOOKFOR_JS_कुञ्चिका		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    पूर्णाङ्क
cin_is_cinword(char_u *पङ्क्तिः)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    पूर्णाङ्क		cinw_len;
    पूर्णाङ्क		retval = FALSE;
    पूर्णाङ्क		len;

    cinw_len = (पूर्णाङ्क)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    सचे (cinw_buf == रिक्त)
	पटिनिवत्त FALSE;

    पङ्क्तिः = skipwhite(पङ्क्तिः);
    अत्थाय (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	सचे (STRNCMP(पङ्क्तिः, cinw_buf, len) == 0
		&& (!vim_iswordc(पङ्क्तिः[len]) || !vim_iswordc(पङ्क्तिः[len - 1])))
	{
	    retval = TRUE;
	    भङ्ग;
	}
    }
    vim_free(cinw_buf);
    पटिनिवत्त retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    स्थिर char_u *
skip_string(char_u *p)
{
    पूर्णाङ्क	    i;

    // We loop, because strings may be concatenated: "date""time".
    अत्थाय ( ; ; ++p)
    {
	सचे (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    सचे (p[1] == NUL)		    // ' at end of line
		भङ्ग;
	    i = 2;
	    सचे (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		याव (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    सचे (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		अनुवर्त;
	    }
	}
	अञ्ञथा सचे (p[0] == '"')		    // start of string
	{
	    अत्थाय (++p; p[0]; ++p)
	    {
		सचे (p[0] == '\\' && p[1] != NUL)
		    ++p;
		अञ्ञथा सचे (p[0] == '"')	    // end of string
		    भङ्ग;
	    }
	    सचे (p[0] == '"')
		अनुवर्त; // continue for another string
	}
	अञ्ञथा सचे (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    सचे (paren != रिक्त)
	    {
		size_t delim_len = paren - delim;

		अत्थाय (p += 3; *p; ++p)
		    सचे (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			भङ्ग;
		    }
		सचे (p[0] == '"')
		    अनुवर्त; // continue for another string
	    }
	}
	भङ्ग;				    // no string found
    }
    सचे (!*p)
	--p;				    // backup from NUL
    पटिनिवत्त p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    पूर्णाङ्क
is_pos_in_string(char_u *पङ्क्तिः, colnr_T col)
{
    char_u *p;

    अत्थाय (p = पङ्क्तिः; *p && (colnr_T)(p - पङ्क्तिः) < col; ++p)
	p = skip_string(p);
    पटिनिवत्त !((colnr_T)(p - पङ्क्तिः) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    पूर्णाङ्क
check_linecomment(char_u *पङ्क्तिः)
{
    char_u  *p;

    p = पङ्क्तिः;
    // skip Lispish one-line comments
    सचे (curbuf->b_p_lisp)
    {
	सचे (vim_strchr(p, ';') != रिक्त) // there may be comments
	{
	    पूर्णाङ्क in_str = FALSE;	// inside of string

	    p = पङ्क्तिः;		// scan from start
	    याव ((p = vim_strpbrk(p, (char_u *)"\";")) != रिक्त)
	    {
		सचे (*p == '"')
		{
		    सचे (in_str)
		    {
			सचे (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    अञ्ञथा सचे (p == पङ्क्तिः || ((p - पङ्क्तिः) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		अञ्ञथा सचे (!in_str && ((p - पङ्क्तिः) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(पङ्क्तिः, (colnr_T)(p - पङ्क्तिः)))
		    भङ्ग;	// found!
		++p;
	    }
	}
	अञ्ञथा
	    p = रिक्त;
    }
    अञ्ञथा
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	अत्थाय ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    सचे (*p == NUL)
		भङ्ग;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    सचे (p[0] == '/' && p[1] == '/'
				 && (p == पङ्क्तिः || p[-1] != '*' || p[2] != '*'))
		भङ्ग;
	}
	सचे (*p == NUL)
	    p = रिक्त;
    }

    सचे (p == रिक्त)
	पटिनिवत्त MAXCOL;
    पटिनिवत्त (पूर्णाङ्क)(p - पङ्क्तिः);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    स्थिर pos_T *
ind_find_start_comment(शून्य)	    // XXX
{
    पटिनिवत्त find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(पूर्णाङ्क ind_maxcomment)	// XXX
{
    pos_T	*pos;
    पूर्णाङ्क		cur_maxcomment = ind_maxcomment;

    अत्थाय (;;)
    {
	pos = findmatchlimit(रिक्त, '*', FM_BACKWARD, cur_maxcomment);
	सचे (pos == रिक्त)
	    भङ्ग;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	सचे (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    भङ्ग;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	सचे (cur_maxcomment <= 0)
	{
	    pos = रिक्त;
	    भङ्ग;
	}
    }
    पटिनिवत्त pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    स्थिर pos_T *
find_start_rawstring(पूर्णाङ्क ind_maxcomment)	// XXX
{
    pos_T	*pos;
    पूर्णाङ्क		cur_maxcomment = ind_maxcomment;

    अत्थाय (;;)
    {
	pos = findmatchlimit(रिक्त, 'R', FM_BACKWARD, cur_maxcomment);
	सचे (pos == रिक्त)
	    भङ्ग;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	सचे (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    भङ्ग;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	सचे (cur_maxcomment <= 0)
	{
	    pos = रिक्त;
	    भङ्ग;
	}
    }
    पटिनिवत्त pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    स्थिर pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    स्थिर pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    सचे (comment_pos != रिक्त)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    सचे (comment_pos == रिक्त || (rs_pos != रिक्त
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	सचे (is_raw != रिक्त && rs_pos != रिक्त)
	    *is_raw = rs_pos->lnum;
	पटिनिवत्त rs_pos;
    }
    पटिनिवत्त comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    पूर्णाङ्क
cindent_on(शून्य)
{
    पटिनिवत्त (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
प्रकारनिर्देश संरचना {
    पूर्णाङ्क	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    स्थिर char_u *
cin_skipcomment(char_u *s)
{
    याव (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	सचे (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    भङ्ग;
	}
	सचे (*s != '/')
	    भङ्ग;
	++s;
	सचे (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    भङ्ग;
	}
	सचे (*s != '*')
	    भङ्ग;
	अत्थाय (++s; *s; ++s)	// skip slash-star comment
	    सचे (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		भङ्ग;
	    }
    }
    पटिनिवत्त s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    स्थिर पूर्णाङ्क
cin_nocode(char_u *s)
{
    पटिनिवत्त *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    स्थिर पूर्णाङ्क
cin_iscomment(char_u *p)
{
    पटिनिवत्त (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    स्थिर पूर्णाङ्क
cin_islinecomment(char_u *p)
{
    पटिनिवत्त (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    स्थिर pos_T *
find_पङ्क्तिः_comment(शून्य) // XXX
{
    स्थिर pos_T pos;
    char_u	 *पङ्क्तिः;
    char_u	 *p;

    pos = curwin->w_cursor;
    याव (--pos.lnum > 0)
    {
	पङ्क्तिः = ml_get(pos.lnum);
	p = skipwhite(पङ्क्तिः);
	सचे (cin_islinecomment(p))
	{
	    pos.col = (पूर्णाङ्क)(p - पङ्क्तिः);
	    पटिनिवत्त &pos;
	}
	सचे (*p != NUL)
	    भङ्ग;
    }
    पटिनिवत्त रिक्त;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    स्थिर पूर्णाङ्क
cin_has_js_कुञ्चिका(char_u *text)
{
    char_u *s = skipwhite(text);
    पूर्णाङ्क	    quote = -1;

    सचे (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    सचे (!vim_isIDc(*s))	    // need at least one ID character
	पटिनिवत्त FALSE;

    याव (vim_isIDc(*s))
	++s;
    सचे (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    पटिनिवत्त (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    स्थिर पूर्णाङ्क
cin_islabel_skip(char_u **s)
{
    सचे (!vim_isIDc(**s))	    // need at least one ID character
	पटिनिवत्त FALSE;

    याव (vim_isIDc(**s))
    {
	सचे (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	अञ्ञथा
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    पटिनिवत्त (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    स्थिर पूर्णाङ्क
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    पूर्णाङ्क	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    सचे (cinsd_buf == रिक्त)
	पटिनिवत्त FALSE;

    अत्थाय (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (पूर्णाङ्क)cinsd_len, ",");
	सचे (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    सचे (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		भङ्ग;
	    }
	}
    }

    vim_free(cinsd_buf);
    पटिनिवत्त found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    स्थिर पूर्णाङ्क
cin_ispreproc(char_u *s)
{
    सचे (*skipwhite(s) == '#')
	पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    स्थिर पूर्णाङ्क
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, पूर्णाङ्क *amount)
{
    char_u	*पङ्क्तिः = *pp;
    linenr_T	lnum = *lnump;
    पूर्णाङ्क		retval = FALSE;
    पूर्णाङ्क		candidate_amount = *amount;

    सचे (*पङ्क्तिः != NUL && पङ्क्तिः[STRLEN(पङ्क्तिः) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    अत्थाय (;;)
    {
	सचे (cin_ispreproc(पङ्क्तिः))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    भङ्ग;
	}
	सचे (lnum == 1)
	    भङ्ग;
	पङ्क्तिः = ml_get(--lnum);
	सचे (*पङ्क्तिः == NUL || पङ्क्तिः[STRLEN(पङ्क्तिः) - 1] != '\\')
	    भङ्ग;
    }

    सचे (lnum != *lnump)
	*pp = ml_get(*lnump);
    सचे (retval)
	*amount = candidate_amount;
    पटिनिवत्त retval;
}

    स्थिर पूर्णाङ्क
cin_iselse(
    char_u  *p)
{
    सचे (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    पटिनिवत्त (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    स्थिर पूर्णाङ्क
cin_isterminated(
    char_u	*s,
    पूर्णाङ्क		incl_open,	// include '{' at the end as terminator
    पूर्णाङ्क		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    पूर्णाङ्क		is_else = FALSE;

    s = cin_skipcomment(s);

    सचे (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    सचे (!found_start)
	is_else = cin_iselse(s);

    याव (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	सचे (*s == '}' && n_open > 0)
	    --n_open;
	सचे ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    पटिनिवत्त *s;
	अञ्ञथा सचे (*s == '{')
	{
	    सचे (incl_open && cin_nocode(s + 1))
		पटिनिवत्त *s;
	    अञ्ञथा
		++n_open;
	}

	सचे (*s)
	    s++;
    }
    पटिनिवत्त found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    स्थिर पूर्णाङ्क
cin_starts_with(char_u *s, वर्ण *word)
{
    पूर्णाङ्क l = (पूर्णाङ्क)STRLEN(word);

    पटिनिवत्त (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    स्थिर पूर्णाङ्क
cin_isdefault(char_u *s)
{
    पटिनिवत्त (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    स्थिर पूर्णाङ्क
cin_iscase(
    char_u *s,
    पूर्णाङ्क strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    सचे (cin_starts_with(s, "case"))
    {
	अत्थाय (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    सचे (*s == NUL)
		भङ्ग;
	    सचे (*s == ':')
	    {
		सचे (s[1] == ':')	// skip over "::" for C++
		    ++s;
		अञ्ञथा
		    पटिनिवत्त TRUE;
	    }
	    सचे (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    अञ्ञथा सचे (*s == '/' && (s[1] == '*' || s[1] == '/'))
		पटिनिवत्त FALSE;		// stop at comment
	    अञ्ञथा सचे (*s == '"')
	    {
		// JS etc.
		सचे (strict)
		    पटिनिवत्त FALSE;		// stop at string
		अञ्ञथा
		    पटिनिवत्त TRUE;
	    }
	}
	पटिनिवत्त FALSE;
    }

    सचे (cin_isdefault(s))
	पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    स्थिर पूर्णाङ्क
cin_islabel(शून्य)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    सचे (cin_isdefault(s))
	पटिनिवत्त FALSE;
    सचे (cin_isscopedecl(s))
	पटिनिवत्त FALSE;

    सचे (!cin_islabel_skip(&s))
	पटिनिवत्त FALSE;

    सचे (ind_find_start_CORS(रिक्त))
	पटिनिवत्त FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*पङ्क्तिः;

    cursor_save = curwin->w_cursor;
    याव (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	सचे ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त) // XXX
	    curwin->w_cursor = *trypos;

	पङ्क्तिः = ml_get_curline();
	सचे (cin_ispreproc(पङ्क्तिः))	// ignore #defines, #if, etc.
	    अनुवर्त;
	सचे (*(पङ्क्तिः = cin_skipcomment(पङ्क्तिः)) == NUL)
	    अनुवर्त;

	curwin->w_cursor = cursor_save;
	सचे (cin_isterminated(पङ्क्तिः, TRUE, FALSE)
		|| cin_isscopedecl(पङ्क्तिः)
		|| cin_iscase(पङ्क्तिः, TRUE)
		|| (cin_islabel_skip(&पङ्क्तिः) && cin_nocode(पङ्क्तिः)))
	    पटिनिवत्त TRUE;
	पटिनिवत्त FALSE;
    }
    curwin->w_cursor = cursor_save;
    पटिनिवत्त TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    स्थिर पूर्णाङ्क
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    पूर्णाङ्क		len = (पूर्णाङ्क)STRLEN(find);

    याव (*p != NUL)
    {
	p = cin_skipcomment(p);
	सचे (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    सचे (cin_nocode(r))
		पटिनिवत्त TRUE;
	}
	सचे (*p != NUL)
	    ++p;
    }
    पटिनिवत्त FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    स्थिर char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = रिक्त, *p = s;
    करोहि
    {
	r = p;
	p = cin_skipcomment(p);
	सचे (*p)
	    p = skip_string(p);
    } याव (p != r);
    पटिनिवत्त p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    स्थिर पूर्णाङ्क
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = रिक्त;

    याव (*p)
    {
	सचे (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	अञ्ञथा सचे (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	अञ्ञथा
	    p = cin_skip_comment_and_string(p + 1);
    }
    सचे (!r)
	पटिनिवत्त FALSE;
    p = r; // p points now after '=' or "return"

    सचे (cin_nocode(p))
	पटिनिवत्त TRUE;

    सचे (*p == '&')
	p = cin_skipcomment(p + 1);

    सचे (*p == '(') // skip a typecast
    {
	पूर्णाङ्क open_गणनम् = 1;
	करोहि
	{
	    p = cin_skip_comment_and_string(p + 1);
	    सचे (cin_nocode(p))
		पटिनिवत्त TRUE;
	    open_गणनम् += (*p == '(') - (*p == ')');
	} याव (open_गणनम्);
	p = cin_skipcomment(p + 1);
	सचे (cin_nocode(p))
	    पटिनिवत्त TRUE;
    }

    याव (*p == '{')
	p = cin_skipcomment(p + 1);
    पटिनिवत्त cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    स्थिर पूर्णाङ्क
cin_isinit(शून्य)
{
    char_u	*s;
    स्थिर वर्ण *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    सचे (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    अत्थाय (;;)
    {
	पूर्णाङ्क i, l;

	अत्थाय (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(skip); ++i)
	{
	    l = (पूर्णाङ्क)strlen(skip[i]);
	    सचे (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		भङ्ग;
	    }
	}
	सचे (l != 0)
	    भङ्ग;
    }

    सचे (cin_starts_with(s, "enum"))
	पटिनिवत्त TRUE;

    पटिनिवत्त cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    स्थिर पूर्णाङ्क
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    पूर्णाङ्क		has_name = FALSE;
    पूर्णाङ्क		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    याव ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    सचे (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	याव (*p != NUL)
	{
	    सचे (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    अञ्ञथा सचे (*p == '{')
	    {
		भङ्ग;
	    }
	    अञ्ञथा सचे (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		सचे (has_name)
		    पटिनिवत्त FALSE; // word character after skipping past name
		++p;
	    }
	    अञ्ञथा सचे (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		सचे (!has_name_start || has_name)
		    पटिनिवत्त FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    अञ्ञथा
	    {
		पटिनिवत्त FALSE;
	    }
	}
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    स्थिर पूर्णाङ्क
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    पूर्णाङ्क		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    सचे (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	याव (*p != NUL)
	{
	    सचे (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    अञ्ञथा सचे (*p == '{')
	    {
		भङ्ग;
	    }
	    अञ्ञथा सचे (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		सचे (has_string_literal)
		    पटिनिवत्त FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    अञ्ञथा सचे (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		सचे (has_string_literal)
		    पटिनिवत्त FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    अञ्ञथा
	    {
		पटिनिवत्त FALSE;
	    }
	}
	पटिनिवत्त has_string_literal ? TRUE : FALSE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    स्थिर char_u *
after_label(char_u *l)
{
    अत्थाय ( ; *l; ++l)
    {
	सचे (*l == ':')
	{
	    सचे (l[1] == ':')	    // skip over "::" for C++
		++l;
	    अञ्ञथा सचे (!cin_iscase(l + 1, FALSE))
		भङ्ग;
	}
	अञ्ञथा सचे (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    सचे (*l == NUL)
	पटिनिवत्त रिक्त;
    l = cin_skipcomment(l + 1);
    सचे (*l == NUL)
	पटिनिवत्त रिक्त;
    पटिनिवत्त l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    स्थिर पूर्णाङ्क
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    सचे (p == रिक्त)
	पटिनिवत्त 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    पटिनिवत्त (पूर्णाङ्क)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    स्थिर पूर्णाङ्क
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    पूर्णाङ्क		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    सचे (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	सचे (l == रिक्त)		// just in case
	    l = ml_get_curline();
    }
    अञ्ञथा
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    पटिनिवत्त amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    स्थिर पूर्णाङ्क
cin_first_id_amount(शून्य)
{
    char_u	*पङ्क्तिः, *p, *s;
    पूर्णाङ्क		len;
    pos_T	fp;
    colnr_T	col;

    पङ्क्तिः = ml_get_curline();
    p = skipwhite(पङ्क्तिः);
    len = (पूर्णाङ्क)(skiptowhite(p) - p);
    सचे (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (पूर्णाङ्क)(skiptowhite(p) - p);
    }
    सचे (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    अञ्ञथा सचे (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    अञ्ञथा सचे ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	सचे ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    अत्थाय (len = 0; vim_isIDc(p[len]); ++len)
	;
    सचे (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	पटिनिवत्त 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - पङ्क्तिः);
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    पटिनिवत्त (पूर्णाङ्क)col;
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
    स्थिर पूर्णाङ्क
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*पङ्क्तिः;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    सचे (lnum > 1)
    {
	पङ्क्तिः = ml_get(lnum - 1);
	सचे (*पङ्क्तिः != NUL && पङ्क्तिः[STRLEN(पङ्क्तिः) - 1] == '\\')
	    पटिनिवत्त -1;
    }

    पङ्क्तिः = s = ml_get(lnum);
    याव (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == रिक्त)
    {
	सचे (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अञ्ञथा
	    ++s;
    }
    सचे (*s != '=')
	पटिनिवत्त 0;

    s = skipwhite(s + 1);
    सचे (cin_nocode(s))
	पटिनिवत्त 0;

    सचे (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - पङ्क्तिः);
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    पटिनिवत्त (पूर्णाङ्क)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    स्थिर पूर्णाङ्क
cin_skip2pos(pos_T *trypos)
{
    char_u	*पङ्क्तिः;
    char_u	*p;
    char_u	*new_p;

    p = पङ्क्तिः = ml_get(trypos->lnum);
    याव (*p && (colnr_T)(p - पङ्क्तिः) < trypos->col)
    {
	सचे (cin_iscomment(p))
	    p = cin_skipcomment(p);
	अञ्ञथा
	{
	    new_p = skip_string(p);
	    सचे (new_p == p)
		++p;
	    अञ्ञथा
		p = new_p;
	}
    }
    पटिनिवत्त (पूर्णाङ्क)(p - पङ्क्तिः);
}

    स्थिर pos_T *
find_match_char(पूर्णाङ्क c, पूर्णाङ्क ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    स्थिर pos_T pos_copy;
    पूर्णाङ्क		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    सचे ((trypos = findmatchlimit(रिक्त, c, FM_SKIPCOMM, ind_maxp_wk)) != रिक्त)
    {
	// check if the ( is in a // comment
	सचे ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (पूर्णाङ्क)(cursor_save.lnum - trypos->lnum);
	    सचे (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		गच्छ retry;
	    }
	    trypos = रिक्त;
	}
	अञ्ञथा
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    सचे ((trypos_wk = ind_find_start_CORS(रिक्त)) != रिक्त) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (पूर्णाङ्क)(cursor_save.lnum
			- trypos_wk->lnum);
		सचे (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    गच्छ retry;
		}
		trypos = रिक्त;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    पटिनिवत्त trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    स्थिर pos_T *
find_match_paren(पूर्णाङ्क ind_maxparen)	// XXX
{
    पटिनिवत्त find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    स्थिर पूर्णाङ्क
find_last_paren(char_u *l, पूर्णाङ्क start, पूर्णाङ्क end)
{
    पूर्णाङ्क		i;
    पूर्णाङ्क		retval = FALSE;
    पूर्णाङ्क		open_गणनम् = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    अत्थाय (i = 0; l[i] != NUL; i++)
    {
	i = (पूर्णाङ्क)(cin_skipcomment(l + i) - l); // ignore parens in comments
	सचे (l[i] == NUL)
	    भङ्ग;
	i = (पूर्णाङ्क)(skip_string(l + i) - l);    // ignore parens in quotes
	सचे (l[i] == start)
	    ++open_गणनम्;
	अञ्ञथा सचे (l[i] == end)
	{
	    सचे (open_गणनम् > 0)
		--open_गणनम्;
	    अञ्ञथा
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    पटिनिवत्त retval;
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
    स्थिर पूर्णाङ्क
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    पूर्णाङ्क		retval = FALSE;
    pos_T	*trypos;
    पूर्णाङ्क		just_started = TRUE;

    सचे (sp == रिक्त)
	s = ml_get(lnum);
    अञ्ञथा
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    सचे (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
    {
	lnum = trypos->lnum;
	सचे (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    पटिनिवत्त FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    सचे (cin_ispreproc(s))
	पटिनिवत्त FALSE;

    याव (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	सचे (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अञ्ञथा सचे (*s == ':')
	{
	    सचे (*(s + 1) == ':')
		s += 2;
	    अञ्ञथा
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		पटिनिवत्त FALSE;
	}
	अञ्ञथा
	    ++s;
    }
    सचे (*s != '(')
	पटिनिवत्त FALSE;		// ';', ' or "  before any () or no '('

    याव (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	सचे (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    सचे (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    गच्छ done;
	}
	सचे ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    पूर्णाङ्क comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    अत्थाय (;;)
	    {
		सचे (lnum >= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		    भङ्ग;
		s = ml_get(++lnum);
		सचे (!cin_ispreproc(s))
		    भङ्ग;
	    }
	    सचे (lnum >= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		भङ्ग;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    सचे (!just_started && (!comma && *s != ',' && *s != ')'))
		भङ्ग;
	    just_started = FALSE;
	}
	अञ्ञथा सचे (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अञ्ञथा
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    सचे (lnum != first_lnum && sp != रिक्त)
	*sp = ml_get(first_lnum);

    पटिनिवत्त retval;
}

    स्थिर पूर्णाङ्क
cin_isif(char_u *p)
{
    पटिनिवत्त (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    स्थिर पूर्णाङ्क
cin_isdo(char_u *p)
{
    पटिनिवत्त (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    स्थिर पूर्णाङ्क
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    पूर्णाङ्क		retval = FALSE;

    p = cin_skipcomment(p);
    सचे (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    सचे (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	याव (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	सचे ((trypos = findmatchlimit(रिक्त, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != रिक्त
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    पटिनिवत्त retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    स्थिर पूर्णाङ्क
cin_is_if_for_while_before_offset(char_u *पङ्क्तिः, पूर्णाङ्क *poffset)
{
    पूर्णाङ्क offset = *poffset;

    सचे (offset-- < 2)
	पटिनिवत्त 0;
    याव (offset > 2 && VIM_ISWHITE(पङ्क्तिः[offset]))
	--offset;

    offset -= 1;
    सचे (STRNCMP(पङ्क्तिः + offset, "if", 2) == 0)
	गच्छ probablyFound;

    सचे (offset >= 1)
    {
	offset -= 1;
	सचे (STRNCMP(पङ्क्तिः + offset, "for", 3) == 0)
	    गच्छ probablyFound;

	सचे (offset >= 2)
	{
	    offset -= 2;
	    सचे (STRNCMP(पङ्क्तिः + offset, "while", 5) == 0)
		गच्छ probablyFound;
	}
    }
    पटिनिवत्त 0;

probablyFound:
    सचे (!offset || !vim_isIDc(पङ्क्तिः[offset - 1]))
    {
	*poffset = offset;
	पटिनिवत्त 1;
    }
    पटिनिवत्त 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    स्थिर पूर्णाङ्क
cin_iswhileofdo_end(पूर्णाङ्क terminated)
{
    char_u	*पङ्क्तिः;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    पूर्णाङ्क		i;

    सचे (terminated != ';')	// there must be a ';' at the end
	पटिनिवत्त FALSE;

    p = पङ्क्तिः = ml_get_curline();
    याव (*p != NUL)
    {
	p = cin_skipcomment(p);
	सचे (*p == ')')
	{
	    s = skipwhite(p + 1);
	    सचे (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (पूर्णाङ्क)(p - पङ्क्तिः);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		सचे (trypos != रिक्त)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    सचे (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    सचे (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			पटिनिवत्त TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		पङ्क्तिः = ml_get_curline();
		p = पङ्क्तिः + i;
	    }
	}
	सचे (*p != NUL)
	    ++p;
    }
    पटिनिवत्त FALSE;
}

    स्थिर पूर्णाङ्क
cin_isbreak(char_u *p)
{
    पटिनिवत्त (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    स्थिर पूर्णाङ्क
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    पूर्णाङ्क		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*पङ्क्तिः = ml_get_curline();

    सचे (pos->lnum <= lnum)
	पटिनिवत्त cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(पङ्क्तिः);
    सचे (*s == '#')		// skip #define FOO x ? (x) : x
	पटिनिवत्त FALSE;
    s = cin_skipcomment(s);
    सचे (*s == NUL)
	पटिनिवत्त FALSE;

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
    याव (lnum > 1)
    {
	पङ्क्तिः = ml_get(lnum - 1);
	s = skipwhite(पङ्क्तिः);
	सचे (*s == '#' || *s == NUL)
	    भङ्ग;
	याव (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    सचे (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		भङ्ग;
	    सचे (*s != NUL)
		++s;
	}
	सचे (*s != NUL)
	    भङ्ग;
	--lnum;
    }

    pos->lnum = lnum;
    पङ्क्तिः = ml_get(lnum);
    s = पङ्क्तिः;
    अत्थाय (;;)
    {
	सचे (*s == NUL)
	{
	    सचे (lnum == curwin->w_cursor.lnum)
		भङ्ग;
	    // Continue in the cursor line.
	    पङ्क्तिः = ml_get(++lnum);
	    s = पङ्क्तिः;
	}
	सचे (s == पङ्क्तिः)
	{
	    // don't recognize "case (foo):" as a baseclass
	    सचे (cin_iscase(s, FALSE))
		भङ्ग;
	    s = cin_skipcomment(पङ्क्तिः);
	    सचे (*s == NUL)
		अनुवर्त;
	}

	सचे (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	अञ्ञथा सचे (s[0] == ':')
	{
	    सचे (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    अञ्ञथा सचे (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    अञ्ञथा
		s = cin_skipcomment(s + 1);
	}
	अञ्ञथा सचे ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    सचे (*s == 'c')
		s = cin_skipcomment(s + 5);
	    अञ्ञथा
		s = cin_skipcomment(s + 6);
	}
	अञ्ञथा
	{
	    सचे (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    अञ्ञथा सचे (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    अञ्ञथा सचे (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		पटिनिवत्त FALSE;
	    }
	    अञ्ञथा सचे (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    अञ्ञथा सचे (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		सचे (cpp_base_class)
		    pos->col = (colnr_T)(s - पङ्क्तिः);
	    }

	    // When the line ends in a comma don't align with it.
	    सचे (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    सचे (cpp_base_class)
	pos->lnum = lnum;
    पटिनिवत्त cpp_base_class;
}

    स्थिर पूर्णाङ्क
get_baseclass_amount(पूर्णाङ्क col)
{
    पूर्णाङ्क		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    सचे (col == 0)
    {
	amount = get_indent();
	सचे (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	सचे (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    अञ्ञथा
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, रिक्त, रिक्त, 0);
	amount = (पूर्णाङ्क)vcol;
    }
    सचे (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    पटिनिवत्त amount;
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

    स्थिर pos_T *
find_start_brace(शून्य)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    स्थिर pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    याव ((trypos = findmatchlimit(रिक्त, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != रिक्त)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = रिक्त;
	// ignore the { if it's in a // or / *  * / comment
	सचे ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(रिक्त)) == रिक्त) // XXX
	    भङ्ग;
	सचे (pos != रिक्त)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    पटिनिवत्त trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    स्थिर pos_T *
find_match_paren_after_brace (पूर्णाङ्क ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    सचे (trypos == रिक्त)
	पटिनिवत्त रिक्त;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    सचे (tryposBrace != रिक्त
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = रिक्त;
    पटिनिवत्त trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    स्थिर पूर्णाङ्क
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    सचे (n > 0 && n < curbuf->b_ind_maxparen / 2)
	पटिनिवत्त curbuf->b_ind_maxparen - (पूर्णाङ्क)n;
    पटिनिवत्त curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    शून्य
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    पूर्णाङ्क		divider;
    पूर्णाङ्क		fraction = 0;
    पूर्णाङ्क		sw;
    long	t = get_sw_मानम्(buf);

    // needed for cino-(, it will be multiplied by 2 again
    सचे (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    अञ्ञथा
	sw = (पूर्णाङ्क)t;

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
    buf->b_ind_open_वामः_imag = 0;

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

    अत्थाय (p = buf->b_p_cino; *p; )
    {
	l = p++;
	सचे (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	सचे (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((वर्ण *)++p);
	    याव (VIM_ISDIGIT(*p))
	    {
		++p;
		सचे (divider)
		    divider *= 10;
		अञ्ञथा
		    divider = 10;
	    }
	}
	सचे (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    सचे (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    अञ्ञथा
	    {
		n *= sw;
		सचे (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	सचे (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	विकल्प (*l)
	{
	    पक्ष '>': buf->b_ind_level = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'e': buf->b_ind_open_imag = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'n': buf->b_ind_no_brace = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'f': buf->b_ind_first_open = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '{': buf->b_ind_open_extra = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '}': buf->b_ind_close_extra = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '^': buf->b_ind_open_वामः_imag = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'L': buf->b_ind_jump_label = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष ':': buf->b_ind_case = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '=': buf->b_ind_case_code = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'b': buf->b_ind_case_break = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'p': buf->b_ind_param = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 't': buf->b_ind_func_type = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '/': buf->b_ind_comment = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'c': buf->b_ind_in_comment = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'C': buf->b_ind_in_comment2 = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'i': buf->b_ind_cpp_baseclass = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '+': buf->b_ind_continuation = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '(': buf->b_ind_unclosed = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'u': buf->b_ind_unclosed2 = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'U': buf->b_ind_unclosed_noignore = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'W': buf->b_ind_unclosed_wrapped = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'w': buf->b_ind_unclosed_whiteok = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'm': buf->b_ind_matching_paren = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'M': buf->b_ind_paren_prev = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष ')': buf->b_ind_maxparen = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '*': buf->b_ind_maxcomment = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'g': buf->b_ind_scopedecl = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'h': buf->b_ind_scopedecl_code = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'j': buf->b_ind_java = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'J': buf->b_ind_js = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'l': buf->b_ind_keep_case_label = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष '#': buf->b_ind_hash_comment = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'N': buf->b_ind_cpp_namespace = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'k': buf->b_ind_if_for_while = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'E': buf->b_ind_cpp_extern_c = (पूर्णाङ्क)n; भङ्ग;
	    पक्ष 'P': buf->b_ind_pragma = (पूर्णाङ्क)n; भङ्ग;
	}
	सचे (*p == ',')
	    ++p;
    }
}

    स्थिर पूर्णाङ्क
find_match(पूर्णाङ्क lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    पूर्णाङ्क		elselevel;
    पूर्णाङ्क		whilelevel;

    सचे (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    अञ्ञथा
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    याव (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	सचे (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    सचे (theirscope == रिक्त)
		भङ्ग;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    सचे (theirscope->lnum < ourscope)
		भङ्ग;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    सचे (theirscope->lnum > ourscope)
		अनुवर्त;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    सचे (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		सचे (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    सचे (!cin_isif(mightbeif))
			++elselevel;
		    अनुवर्त;
		}

		// If it's an "if" decrement elselevel
		सचे (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    सचे (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    सचे (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		अनुवर्त;
	    }

	    // If it's a "do" decrement whilelevel
	    सचे (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    सचे (elselevel <= 0 && whilelevel <= 0)
		पटिनिवत्त OK;
	}
    }
    पटिनिवत्त FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    पूर्णाङ्क
get_c_indent(शून्य)
{
    pos_T	cur_curpos;
    पूर्णाङ्क		amount;
    पूर्णाङ्क		scope_amount;
    पूर्णाङ्क		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = रिक्त;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    पूर्णाङ्क		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    पूर्णाङ्क		lookfor;
    पूर्णाङ्क		whilelevel;
    linenr_T	lnum;
    पूर्णाङ्क		n;
    पूर्णाङ्क		iscase;
    पूर्णाङ्क		lookfor_break;
    पूर्णाङ्क		lookfor_cpp_namespace = FALSE;
    पूर्णाङ्क		cont_amount = 0;    // amount for continuation line
    पूर्णाङ्क		original_पङ्क्तिः_islabel;
    पूर्णाङ्क		added_to_amount = 0;
    पूर्णाङ्क		js_cur_has_कुञ्चिका = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    पूर्णाङ्क		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    सचे (cur_curpos.lnum == 1)
	पटिनिवत्त 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    सचे (linecopy == रिक्त)
	पटिनिवत्त 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    सचे ((State & MODE_निवेशय)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_पङ्क्तिः_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    सचे (comment_pos != रिक्त)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    सचे (trypos != रिक्त && (comment_pos == रिक्त
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	गच्छ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    सचे (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	सचे (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    गच्छ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    सचे (original_पङ्क्तिः_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	गच्छ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    सचे (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_पङ्क्तिः_comment(); // XXX
	सचे (trypos == रिक्त && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    सचे (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	सचे (trypos  != रिक्त)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, रिक्त, रिक्त, 0);
	    amount = col;
	    गच्छ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    सचे (!cin_iscomment(theline) && comment_pos != रिक्त) // XXX
    {
	पूर्णाङ्क	lead_start_len = 2;
	पूर्णाङ्क	lead_मध्यः_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_मध्यः[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	पूर्णाङ्क	lead_end_len;
	char_u	*p;
	पूर्णाङ्क	start_align = 0;
	पूर्णाङ्क	start_off = 0;
	पूर्णाङ्क	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, रिक्त, रिक्त, 0);
	amount = col;
	*lead_start = NUL;
	*lead_मध्यः = NUL;

	p = curbuf->b_p_com;
	याव (*p != NUL)
	{
	    पूर्णाङ्क	align = 0;
	    पूर्णाङ्क	off = 0;
	    पूर्णाङ्क what = 0;

	    याव (*p != NUL && *p != ':')
	    {
		सचे (*p == COM_START || *p == COM_END || *p == COM_मध्यः)
		    what = *p++;
		अञ्ञथा सचे (*p == COM_वामः || *p == COM_दक्षिणः)
		    align = *p++;
		अञ्ञथा सचे (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		अञ्ञथा
		    ++p;
	    }

	    सचे (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    सचे (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    अञ्ञथा सचे (what == COM_मध्यः)
	    {
		STRCPY(lead_मध्यः, lead_end);
		lead_मध्यः_len = lead_end_len;
	    }
	    अञ्ञथा सचे (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		सचे (STRNCMP(theline, lead_मध्यः, lead_मध्यः_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    सचे (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			सचे (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			अञ्ञथा सचे (STRNCMP(look, lead_मध्यः,
							lead_मध्यः_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    भङ्ग;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			अञ्ञथा सचे (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    अनुवर्त;
		    }
		    सचे (start_off != 0)
			amount += start_off;
		    अञ्ञथा सचे (start_align == COM_दक्षिणः)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_मध्यः);
		    भङ्ग;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		सचे (STRNCMP(theline, lead_मध्यः, lead_मध्यः_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    सचे (off != 0)
			amount += off;
		    अञ्ञथा सचे (align == COM_दक्षिणः)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_मध्यः);
		    done = TRUE;
		    भङ्ग;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	सचे (done)
	    ;
	अञ्ञथा सचे (theline[0] == '*')
	    amount += 1;
	अञ्ञथा
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    अत्थाय (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		सचे (linewhite(lnum))		    // skip blank lines
		    अनुवर्त;
		amount = get_indent_lnum(lnum);	    // XXX
		भङ्ग;
	    }
	    सचे (amount == -1)			    // use the comment opener
	    {
		सचे (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    सचे (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, रिक्त, रिक्त, 0);
		amount = col;
		सचे (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	गच्छ theend;
    }

    // Are we looking at a ']' that has a match?
    सचे (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != रिक्त)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	गच्छ theend;
    }

    // Are we inside parentheses or braces?  XXX
    सचे (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != रिक्त
	    || trypos != रिक्त)
    {
      सचे (trypos != रिक्त && tryposBrace != रिक्त)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  सचे (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = रिक्त;
	  अञ्ञथा
	      tryposBrace = रिक्त;
      }

      सचे (trypos != रिक्त)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	सचे (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	अञ्ञथा
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    अत्थाय (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		सचे (cin_nocode(l))		// skip comment lines
		    अनुवर्त;
		सचे (cin_ispreproc_cont(&l, &lnum, &amount))
		    अनुवर्त;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		सचे ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त)
		{
		    lnum = trypos->lnum + 1;
		    अनुवर्त;
		}

		// XXX
		सचे ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != रिक्त
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    सचे (theline[0] == ')')
		    {
			सचे (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    भङ्ग;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	सचे (amount == -1)
	{
	    पूर्णाङ्क	    ignore_paren_col = 0;
	    पूर्णाङ्क	    is_if_for_while = 0;

	    सचे (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *पङ्क्तिः;

		trypos = &our_paren_pos;
		करोहि
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} याव (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		पङ्क्तिः = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(पङ्क्तिः, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    सचे (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *पङ्क्तिः;
		पूर्णाङ्क	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		पङ्क्तिः = ml_get_curline();
		look_col = (पूर्णाङ्क)(look - पङ्क्तिः);
		curwin->w_cursor.col = look_col + 1;
		सचे ((trypos = findmatchlimit(रिक्त, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != रिक्त
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    सचे (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		सचे (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    सचे (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			अत्थाय (col = 0; col < our_paren_pos.col; ++col)
			{
			    विकल्प (l[col])
			    {
				पक्ष '(':
				पक्ष '{': ++n;
					  भङ्ग;

				पक्ष ')':
				पक्ष '}': सचे (n > 1)
					      --n;
					  भङ्ग;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    अञ्ञथा सचे (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    अञ्ञथा
		    {
			col = our_paren_pos.col + 1;
			याव (VIM_ISWHITE(l[col]))
			    col++;
			सचे (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			अञ्ञथा
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		सचे (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, रिक्त, रिक्त, 0);
		    सचे (cur_amount > (पूर्णाङ्क)col)
			cur_amount = col;
		}
	    }

	    सचे (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    अञ्ञथा सचे ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		सचे (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    अञ्ञथा
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		याव ((पूर्णाङ्क)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    विकल्प (*ml_get_pos(&our_paren_pos))
		    {
			पक्ष '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  भङ्ग;
			पक्ष ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  भङ्ग;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		सचे (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		अञ्ञथा
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    सचे (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != रिक्त)
			amount += curbuf->b_ind_unclosed2;
		    अञ्ञथा
		    {
			सचे (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			अञ्ञथा
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		सचे (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	सचे (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      अञ्ञथा
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
	सचे (*look == '{')
	{
	    getvcol(curwin, trypos, &col, रिक्त, रिक्त, 0);
	    amount = col;
	    सचे (*start == '{')
		start_brace = BRACE_IN_COL0;
	    अञ्ञथा
		start_brace = BRACE_AT_START;
	}
	अञ्ञथा
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    सचे (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != रिक्त)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    सचे ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    अञ्ञथा सचे (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    अञ्ञथा
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	सचे (curbuf->b_ind_js)
	    js_cur_has_कुञ्चिका = cin_has_js_कुञ्चिका(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	सचे (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	अञ्ञथा
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    सचे (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    अञ्ञथा सचे (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    सचे (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		सचे (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    गच्छ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    सचे (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_वामः_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    अञ्ञथा सचे (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    अञ्ञथा
	    {
		सचे (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    सचे (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    अञ्ञथा सचे (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		अञ्ञथा
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    सचे (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    सचे (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    अञ्ञथा सचे (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    अञ्ञथा
	    {
		सचे (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    अत्थाय (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		सचे (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    सचे (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			सचे (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    सचे (cont_amount > 0)
				amount = cont_amount;
			    अञ्ञथा सचे (!curbuf->b_ind_js)
				amount += ind_continuation;
			    भङ्ग;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(रिक्त);
			सचे (trypos != रिक्त)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    अनुवर्त;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			सचे (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    अनुवर्त;

			सचे (cin_nocode(l))
			    अनुवर्त;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			सचे (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    सचे (terminated == ',')
				भङ्ग;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    सचे (terminated != ';' && cin_isinit())
				भङ्ग;

			    // nothing useful found
			    सचे (terminated == 0 || terminated == '{')
				अनुवर्त;
			}

			सचे (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = रिक्त;
			    सचे (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    सचे (trypos == रिक्त && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    सचे (trypos != रिक्त)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				अनुवर्त;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
		    }
		    अञ्ञथा सचे (lookfor == LOOKFOR_UNTERM)
		    {
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
		    }
		    अञ्ञथा
		    {
			सचे (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    सचे (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			सचे (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    सचे (curwin->w_cursor.lnum == ourscope)
				अनुवर्त;

			    सचे (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				भङ्ग;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(रिक्त);
			    सचे (trypos != रिक्त)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				अनुवर्त;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    सचे (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				अनुवर्त;

			    // Finally the actual check for "namespace".
			    सचे (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				भङ्ग;
			    }
			    अञ्ञथा सचे (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				भङ्ग;
			    }

			    सचे (cin_nocode(l))
				अनुवर्त;
			}
		    }
		    भङ्ग;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		सचे ((trypos = ind_find_start_CORS(&raw_string_start)) != रिक्त)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    अनुवर्त;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		सचे (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    सचे (lookfor == LOOKFOR_CPP_BASECLASS)
			भङ्ग;

		    // When looking for a "do" we are not interested in
		    // labels.
		    सचे (whilelevel > 0)
			अनुवर्त;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    सचे (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
			भङ्ग;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    सचे (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			सचे ((trypos = find_start_brace()) == रिक्त
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    भङ्ग;
			}
			अनुवर्त;
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
		    सचे (lookfor == LOOKFOR_TERM)
		    {
			सचे (n)
			    amount = n;

			सचे (!lookfor_break)
			    भङ्ग;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    सचे (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			सचे (l != रिक्त && cin_is_cinword(l))
			{
			    सचे (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    अञ्ञथा
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			भङ्ग;
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
		    अनुवर्त;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		सचे (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    सचे (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != रिक्त)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    अनुवर्त;
		}

		// Ignore jump labels with nothing after them.
		सचे (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    सचे (l == रिक्त || cin_nocode(l))
			अनुवर्त;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		सचे (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    अनुवर्त;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		सचे (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		सचे (n)
		{
		    सचे (lookfor == LOOKFOR_UNTERM)
		    {
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
		    }
		    अञ्ञथा सचे (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			अनुवर्त;
		    }
		    अञ्ञथा
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    भङ्ग;
		}
		अञ्ञथा सचे (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    सचे (cin_isterminated(l, TRUE, FALSE))
			भङ्ग;
		    अञ्ञथा
			अनुवर्त;
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

		सचे (js_cur_has_कुञ्चिका)
		{
		    js_cur_has_कुञ्चिका = 0; // only check the first line
		    सचे (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_कुञ्चिका;
		    }
		}
		सचे (lookfor == LOOKFOR_JS_कुञ्चिका && cin_has_js_कुञ्चिका(l))
		{
		    amount = get_indent();
		    भङ्ग;
		}
		सचे (lookfor == LOOKFOR_COMMA)
		{
		    सचे (tryposBrace != रिक्त && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			भङ्ग;
		    सचे (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			भङ्ग;
		    अञ्ञथा
		    {
			amount = get_indent();
			सचे (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    भङ्ग;
		    }
		}

		सचे (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    सचे (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (शून्य)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    सचे (trypos != रिक्त && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = रिक्त;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    सचे (trypos == रिक्त && terminated == ',')
		    {
			सचे (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    सचे (trypos != रिक्त)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			सचे (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    अनुवर्त;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    सचे (terminated == ',')
		    {
			याव (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    सचे (*l == NUL || l[STRLEN(l) - 1] != '\\')
				भङ्ग;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    सचे (curbuf->b_ind_js)
			cur_amount = get_indent();
		    अञ्ञथा
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    सचे (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			सचे (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			सचे (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    अनुवर्त;
			}
			भङ्ग;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    सचे (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			सचे (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    सचे (cont_amount > 0)
				amount = cont_amount;
			    अञ्ञथा
				amount += ind_continuation;
			    भङ्ग;
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
			सचे (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			सचे (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    भङ्ग;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			सचे (cin_isdo(l))
			{
			    सचे (whilelevel == 0)
				भङ्ग;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			सचे (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    सचे (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    सचे ((trypos = find_start_brace()) == रिक्त
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				भङ्ग;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    अञ्ञथा
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			सचे (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    सचे (terminated == ',')
				amount += ind_continuation;
			    भङ्ग;
			}

			सचे (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    सचे (terminated == ',')
			    {
				सचे (curbuf->b_ind_cpp_baseclass == 0)
				    भङ्ग;

				lookfor = LOOKFOR_CPP_BASECLASS;
				अनुवर्त;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    सचे (amount > cur_amount)
				amount = cur_amount;
			}
			अञ्ञथा
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (पूर्णाङ्क)STRLEN(l);
			    सचे (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				भङ्ग;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    सचे (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				सचे (curbuf->b_ind_js)
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
				    सचे (cin_iscomment(skipwhite(l)))
					भङ्ग;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    सचे (trypos != रिक्त)
				    {
					सचे (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    भङ्ग;
					}
					ourscope = trypos->lnum;
				    }
				}
				अञ्ञथा
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    अञ्ञथा
			    {
				सचे (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				सचे (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_कुञ्चिका
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		अञ्ञथा सचे (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    सचे (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
			भङ्ग;
		    }

		    सचे (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			सचे (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		अञ्ञथा
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    सचे (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			अनुवर्त;
		    }

		    // Handle "do {" line.
		    सचे (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			सचे (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    अनुवर्त;
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
		    सचे (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			सचे (cont_amount > 0)
			    amount = cont_amount;
			अञ्ञथा
			    amount += ind_continuation;
			भङ्ग;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    सचे (lookfor == LOOKFOR_TERM)
		    {
			सचे (!lookfor_break && whilelevel == 0)
			    भङ्ग;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    अञ्ञथा
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			सचे (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != रिक्त)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    सचे (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				अनुवर्त;
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

			सचे (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			सचे (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			सचे (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    सचे ((trypos = find_start_brace()) == रिक्त
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				भङ्ग;
			    अनुवर्त;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			सचे (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != रिक्त)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    सचे (*l == '}' || !cin_iselse(l))
				गच्छ term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      सचे (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      सचे (curbuf->b_ind_jump_label > 0 && original_पङ्क्तिः_islabel)
	  amount -= curbuf->b_ind_jump_label;

      गच्छ theend;
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

    सचे (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	गच्छ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    सचे (cur_curpos.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == रिक्त
	    && vim_strchr(theline, '}') == रिक्त
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(रिक्त, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	गच्छ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    याव (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	सचे ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    अनुवर्त;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	सचे (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	सचे (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    भङ्ग;
	}

	// Skip preprocessor directives and blank lines.
	सचे (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    अनुवर्त;

	सचे (cin_nocode(l))
	    अनुवर्त;

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
	सचे (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    सचे (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != रिक्त)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    याव (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		सचे (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    भङ्ग;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    सचे (amount == 0)
		amount = cin_first_id_amount();
	    सचे (amount == 0)
		amount = ind_continuation;
	    भङ्ग;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	सचे (cin_isfuncdecl(रिक्त, cur_curpos.lnum, 0))  // XXX
	    भङ्ग;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	सचे (*skipwhite(l) == '}')
	    भङ्ग;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	सचे (cin_ends_in(l, (char_u *)"};"))
	    भङ्ग;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	सचे (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    भङ्ग;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	सचे (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    याव (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		सचे (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    भङ्ग;
	    }
	    सचे (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		भङ्ग;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	सचे (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    भङ्ग;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	सचे (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    सचे (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		भङ्ग;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	सचे ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	भङ्ग;
    }

    // add extra indent for a comment
    सचे (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    सचे (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	सचे (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    सचे (cur_amount > 0)
		amount = cur_amount;
	    अञ्ञथा सचे (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    सचे (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    पटिनिवत्त amount;
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
    पूर्णाङ्क
in_cinkeys(
    पूर्णाङ्क		keytyped,
    पूर्णाङ्क		when,
    पूर्णाङ्क		पङ्क्तिः_is_empty)
{
    char_u	*look;
    पूर्णाङ्क		try_match;
    पूर्णाङ्क		try_match_word;
    char_u	*p;
    char_u	*पङ्क्तिः;
    पूर्णाङ्क		icase;
    पूर्णाङ्क		i;

    सचे (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	पटिनिवत्त FALSE;

#ifdef FEAT_EVAL
    सचे (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    अञ्ञथा
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    याव (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	विकल्प (when)
	{
	    पक्ष '*': try_match = (*look == '*'); भङ्ग;
	    पक्ष '!': try_match = (*look == '!'); भङ्ग;
	    स्वभाव: try_match = (*look != '*'); भङ्ग;
	}
	सचे (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	सचे (*look == '0')
	{
	    try_match_word = try_match;
	    सचे (!पङ्क्तिः_is_empty)
		try_match = FALSE;
	    ++look;
	}
	अञ्ञथा
	    try_match_word = FALSE;

	// does it look like a control character?
	सचे (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    सचे (try_match && keytyped == Ctrl_chr(look[1]))
		पटिनिवत्त TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	अञ्ञथा सचे (*look == 'o')
	{
	    सचे (try_match && keytyped == कुञ्चिका_OPEN_FORW)
		पटिनिवत्त TRUE;
	    ++look;
	}
	अञ्ञथा सचे (*look == 'O')
	{
	    सचे (try_match && keytyped == कुञ्चिका_OPEN_BACK)
		पटिनिवत्त TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	अञ्ञथा सचे (*look == 'e')
	{
	    सचे (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		सचे (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    पटिनिवत्त TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	अञ्ञथा सचे (*look == ':')
	{
	    सचे (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		सचे (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    पटिनिवत्त TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		सचे (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    सचे (i)
			पटिनिवत्त TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	अञ्ञथा सचे (*look == '<')
	{
	    सचे (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		सचे (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != रिक्त
						       && keytyped == look[1])
		    पटिनिवत्त TRUE;

		सचे (keytyped == get_special_कुञ्चिका_code(look + 1))
		    पटिनिवत्त TRUE;
	    }
	    याव (*look && *look != '>')
		look++;
	    याव (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	अञ्ञथा सचे (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    सचे (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    अञ्ञथा
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    सचे (p == रिक्त)
		p = look + STRLEN(look);
	    सचे ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		पूर्णाङ्क		match = FALSE;

		सचे (keytyped == कुञ्चिका_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    पङ्क्तिः = ml_get_curline();
		    सचे (has_mbyte)
		    {
			char_u	*n;

			अत्थाय (s = पङ्क्तिः + curwin->w_cursor.col; s > पङ्क्तिः; s = n)
			{
			    n = mb_prevptr(पङ्क्तिः, s);
			    सचे (!vim_iswordp(n))
				भङ्ग;
			}
		    }
		    अञ्ञथा
			अत्थाय (s = पङ्क्तिः + curwin->w_cursor.col; s > पङ्क्तिः; --s)
			    सचे (!vim_iswordc(s[-1]))
				भङ्ग;
		    सचे (s + (p - look) <= पङ्क्तिः + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		अञ्ञथा
		    // TODO: multi-byte
		    सचे (keytyped == (पूर्णाङ्क)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((पूर्णाङ्क)p[-1])))
		{
		    पङ्क्तिः = ml_get_cursor();
		    सचे ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(पङ्क्तिः[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(पङ्क्तिः - (p - look), look, p - look)
				: STRNCMP(पङ्क्तिः - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		सचे (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    सचे (getwhitecols_curline() !=
				     (पूर्णाङ्क)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		सचे (match)
		    पटिनिवत्त TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	अञ्ञथा
	{
	    सचे (try_match && *look == keytyped)
		पटिनिवत्त TRUE;
	    सचे (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    पटिनिवत्त FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    शून्य
do_c_व्यञ्जकम्_indent(शून्य)
{
#ifdef FEAT_EVAL
    सचे (*curbuf->b_p_inde != NUL)
	fixthisline(get_व्यञ्जकम्_indent);
    अञ्ञथा
#endif
	fixthisline(get_c_indent);
}

#सचे defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    शून्य
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    सचे (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    सचे (lnum >= 1 && lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_सङ्ख्या = get_c_indent();
	curwin->w_cursor = pos;
    }
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = -1;
}
#endif
