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
#define LOOKFOR_JS_किल्ली		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    पूर्णांक
cin_is_cinword(char_u *ओळ)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    पूर्णांक		cinw_len;
    पूर्णांक		retval = FALSE;
    पूर्णांक		len;

    cinw_len = (पूर्णांक)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    जर (cinw_buf == रिक्त)
	परत FALSE;

    ओळ = skipwhite(ओळ);
    साठी (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	जर (STRNCMP(ओळ, cinw_buf, len) == 0
		&& (!vim_iswordc(ओळ[len]) || !vim_iswordc(ओळ[len - 1])))
	{
	    retval = TRUE;
	    थांबा;
	}
    }
    vim_free(cinw_buf);
    परत retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    स्थिर char_u *
skip_string(char_u *p)
{
    पूर्णांक	    i;

    // We loop, because strings may be concatenated: "date""time".
    साठी ( ; ; ++p)
    {
	जर (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    जर (p[1] == NUL)		    // ' at end of line
		थांबा;
	    i = 2;
	    जर (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		पर्यंत (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    जर (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		सुरूठेवा;
	    }
	}
	अन्यथा जर (p[0] == '"')		    // start of string
	{
	    साठी (++p; p[0]; ++p)
	    {
		जर (p[0] == '\\' && p[1] != NUL)
		    ++p;
		अन्यथा जर (p[0] == '"')	    // end of string
		    थांबा;
	    }
	    जर (p[0] == '"')
		सुरूठेवा; // continue for another string
	}
	अन्यथा जर (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    जर (paren != रिक्त)
	    {
		माप_प्रकार delim_len = paren - delim;

		साठी (p += 3; *p; ++p)
		    जर (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			थांबा;
		    }
		जर (p[0] == '"')
		    सुरूठेवा; // continue for another string
	    }
	}
	थांबा;				    // no string found
    }
    जर (!*p)
	--p;				    // backup from NUL
    परत p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    पूर्णांक
is_pos_in_string(char_u *ओळ, colnr_T col)
{
    char_u *p;

    साठी (p = ओळ; *p && (colnr_T)(p - ओळ) < col; ++p)
	p = skip_string(p);
    परत !((colnr_T)(p - ओळ) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    पूर्णांक
check_linecomment(char_u *ओळ)
{
    char_u  *p;

    p = ओळ;
    // skip Lispish one-line comments
    जर (curbuf->b_p_lisp)
    {
	जर (vim_strchr(p, ';') != रिक्त) // there may be comments
	{
	    पूर्णांक in_str = FALSE;	// inside of string

	    p = ओळ;		// scan from start
	    पर्यंत ((p = vim_strpbrk(p, (char_u *)"\";")) != रिक्त)
	    {
		जर (*p == '"')
		{
		    जर (in_str)
		    {
			जर (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    अन्यथा जर (p == ओळ || ((p - ओळ) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		अन्यथा जर (!in_str && ((p - ओळ) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(ओळ, (colnr_T)(p - ओळ)))
		    थांबा;	// found!
		++p;
	    }
	}
	अन्यथा
	    p = रिक्त;
    }
    अन्यथा
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	साठी ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    जर (*p == NUL)
		थांबा;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    जर (p[0] == '/' && p[1] == '/'
				 && (p == ओळ || p[-1] != '*' || p[2] != '*'))
		थांबा;
	}
	जर (*p == NUL)
	    p = रिक्त;
    }

    जर (p == रिक्त)
	परत MAXCOL;
    परत (पूर्णांक)(p - ओळ);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    स्थिर pos_T *
ind_find_start_comment(शून्य)	    // XXX
{
    परत find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(पूर्णांक ind_maxcomment)	// XXX
{
    pos_T	*pos;
    पूर्णांक		cur_maxcomment = ind_maxcomment;

    साठी (;;)
    {
	pos = findmatchlimit(रिक्त, '*', FM_BACKWARD, cur_maxcomment);
	जर (pos == रिक्त)
	    थांबा;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	जर (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    थांबा;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	जर (cur_maxcomment <= 0)
	{
	    pos = रिक्त;
	    थांबा;
	}
    }
    परत pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    स्थिर pos_T *
find_start_rawstring(पूर्णांक ind_maxcomment)	// XXX
{
    pos_T	*pos;
    पूर्णांक		cur_maxcomment = ind_maxcomment;

    साठी (;;)
    {
	pos = findmatchlimit(रिक्त, 'R', FM_BACKWARD, cur_maxcomment);
	जर (pos == रिक्त)
	    थांबा;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	जर (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    थांबा;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	जर (cur_maxcomment <= 0)
	{
	    pos = रिक्त;
	    थांबा;
	}
    }
    परत pos;
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
    जर (comment_pos != रिक्त)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    जर (comment_pos == रिक्त || (rs_pos != रिक्त
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	जर (is_raw != रिक्त && rs_pos != रिक्त)
	    *is_raw = rs_pos->lnum;
	परत rs_pos;
    }
    परत comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    पूर्णांक
cindent_on(शून्य)
{
    परत (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
प्रकारव्याख्या संरचना {
    पूर्णांक	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    स्थिर char_u *
cin_skipcomment(char_u *s)
{
    पर्यंत (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	जर (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    थांबा;
	}
	जर (*s != '/')
	    थांबा;
	++s;
	जर (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    थांबा;
	}
	जर (*s != '*')
	    थांबा;
	साठी (++s; *s; ++s)	// skip slash-star comment
	    जर (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		थांबा;
	    }
    }
    परत s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    स्थिर पूर्णांक
cin_nocode(char_u *s)
{
    परत *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    स्थिर पूर्णांक
cin_iscomment(char_u *p)
{
    परत (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    स्थिर पूर्णांक
cin_islinecomment(char_u *p)
{
    परत (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    स्थिर pos_T *
find_ओळ_comment(शून्य) // XXX
{
    स्थिर pos_T pos;
    char_u	 *ओळ;
    char_u	 *p;

    pos = curwin->w_cursor;
    पर्यंत (--pos.lnum > 0)
    {
	ओळ = ml_get(pos.lnum);
	p = skipwhite(ओळ);
	जर (cin_islinecomment(p))
	{
	    pos.col = (पूर्णांक)(p - ओळ);
	    परत &pos;
	}
	जर (*p != NUL)
	    थांबा;
    }
    परत रिक्त;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    स्थिर पूर्णांक
cin_has_js_किल्ली(char_u *text)
{
    char_u *s = skipwhite(text);
    पूर्णांक	    quote = -1;

    जर (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    जर (!vim_isIDc(*s))	    // need at least one ID character
	परत FALSE;

    पर्यंत (vim_isIDc(*s))
	++s;
    जर (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    परत (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    स्थिर पूर्णांक
cin_islabel_skip(char_u **s)
{
    जर (!vim_isIDc(**s))	    // need at least one ID character
	परत FALSE;

    पर्यंत (vim_isIDc(**s))
    {
	जर (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	अन्यथा
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    परत (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    स्थिर पूर्णांक
cin_isscopedecl(char_u *p)
{
    माप_प्रकार  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    माप_प्रकार  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    पूर्णांक	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    जर (cinsd_buf == रिक्त)
	परत FALSE;

    साठी (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (पूर्णांक)cinsd_len, ",");
	जर (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    जर (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		थांबा;
	    }
	}
    }

    vim_free(cinsd_buf);
    परत found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    स्थिर पूर्णांक
cin_ispreproc(char_u *s)
{
    जर (*skipwhite(s) == '#')
	परत TRUE;
    परत FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    स्थिर पूर्णांक
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, पूर्णांक *amount)
{
    char_u	*ओळ = *pp;
    linenr_T	lnum = *lnump;
    पूर्णांक		retval = FALSE;
    पूर्णांक		candidate_amount = *amount;

    जर (*ओळ != NUL && ओळ[STRLEN(ओळ) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    साठी (;;)
    {
	जर (cin_ispreproc(ओळ))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    थांबा;
	}
	जर (lnum == 1)
	    थांबा;
	ओळ = ml_get(--lnum);
	जर (*ओळ == NUL || ओळ[STRLEN(ओळ) - 1] != '\\')
	    थांबा;
    }

    जर (lnum != *lnump)
	*pp = ml_get(*lnump);
    जर (retval)
	*amount = candidate_amount;
    परत retval;
}

    स्थिर पूर्णांक
cin_iselse(
    char_u  *p)
{
    जर (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    परत (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    स्थिर पूर्णांक
cin_isterminated(
    char_u	*s,
    पूर्णांक		incl_open,	// include '{' at the end as terminator
    पूर्णांक		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    अचिन्हित	n_open = 0;
    पूर्णांक		is_else = FALSE;

    s = cin_skipcomment(s);

    जर (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    जर (!found_start)
	is_else = cin_iselse(s);

    पर्यंत (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	जर (*s == '}' && n_open > 0)
	    --n_open;
	जर ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    परत *s;
	अन्यथा जर (*s == '{')
	{
	    जर (incl_open && cin_nocode(s + 1))
		परत *s;
	    अन्यथा
		++n_open;
	}

	जर (*s)
	    s++;
    }
    परत found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    स्थिर पूर्णांक
cin_starts_with(char_u *s, अक्षर *word)
{
    पूर्णांक l = (पूर्णांक)STRLEN(word);

    परत (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    स्थिर पूर्णांक
cin_isdefault(char_u *s)
{
    परत (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    स्थिर पूर्णांक
cin_iscase(
    char_u *s,
    पूर्णांक strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    जर (cin_starts_with(s, "case"))
    {
	साठी (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    जर (*s == NUL)
		थांबा;
	    जर (*s == ':')
	    {
		जर (s[1] == ':')	// skip over "::" for C++
		    ++s;
		अन्यथा
		    परत TRUE;
	    }
	    जर (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    अन्यथा जर (*s == '/' && (s[1] == '*' || s[1] == '/'))
		परत FALSE;		// stop at comment
	    अन्यथा जर (*s == '"')
	    {
		// JS etc.
		जर (strict)
		    परत FALSE;		// stop at string
		अन्यथा
		    परत TRUE;
	    }
	}
	परत FALSE;
    }

    जर (cin_isdefault(s))
	परत TRUE;
    परत FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    स्थिर पूर्णांक
cin_islabel(शून्य)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    जर (cin_isdefault(s))
	परत FALSE;
    जर (cin_isscopedecl(s))
	परत FALSE;

    जर (!cin_islabel_skip(&s))
	परत FALSE;

    जर (ind_find_start_CORS(रिक्त))
	परत FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*ओळ;

    cursor_save = curwin->w_cursor;
    पर्यंत (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	जर ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त) // XXX
	    curwin->w_cursor = *trypos;

	ओळ = ml_get_curline();
	जर (cin_ispreproc(ओळ))	// ignore #defines, #if, etc.
	    सुरूठेवा;
	जर (*(ओळ = cin_skipcomment(ओळ)) == NUL)
	    सुरूठेवा;

	curwin->w_cursor = cursor_save;
	जर (cin_isterminated(ओळ, TRUE, FALSE)
		|| cin_isscopedecl(ओळ)
		|| cin_iscase(ओळ, TRUE)
		|| (cin_islabel_skip(&ओळ) && cin_nocode(ओळ)))
	    परत TRUE;
	परत FALSE;
    }
    curwin->w_cursor = cursor_save;
    परत TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    स्थिर पूर्णांक
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    पूर्णांक		len = (पूर्णांक)STRLEN(find);

    पर्यंत (*p != NUL)
    {
	p = cin_skipcomment(p);
	जर (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    जर (cin_nocode(r))
		परत TRUE;
	}
	जर (*p != NUL)
	    ++p;
    }
    परत FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    स्थिर char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = रिक्त, *p = s;
    करा
    {
	r = p;
	p = cin_skipcomment(p);
	जर (*p)
	    p = skip_string(p);
    } पर्यंत (p != r);
    परत p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    स्थिर पूर्णांक
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = रिक्त;

    पर्यंत (*p)
    {
	जर (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	अन्यथा जर (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	अन्यथा
	    p = cin_skip_comment_and_string(p + 1);
    }
    जर (!r)
	परत FALSE;
    p = r; // p points now after '=' or "return"

    जर (cin_nocode(p))
	परत TRUE;

    जर (*p == '&')
	p = cin_skipcomment(p + 1);

    जर (*p == '(') // skip a typecast
    {
	पूर्णांक open_मोजणी = 1;
	करा
	{
	    p = cin_skip_comment_and_string(p + 1);
	    जर (cin_nocode(p))
		परत TRUE;
	    open_मोजणी += (*p == '(') - (*p == ')');
	} पर्यंत (open_मोजणी);
	p = cin_skipcomment(p + 1);
	जर (cin_nocode(p))
	    परत TRUE;
    }

    पर्यंत (*p == '{')
	p = cin_skipcomment(p + 1);
    परत cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    स्थिर पूर्णांक
cin_isinit(शून्य)
{
    char_u	*s;
    स्थिर अक्षर *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    जर (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    साठी (;;)
    {
	पूर्णांक i, l;

	साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(skip); ++i)
	{
	    l = (पूर्णांक)म_माप(skip[i]);
	    जर (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		थांबा;
	    }
	}
	जर (l != 0)
	    थांबा;
    }

    जर (cin_starts_with(s, "enum"))
	परत TRUE;

    परत cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    स्थिर पूर्णांक
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    पूर्णांक		has_name = FALSE;
    पूर्णांक		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    पर्यंत ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    जर (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	पर्यंत (*p != NUL)
	{
	    जर (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    अन्यथा जर (*p == '{')
	    {
		थांबा;
	    }
	    अन्यथा जर (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		जर (has_name)
		    परत FALSE; // word character after skipping past name
		++p;
	    }
	    अन्यथा जर (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		जर (!has_name_start || has_name)
		    परत FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    अन्यथा
	    {
		परत FALSE;
	    }
	}
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    स्थिर पूर्णांक
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    पूर्णांक		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    जर (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	पर्यंत (*p != NUL)
	{
	    जर (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    अन्यथा जर (*p == '{')
	    {
		थांबा;
	    }
	    अन्यथा जर (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		जर (has_string_literal)
		    परत FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    अन्यथा जर (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		जर (has_string_literal)
		    परत FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    अन्यथा
	    {
		परत FALSE;
	    }
	}
	परत has_string_literal ? TRUE : FALSE;
    }
    परत FALSE;
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
    साठी ( ; *l; ++l)
    {
	जर (*l == ':')
	{
	    जर (l[1] == ':')	    // skip over "::" for C++
		++l;
	    अन्यथा जर (!cin_iscase(l + 1, FALSE))
		थांबा;
	}
	अन्यथा जर (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    जर (*l == NUL)
	परत रिक्त;
    l = cin_skipcomment(l + 1);
    जर (*l == NUL)
	परत रिक्त;
    परत l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    स्थिर पूर्णांक
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    जर (p == रिक्त)
	परत 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    परत (पूर्णांक)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    स्थिर पूर्णांक
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    पूर्णांक		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    जर (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	जर (l == रिक्त)		// just in case
	    l = ml_get_curline();
    }
    अन्यथा
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    परत amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    स्थिर पूर्णांक
cin_first_id_amount(शून्य)
{
    char_u	*ओळ, *p, *s;
    पूर्णांक		len;
    pos_T	fp;
    colnr_T	col;

    ओळ = ml_get_curline();
    p = skipwhite(ओळ);
    len = (पूर्णांक)(skiptowhite(p) - p);
    जर (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (पूर्णांक)(skiptowhite(p) - p);
    }
    जर (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    अन्यथा जर (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    अन्यथा जर ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	जर ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    साठी (len = 0; vim_isIDc(p[len]); ++len)
	;
    जर (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	परत 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - ओळ);
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    परत (पूर्णांक)col;
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
    स्थिर पूर्णांक
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*ओळ;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    जर (lnum > 1)
    {
	ओळ = ml_get(lnum - 1);
	जर (*ओळ != NUL && ओळ[STRLEN(ओळ) - 1] == '\\')
	    परत -1;
    }

    ओळ = s = ml_get(lnum);
    पर्यंत (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == रिक्त)
    {
	जर (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अन्यथा
	    ++s;
    }
    जर (*s != '=')
	परत 0;

    s = skipwhite(s + 1);
    जर (cin_nocode(s))
	परत 0;

    जर (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - ओळ);
    getvcol(curwin, &fp, &col, रिक्त, रिक्त, 0);
    परत (पूर्णांक)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    स्थिर पूर्णांक
cin_skip2pos(pos_T *trypos)
{
    char_u	*ओळ;
    char_u	*p;
    char_u	*new_p;

    p = ओळ = ml_get(trypos->lnum);
    पर्यंत (*p && (colnr_T)(p - ओळ) < trypos->col)
    {
	जर (cin_iscomment(p))
	    p = cin_skipcomment(p);
	अन्यथा
	{
	    new_p = skip_string(p);
	    जर (new_p == p)
		++p;
	    अन्यथा
		p = new_p;
	}
    }
    परत (पूर्णांक)(p - ओळ);
}

    स्थिर pos_T *
find_match_char(पूर्णांक c, पूर्णांक ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    स्थिर pos_T pos_copy;
    पूर्णांक		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    जर ((trypos = findmatchlimit(रिक्त, c, FM_SKIPCOMM, ind_maxp_wk)) != रिक्त)
    {
	// check if the ( is in a // comment
	जर ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (पूर्णांक)(cursor_save.lnum - trypos->lnum);
	    जर (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		जा retry;
	    }
	    trypos = रिक्त;
	}
	अन्यथा
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    जर ((trypos_wk = ind_find_start_CORS(रिक्त)) != रिक्त) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (पूर्णांक)(cursor_save.lnum
			- trypos_wk->lnum);
		जर (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    जा retry;
		}
		trypos = रिक्त;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    परत trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    स्थिर pos_T *
find_match_paren(पूर्णांक ind_maxparen)	// XXX
{
    परत find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    स्थिर पूर्णांक
find_last_paren(char_u *l, पूर्णांक start, पूर्णांक end)
{
    पूर्णांक		i;
    पूर्णांक		retval = FALSE;
    पूर्णांक		open_मोजणी = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    साठी (i = 0; l[i] != NUL; i++)
    {
	i = (पूर्णांक)(cin_skipcomment(l + i) - l); // ignore parens in comments
	जर (l[i] == NUL)
	    थांबा;
	i = (पूर्णांक)(skip_string(l + i) - l);    // ignore parens in quotes
	जर (l[i] == start)
	    ++open_मोजणी;
	अन्यथा जर (l[i] == end)
	{
	    जर (open_मोजणी > 0)
		--open_मोजणी;
	    अन्यथा
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    परत retval;
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
    स्थिर पूर्णांक
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    पूर्णांक		retval = FALSE;
    pos_T	*trypos;
    पूर्णांक		just_started = TRUE;

    जर (sp == रिक्त)
	s = ml_get(lnum);
    अन्यथा
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    जर (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
    {
	lnum = trypos->lnum;
	जर (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    परत FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    जर (cin_ispreproc(s))
	परत FALSE;

    पर्यंत (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	जर (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अन्यथा जर (*s == ':')
	{
	    जर (*(s + 1) == ':')
		s += 2;
	    अन्यथा
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		परत FALSE;
	}
	अन्यथा
	    ++s;
    }
    जर (*s != '(')
	परत FALSE;		// ';', ' or "  before any () or no '('

    पर्यंत (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	जर (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    जर (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    जा done;
	}
	जर ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    पूर्णांक comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    साठी (;;)
	    {
		जर (lnum >= curbuf->b_ml.ml_ओळ_मोजणी)
		    थांबा;
		s = ml_get(++lnum);
		जर (!cin_ispreproc(s))
		    थांबा;
	    }
	    जर (lnum >= curbuf->b_ml.ml_ओळ_मोजणी)
		थांबा;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    जर (!just_started && (!comma && *s != ',' && *s != ')'))
		थांबा;
	    just_started = FALSE;
	}
	अन्यथा जर (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	अन्यथा
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    जर (lnum != first_lnum && sp != रिक्त)
	*sp = ml_get(first_lnum);

    परत retval;
}

    स्थिर पूर्णांक
cin_isif(char_u *p)
{
    परत (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    स्थिर पूर्णांक
cin_isdo(char_u *p)
{
    परत (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    स्थिर पूर्णांक
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    पूर्णांक		retval = FALSE;

    p = cin_skipcomment(p);
    जर (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    जर (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	पर्यंत (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	जर ((trypos = findmatchlimit(रिक्त, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != रिक्त
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    परत retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    स्थिर पूर्णांक
cin_is_if_for_while_before_offset(char_u *ओळ, पूर्णांक *poffset)
{
    पूर्णांक offset = *poffset;

    जर (offset-- < 2)
	परत 0;
    पर्यंत (offset > 2 && VIM_ISWHITE(ओळ[offset]))
	--offset;

    offset -= 1;
    जर (STRNCMP(ओळ + offset, "if", 2) == 0)
	जा probablyFound;

    जर (offset >= 1)
    {
	offset -= 1;
	जर (STRNCMP(ओळ + offset, "for", 3) == 0)
	    जा probablyFound;

	जर (offset >= 2)
	{
	    offset -= 2;
	    जर (STRNCMP(ओळ + offset, "while", 5) == 0)
		जा probablyFound;
	}
    }
    परत 0;

probablyFound:
    जर (!offset || !vim_isIDc(ओळ[offset - 1]))
    {
	*poffset = offset;
	परत 1;
    }
    परत 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    स्थिर पूर्णांक
cin_iswhileofdo_end(पूर्णांक terminated)
{
    char_u	*ओळ;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    पूर्णांक		i;

    जर (terminated != ';')	// there must be a ';' at the end
	परत FALSE;

    p = ओळ = ml_get_curline();
    पर्यंत (*p != NUL)
    {
	p = cin_skipcomment(p);
	जर (*p == ')')
	{
	    s = skipwhite(p + 1);
	    जर (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (पूर्णांक)(p - ओळ);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		जर (trypos != रिक्त)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    जर (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    जर (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			परत TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		ओळ = ml_get_curline();
		p = ओळ + i;
	    }
	}
	जर (*p != NUL)
	    ++p;
    }
    परत FALSE;
}

    स्थिर पूर्णांक
cin_isbreak(char_u *p)
{
    परत (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    स्थिर पूर्णांक
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    पूर्णांक		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*ओळ = ml_get_curline();

    जर (pos->lnum <= lnum)
	परत cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(ओळ);
    जर (*s == '#')		// skip #define FOO x ? (x) : x
	परत FALSE;
    s = cin_skipcomment(s);
    जर (*s == NUL)
	परत FALSE;

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
    पर्यंत (lnum > 1)
    {
	ओळ = ml_get(lnum - 1);
	s = skipwhite(ओळ);
	जर (*s == '#' || *s == NUL)
	    थांबा;
	पर्यंत (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    जर (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		थांबा;
	    जर (*s != NUL)
		++s;
	}
	जर (*s != NUL)
	    थांबा;
	--lnum;
    }

    pos->lnum = lnum;
    ओळ = ml_get(lnum);
    s = ओळ;
    साठी (;;)
    {
	जर (*s == NUL)
	{
	    जर (lnum == curwin->w_cursor.lnum)
		थांबा;
	    // Continue in the cursor line.
	    ओळ = ml_get(++lnum);
	    s = ओळ;
	}
	जर (s == ओळ)
	{
	    // don't recognize "case (foo):" as a baseclass
	    जर (cin_iscase(s, FALSE))
		थांबा;
	    s = cin_skipcomment(ओळ);
	    जर (*s == NUL)
		सुरूठेवा;
	}

	जर (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	अन्यथा जर (s[0] == ':')
	{
	    जर (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    अन्यथा जर (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    अन्यथा
		s = cin_skipcomment(s + 1);
	}
	अन्यथा जर ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    जर (*s == 'c')
		s = cin_skipcomment(s + 5);
	    अन्यथा
		s = cin_skipcomment(s + 6);
	}
	अन्यथा
	{
	    जर (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    अन्यथा जर (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    अन्यथा जर (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		परत FALSE;
	    }
	    अन्यथा जर (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    अन्यथा जर (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		जर (cpp_base_class)
		    pos->col = (colnr_T)(s - ओळ);
	    }

	    // When the line ends in a comma don't align with it.
	    जर (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    जर (cpp_base_class)
	pos->lnum = lnum;
    परत cpp_base_class;
}

    स्थिर पूर्णांक
get_baseclass_amount(पूर्णांक col)
{
    पूर्णांक		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    जर (col == 0)
    {
	amount = get_indent();
	जर (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	जर (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    अन्यथा
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, रिक्त, रिक्त, 0);
	amount = (पूर्णांक)vcol;
    }
    जर (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    परत amount;
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
    पर्यंत ((trypos = findmatchlimit(रिक्त, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != रिक्त)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = रिक्त;
	// ignore the { if it's in a // or / *  * / comment
	जर ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(रिक्त)) == रिक्त) // XXX
	    थांबा;
	जर (pos != रिक्त)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    परत trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    स्थिर pos_T *
find_match_paren_after_brace (पूर्णांक ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    जर (trypos == रिक्त)
	परत रिक्त;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    जर (tryposBrace != रिक्त
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = रिक्त;
    परत trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    स्थिर पूर्णांक
corr_ind_maxparen(pos_T *startpos)
{
    दीर्घ	n = (दीर्घ)startpos->lnum - (दीर्घ)curwin->w_cursor.lnum;

    जर (n > 0 && n < curbuf->b_ind_maxparen / 2)
	परत curbuf->b_ind_maxparen - (पूर्णांक)n;
    परत curbuf->b_ind_maxparen;
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
    पूर्णांक		divider;
    पूर्णांक		fraction = 0;
    पूर्णांक		sw;
    दीर्घ	t = get_sw_मूल्य(buf);

    // needed for cino-(, it will be multiplied by 2 again
    जर (t > पूर्णांक_उच्च / 2)
	sw = पूर्णांक_उच्च / 2;
    अन्यथा
	sw = (पूर्णांक)t;

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
    buf->b_ind_open_डावा_imag = 0;

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

    साठी (p = buf->b_p_cino; *p; )
    {
	l = p++;
	जर (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	जर (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = म_से_द((अक्षर *)++p);
	    पर्यंत (VIM_ISDIGIT(*p))
	    {
		++p;
		जर (divider)
		    divider *= 10;
		अन्यथा
		    divider = 10;
	    }
	}
	जर (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    जर (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    अन्यथा
	    {
		n *= sw;
		जर (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	जर (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	निवड (*l)
	{
	    स्थिती '>': buf->b_ind_level = (पूर्णांक)n; थांबा;
	    स्थिती 'e': buf->b_ind_open_imag = (पूर्णांक)n; थांबा;
	    स्थिती 'n': buf->b_ind_no_brace = (पूर्णांक)n; थांबा;
	    स्थिती 'f': buf->b_ind_first_open = (पूर्णांक)n; थांबा;
	    स्थिती '{': buf->b_ind_open_extra = (पूर्णांक)n; थांबा;
	    स्थिती '}': buf->b_ind_close_extra = (पूर्णांक)n; थांबा;
	    स्थिती '^': buf->b_ind_open_डावा_imag = (पूर्णांक)n; थांबा;
	    स्थिती 'L': buf->b_ind_jump_label = (पूर्णांक)n; थांबा;
	    स्थिती ':': buf->b_ind_case = (पूर्णांक)n; थांबा;
	    स्थिती '=': buf->b_ind_case_code = (पूर्णांक)n; थांबा;
	    स्थिती 'b': buf->b_ind_case_break = (पूर्णांक)n; थांबा;
	    स्थिती 'p': buf->b_ind_param = (पूर्णांक)n; थांबा;
	    स्थिती 't': buf->b_ind_func_type = (पूर्णांक)n; थांबा;
	    स्थिती '/': buf->b_ind_comment = (पूर्णांक)n; थांबा;
	    स्थिती 'c': buf->b_ind_in_comment = (पूर्णांक)n; थांबा;
	    स्थिती 'C': buf->b_ind_in_comment2 = (पूर्णांक)n; थांबा;
	    स्थिती 'i': buf->b_ind_cpp_baseclass = (पूर्णांक)n; थांबा;
	    स्थिती '+': buf->b_ind_continuation = (पूर्णांक)n; थांबा;
	    स्थिती '(': buf->b_ind_unclosed = (पूर्णांक)n; थांबा;
	    स्थिती 'u': buf->b_ind_unclosed2 = (पूर्णांक)n; थांबा;
	    स्थिती 'U': buf->b_ind_unclosed_noignore = (पूर्णांक)n; थांबा;
	    स्थिती 'W': buf->b_ind_unclosed_wrapped = (पूर्णांक)n; थांबा;
	    स्थिती 'w': buf->b_ind_unclosed_whiteok = (पूर्णांक)n; थांबा;
	    स्थिती 'm': buf->b_ind_matching_paren = (पूर्णांक)n; थांबा;
	    स्थिती 'M': buf->b_ind_paren_prev = (पूर्णांक)n; थांबा;
	    स्थिती ')': buf->b_ind_maxparen = (पूर्णांक)n; थांबा;
	    स्थिती '*': buf->b_ind_maxcomment = (पूर्णांक)n; थांबा;
	    स्थिती 'g': buf->b_ind_scopedecl = (पूर्णांक)n; थांबा;
	    स्थिती 'h': buf->b_ind_scopedecl_code = (पूर्णांक)n; थांबा;
	    स्थिती 'j': buf->b_ind_java = (पूर्णांक)n; थांबा;
	    स्थिती 'J': buf->b_ind_js = (पूर्णांक)n; थांबा;
	    स्थिती 'l': buf->b_ind_keep_case_label = (पूर्णांक)n; थांबा;
	    स्थिती '#': buf->b_ind_hash_comment = (पूर्णांक)n; थांबा;
	    स्थिती 'N': buf->b_ind_cpp_namespace = (पूर्णांक)n; थांबा;
	    स्थिती 'k': buf->b_ind_if_for_while = (पूर्णांक)n; थांबा;
	    स्थिती 'E': buf->b_ind_cpp_extern_c = (पूर्णांक)n; थांबा;
	    स्थिती 'P': buf->b_ind_pragma = (पूर्णांक)n; थांबा;
	}
	जर (*p == ',')
	    ++p;
    }
}

    स्थिर पूर्णांक
find_match(पूर्णांक lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    पूर्णांक		elselevel;
    पूर्णांक		whilelevel;

    जर (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    अन्यथा
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    पर्यंत (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	जर (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    जर (theirscope == रिक्त)
		थांबा;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    जर (theirscope->lnum < ourscope)
		थांबा;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    जर (theirscope->lnum > ourscope)
		सुरूठेवा;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    जर (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		जर (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    जर (!cin_isif(mightbeif))
			++elselevel;
		    सुरूठेवा;
		}

		// If it's an "if" decrement elselevel
		जर (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    जर (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    जर (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		सुरूठेवा;
	    }

	    // If it's a "do" decrement whilelevel
	    जर (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    जर (elselevel <= 0 && whilelevel <= 0)
		परत OK;
	}
    }
    परत FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    पूर्णांक
get_c_indent(शून्य)
{
    pos_T	cur_curpos;
    पूर्णांक		amount;
    पूर्णांक		scope_amount;
    पूर्णांक		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = रिक्त;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    पूर्णांक		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    पूर्णांक		lookfor;
    पूर्णांक		whilelevel;
    linenr_T	lnum;
    पूर्णांक		n;
    पूर्णांक		iscase;
    पूर्णांक		lookfor_break;
    पूर्णांक		lookfor_cpp_namespace = FALSE;
    पूर्णांक		cont_amount = 0;    // amount for continuation line
    पूर्णांक		original_ओळ_islabel;
    पूर्णांक		added_to_amount = 0;
    पूर्णांक		js_cur_has_किल्ली = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    पूर्णांक		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    जर (cur_curpos.lnum == 1)
	परत 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    जर (linecopy == रिक्त)
	परत 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    जर ((State & MODE_घाला)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_ओळ_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    जर (comment_pos != रिक्त)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    जर (trypos != रिक्त && (comment_pos == रिक्त
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	जा laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    जर (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	जर (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    जा theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    जर (original_ओळ_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	जा theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    जर (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_ओळ_comment(); // XXX
	जर (trypos == रिक्त && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    जर (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	जर (trypos  != रिक्त)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, रिक्त, रिक्त, 0);
	    amount = col;
	    जा theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    जर (!cin_iscomment(theline) && comment_pos != रिक्त) // XXX
    {
	पूर्णांक	lead_start_len = 2;
	पूर्णांक	lead_मधला_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_मधला[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	पूर्णांक	lead_end_len;
	char_u	*p;
	पूर्णांक	start_align = 0;
	पूर्णांक	start_off = 0;
	पूर्णांक	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, रिक्त, रिक्त, 0);
	amount = col;
	*lead_start = NUL;
	*lead_मधला = NUL;

	p = curbuf->b_p_com;
	पर्यंत (*p != NUL)
	{
	    पूर्णांक	align = 0;
	    पूर्णांक	off = 0;
	    पूर्णांक what = 0;

	    पर्यंत (*p != NUL && *p != ':')
	    {
		जर (*p == COM_START || *p == COM_END || *p == COM_मधला)
		    what = *p++;
		अन्यथा जर (*p == COM_डावा || *p == COM_उजवा)
		    align = *p++;
		अन्यथा जर (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		अन्यथा
		    ++p;
	    }

	    जर (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    जर (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    अन्यथा जर (what == COM_मधला)
	    {
		STRCPY(lead_मधला, lead_end);
		lead_मधला_len = lead_end_len;
	    }
	    अन्यथा जर (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		जर (STRNCMP(theline, lead_मधला, lead_मधला_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    जर (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			जर (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			अन्यथा जर (STRNCMP(look, lead_मधला,
							lead_मधला_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    थांबा;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			अन्यथा जर (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    सुरूठेवा;
		    }
		    जर (start_off != 0)
			amount += start_off;
		    अन्यथा जर (start_align == COM_उजवा)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_मधला);
		    थांबा;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		जर (STRNCMP(theline, lead_मधला, lead_मधला_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    जर (off != 0)
			amount += off;
		    अन्यथा जर (align == COM_उजवा)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_मधला);
		    done = TRUE;
		    थांबा;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	जर (done)
	    ;
	अन्यथा जर (theline[0] == '*')
	    amount += 1;
	अन्यथा
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    साठी (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		जर (linewhite(lnum))		    // skip blank lines
		    सुरूठेवा;
		amount = get_indent_lnum(lnum);	    // XXX
		थांबा;
	    }
	    जर (amount == -1)			    // use the comment opener
	    {
		जर (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    जर (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, रिक्त, रिक्त, 0);
		amount = col;
		जर (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	जा theend;
    }

    // Are we looking at a ']' that has a match?
    जर (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != रिक्त)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	जा theend;
    }

    // Are we inside parentheses or braces?  XXX
    जर (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != रिक्त
	    || trypos != रिक्त)
    {
      जर (trypos != रिक्त && tryposBrace != रिक्त)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  जर (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = रिक्त;
	  अन्यथा
	      tryposBrace = रिक्त;
      }

      जर (trypos != रिक्त)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	जर (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	अन्यथा
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    साठी (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		जर (cin_nocode(l))		// skip comment lines
		    सुरूठेवा;
		जर (cin_ispreproc_cont(&l, &lnum, &amount))
		    सुरूठेवा;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		जर ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त)
		{
		    lnum = trypos->lnum + 1;
		    सुरूठेवा;
		}

		// XXX
		जर ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != रिक्त
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    जर (theline[0] == ')')
		    {
			जर (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    थांबा;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	जर (amount == -1)
	{
	    पूर्णांक	    ignore_paren_col = 0;
	    पूर्णांक	    is_if_for_while = 0;

	    जर (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *ओळ;

		trypos = &our_paren_pos;
		करा
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} पर्यंत (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		ओळ = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(ओळ, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    जर (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *ओळ;
		पूर्णांक	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		ओळ = ml_get_curline();
		look_col = (पूर्णांक)(look - ओळ);
		curwin->w_cursor.col = look_col + 1;
		जर ((trypos = findmatchlimit(रिक्त, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != रिक्त
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    जर (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		जर (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    जर (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			साठी (col = 0; col < our_paren_pos.col; ++col)
			{
			    निवड (l[col])
			    {
				स्थिती '(':
				स्थिती '{': ++n;
					  थांबा;

				स्थिती ')':
				स्थिती '}': जर (n > 1)
					      --n;
					  थांबा;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    अन्यथा जर (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    अन्यथा
		    {
			col = our_paren_pos.col + 1;
			पर्यंत (VIM_ISWHITE(l[col]))
			    col++;
			जर (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			अन्यथा
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		जर (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, रिक्त, रिक्त, 0);
		    जर (cur_amount > (पूर्णांक)col)
			cur_amount = col;
		}
	    }

	    जर (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    अन्यथा जर ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		जर (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    अन्यथा
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		पर्यंत ((पूर्णांक)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    निवड (*ml_get_pos(&our_paren_pos))
		    {
			स्थिती '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  थांबा;
			स्थिती ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  थांबा;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		जर (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		अन्यथा
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    जर (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != रिक्त)
			amount += curbuf->b_ind_unclosed2;
		    अन्यथा
		    {
			जर (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			अन्यथा
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		जर (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	जर (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      अन्यथा
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
	जर (*look == '{')
	{
	    getvcol(curwin, trypos, &col, रिक्त, रिक्त, 0);
	    amount = col;
	    जर (*start == '{')
		start_brace = BRACE_IN_COL0;
	    अन्यथा
		start_brace = BRACE_AT_START;
	}
	अन्यथा
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    जर (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != रिक्त)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    जर ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    अन्यथा जर (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    अन्यथा
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	जर (curbuf->b_ind_js)
	    js_cur_has_किल्ली = cin_has_js_किल्ली(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	जर (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	अन्यथा
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    जर (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    अन्यथा जर (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    जर (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		जर (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    जा theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    जर (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_डावा_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    अन्यथा जर (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    अन्यथा
	    {
		जर (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    जर (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    अन्यथा जर (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		अन्यथा
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    जर (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    जर (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    अन्यथा जर (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    अन्यथा
	    {
		जर (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    साठी (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		जर (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    जर (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			जर (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    जर (cont_amount > 0)
				amount = cont_amount;
			    अन्यथा जर (!curbuf->b_ind_js)
				amount += ind_continuation;
			    थांबा;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(रिक्त);
			जर (trypos != रिक्त)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    सुरूठेवा;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			जर (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    सुरूठेवा;

			जर (cin_nocode(l))
			    सुरूठेवा;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			जर (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    जर (terminated == ',')
				थांबा;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    जर (terminated != ';' && cin_isinit())
				थांबा;

			    // nothing useful found
			    जर (terminated == 0 || terminated == '{')
				सुरूठेवा;
			}

			जर (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = रिक्त;
			    जर (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    जर (trypos == रिक्त && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    जर (trypos != रिक्त)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				सुरूठेवा;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
		    }
		    अन्यथा जर (lookfor == LOOKFOR_UNTERM)
		    {
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
		    }
		    अन्यथा
		    {
			जर (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    जर (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			जर (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    जर (curwin->w_cursor.lnum == ourscope)
				सुरूठेवा;

			    जर (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				थांबा;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(रिक्त);
			    जर (trypos != रिक्त)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				सुरूठेवा;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    जर (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				सुरूठेवा;

			    // Finally the actual check for "namespace".
			    जर (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				थांबा;
			    }
			    अन्यथा जर (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				थांबा;
			    }

			    जर (cin_nocode(l))
				सुरूठेवा;
			}
		    }
		    थांबा;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		जर ((trypos = ind_find_start_CORS(&raw_string_start)) != रिक्त)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    सुरूठेवा;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		जर (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    जर (lookfor == LOOKFOR_CPP_BASECLASS)
			थांबा;

		    // When looking for a "do" we are not interested in
		    // labels.
		    जर (whilelevel > 0)
			सुरूठेवा;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    जर (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
			थांबा;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    जर (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			जर ((trypos = find_start_brace()) == रिक्त
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    थांबा;
			}
			सुरूठेवा;
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
		    जर (lookfor == LOOKFOR_TERM)
		    {
			जर (n)
			    amount = n;

			जर (!lookfor_break)
			    थांबा;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    जर (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			जर (l != रिक्त && cin_is_cinword(l))
			{
			    जर (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    अन्यथा
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			थांबा;
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
		    सुरूठेवा;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		जर (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    जर (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != रिक्त)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    सुरूठेवा;
		}

		// Ignore jump labels with nothing after them.
		जर (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    जर (l == रिक्त || cin_nocode(l))
			सुरूठेवा;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		जर (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    सुरूठेवा;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		जर (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		जर (n)
		{
		    जर (lookfor == LOOKFOR_UNTERM)
		    {
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
		    }
		    अन्यथा जर (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			सुरूठेवा;
		    }
		    अन्यथा
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    थांबा;
		}
		अन्यथा जर (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    जर (cin_isterminated(l, TRUE, FALSE))
			थांबा;
		    अन्यथा
			सुरूठेवा;
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

		जर (js_cur_has_किल्ली)
		{
		    js_cur_has_किल्ली = 0; // only check the first line
		    जर (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_किल्ली;
		    }
		}
		जर (lookfor == LOOKFOR_JS_किल्ली && cin_has_js_किल्ली(l))
		{
		    amount = get_indent();
		    थांबा;
		}
		जर (lookfor == LOOKFOR_COMMA)
		{
		    जर (tryposBrace != रिक्त && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			थांबा;
		    जर (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			थांबा;
		    अन्यथा
		    {
			amount = get_indent();
			जर (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    थांबा;
		    }
		}

		जर (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    जर (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    जर (trypos != रिक्त && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = रिक्त;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    जर (trypos == रिक्त && terminated == ',')
		    {
			जर (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    जर (trypos != रिक्त)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			जर (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    सुरूठेवा;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    जर (terminated == ',')
		    {
			पर्यंत (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    जर (*l == NUL || l[STRLEN(l) - 1] != '\\')
				थांबा;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    जर (curbuf->b_ind_js)
			cur_amount = get_indent();
		    अन्यथा
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    जर (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			जर (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			जर (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    सुरूठेवा;
			}
			थांबा;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    जर (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			जर (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    जर (cont_amount > 0)
				amount = cont_amount;
			    अन्यथा
				amount += ind_continuation;
			    थांबा;
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
			जर (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			जर (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    थांबा;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			जर (cin_isdo(l))
			{
			    जर (whilelevel == 0)
				थांबा;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			जर (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    जर (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    जर ((trypos = find_start_brace()) == रिक्त
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				थांबा;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    अन्यथा
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			जर (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    जर (terminated == ',')
				amount += ind_continuation;
			    थांबा;
			}

			जर (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    जर (terminated == ',')
			    {
				जर (curbuf->b_ind_cpp_baseclass == 0)
				    थांबा;

				lookfor = LOOKFOR_CPP_BASECLASS;
				सुरूठेवा;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    जर (amount > cur_amount)
				amount = cur_amount;
			}
			अन्यथा
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (पूर्णांक)STRLEN(l);
			    जर (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				थांबा;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    जर (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				जर (curbuf->b_ind_js)
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
				    जर (cin_iscomment(skipwhite(l)))
					थांबा;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    जर (trypos != रिक्त)
				    {
					जर (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    थांबा;
					}
					ourscope = trypos->lnum;
				    }
				}
				अन्यथा
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    अन्यथा
			    {
				जर (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				जर (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_किल्ली
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		अन्यथा जर (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    जर (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
			थांबा;
		    }

		    जर (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			जर (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		अन्यथा
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    जर (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			सुरूठेवा;
		    }

		    // Handle "do {" line.
		    जर (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			जर (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    सुरूठेवा;
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
		    जर (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			जर (cont_amount > 0)
			    amount = cont_amount;
			अन्यथा
			    amount += ind_continuation;
			थांबा;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    जर (lookfor == LOOKFOR_TERM)
		    {
			जर (!lookfor_break && whilelevel == 0)
			    थांबा;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    अन्यथा
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			जर (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != रिक्त)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    जर (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				सुरूठेवा;
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

			जर (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			जर (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			जर (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    जर ((trypos = find_start_brace()) == रिक्त
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				थांबा;
			    सुरूठेवा;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			जर (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != रिक्त)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    जर (*l == '}' || !cin_iselse(l))
				जा term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      जर (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      जर (curbuf->b_ind_jump_label > 0 && original_ओळ_islabel)
	  amount -= curbuf->b_ind_jump_label;

      जा theend;
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

    जर (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	जा theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    जर (cur_curpos.lnum < curbuf->b_ml.ml_ओळ_मोजणी
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
	जा theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    पर्यंत (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	जर ((trypos = ind_find_start_CORS(रिक्त)) != रिक्त)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    सुरूठेवा;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	जर (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	जर (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    थांबा;
	}

	// Skip preprocessor directives and blank lines.
	जर (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    सुरूठेवा;

	जर (cin_nocode(l))
	    सुरूठेवा;

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
	जर (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    जर (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != रिक्त)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    पर्यंत (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		जर (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    थांबा;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    जर (amount == 0)
		amount = cin_first_id_amount();
	    जर (amount == 0)
		amount = ind_continuation;
	    थांबा;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	जर (cin_isfuncdecl(रिक्त, cur_curpos.lnum, 0))  // XXX
	    थांबा;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	जर (*skipwhite(l) == '}')
	    थांबा;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	जर (cin_ends_in(l, (char_u *)"};"))
	    थांबा;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	जर (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    थांबा;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	जर (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    पर्यंत (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		जर (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    थांबा;
	    }
	    जर (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		थांबा;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	जर (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    थांबा;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	जर (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    जर (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		थांबा;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	जर ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != रिक्त)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	थांबा;
    }

    // add extra indent for a comment
    जर (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    जर (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	जर (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    जर (cur_amount > 0)
		amount = cur_amount;
	    अन्यथा जर (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    जर (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    परत amount;
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
    पूर्णांक
in_cinkeys(
    पूर्णांक		keytyped,
    पूर्णांक		when,
    पूर्णांक		ओळ_is_empty)
{
    char_u	*look;
    पूर्णांक		try_match;
    पूर्णांक		try_match_word;
    char_u	*p;
    char_u	*ओळ;
    पूर्णांक		icase;
    पूर्णांक		i;

    जर (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	परत FALSE;

#ifdef FEAT_EVAL
    जर (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    अन्यथा
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    पर्यंत (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	निवड (when)
	{
	    स्थिती '*': try_match = (*look == '*'); थांबा;
	    स्थिती '!': try_match = (*look == '!'); थांबा;
	    पूर्वनिर्धारित: try_match = (*look != '*'); थांबा;
	}
	जर (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	जर (*look == '0')
	{
	    try_match_word = try_match;
	    जर (!ओळ_is_empty)
		try_match = FALSE;
	    ++look;
	}
	अन्यथा
	    try_match_word = FALSE;

	// does it look like a control character?
	जर (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    जर (try_match && keytyped == Ctrl_chr(look[1]))
		परत TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	अन्यथा जर (*look == 'o')
	{
	    जर (try_match && keytyped == किल्ली_OPEN_FORW)
		परत TRUE;
	    ++look;
	}
	अन्यथा जर (*look == 'O')
	{
	    जर (try_match && keytyped == किल्ली_OPEN_BACK)
		परत TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	अन्यथा जर (*look == 'e')
	{
	    जर (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		जर (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    परत TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	अन्यथा जर (*look == ':')
	{
	    जर (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		जर (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    परत TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		जर (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    जर (i)
			परत TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	अन्यथा जर (*look == '<')
	{
	    जर (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		जर (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != रिक्त
						       && keytyped == look[1])
		    परत TRUE;

		जर (keytyped == get_special_किल्ली_code(look + 1))
		    परत TRUE;
	    }
	    पर्यंत (*look && *look != '>')
		look++;
	    पर्यंत (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	अन्यथा जर (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    जर (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    अन्यथा
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    जर (p == रिक्त)
		p = look + STRLEN(look);
	    जर ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		पूर्णांक		match = FALSE;

		जर (keytyped == किल्ली_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    ओळ = ml_get_curline();
		    जर (has_mbyte)
		    {
			char_u	*n;

			साठी (s = ओळ + curwin->w_cursor.col; s > ओळ; s = n)
			{
			    n = mb_prevptr(ओळ, s);
			    जर (!vim_iswordp(n))
				थांबा;
			}
		    }
		    अन्यथा
			साठी (s = ओळ + curwin->w_cursor.col; s > ओळ; --s)
			    जर (!vim_iswordc(s[-1]))
				थांबा;
		    जर (s + (p - look) <= ओळ + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		अन्यथा
		    // TODO: multi-byte
		    जर (keytyped == (पूर्णांक)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((पूर्णांक)p[-1])))
		{
		    ओळ = ml_get_cursor();
		    जर ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(ओळ[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(ओळ - (p - look), look, p - look)
				: STRNCMP(ओळ - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		जर (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    जर (getwhitecols_curline() !=
				     (पूर्णांक)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		जर (match)
		    परत TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	अन्यथा
	{
	    जर (try_match && *look == keytyped)
		परत TRUE;
	    जर (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    परत FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    शून्य
do_c_अभिव्यक्ती_indent(शून्य)
{
#ifdef FEAT_EVAL
    जर (*curbuf->b_p_inde != NUL)
	fixthisline(get_अभिव्यक्ती_indent);
    अन्यथा
#endif
	fixthisline(get_c_indent);
}

#जर defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    शून्य
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    जर (lnum >= 1 && lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_संख्या = get_c_indent();
	curwin->w_cursor = pos;
    }
    अन्यथा
	rettv->vval.v_संख्या = -1;
}
#endif
