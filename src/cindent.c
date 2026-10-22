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

#അന്തര്ഭാവ "vim.h"

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
#define LOOKFOR_JS_താക്കോൽ		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    പൂർണ്ണസംഖ്യ
cin_is_cinword(char_u *വരി)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    പൂർണ്ണസംഖ്യ		cinw_len;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		len;

    cinw_len = (പൂർണ്ണസംഖ്യ)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    എങ്കിൽ (cinw_buf == ശൂന്യ)
	തിരികെ FALSE;

    വരി = skipwhite(വരി);
    വേണ്ടി (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	എങ്കിൽ (STRNCMP(വരി, cinw_buf, len) == 0
		&& (!vim_iswordc(വരി[len]) || !vim_iswordc(വരി[len - 1])))
	{
	    retval = TRUE;
	    നിർത്തുക;
	}
    }
    vim_free(cinw_buf);
    തിരികെ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    സ്ഥിതം char_u *
skip_string(char_u *p)
{
    പൂർണ്ണസംഖ്യ	    i;

    // We loop, because strings may be concatenated: "date""time".
    വേണ്ടി ( ; ; ++p)
    {
	എങ്കിൽ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    എങ്കിൽ (p[1] == NUL)		    // ' at end of line
		നിർത്തുക;
	    i = 2;
	    എങ്കിൽ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ആകുമ്പോൾ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    എങ്കിൽ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		തുടരുക;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '"')		    // start of string
	{
	    വേണ്ടി (++p; p[0]; ++p)
	    {
		എങ്കിൽ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '"')	    // end of string
		    നിർത്തുക;
	    }
	    എങ്കിൽ (p[0] == '"')
		തുടരുക; // continue for another string
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    എങ്കിൽ (paren != ശൂന്യ)
	    {
		size_t delim_len = paren - delim;

		വേണ്ടി (p += 3; *p; ++p)
		    എങ്കിൽ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			നിർത്തുക;
		    }
		എങ്കിൽ (p[0] == '"')
		    തുടരുക; // continue for another string
	    }
	}
	നിർത്തുക;				    // no string found
    }
    എങ്കിൽ (!*p)
	--p;				    // backup from NUL
    തിരികെ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    പൂർണ്ണസംഖ്യ
is_pos_in_string(char_u *വരി, colnr_T col)
{
    char_u *p;

    വേണ്ടി (p = വരി; *p && (colnr_T)(p - വരി) < col; ++p)
	p = skip_string(p);
    തിരികെ !((colnr_T)(p - വരി) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    പൂർണ്ണസംഖ്യ
check_linecomment(char_u *വരി)
{
    char_u  *p;

    p = വരി;
    // skip Lispish one-line comments
    എങ്കിൽ (curbuf->b_p_lisp)
    {
	എങ്കിൽ (vim_strchr(p, ';') != ശൂന്യ) // there may be comments
	{
	    പൂർണ്ണസംഖ്യ in_str = FALSE;	// inside of string

	    p = വരി;		// scan from start
	    ആകുമ്പോൾ ((p = vim_strpbrk(p, (char_u *)"\";")) != ശൂന്യ)
	    {
		എങ്കിൽ (*p == '"')
		{
		    എങ്കിൽ (in_str)
		    {
			എങ്കിൽ (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (p == വരി || ((p - വരി) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!in_str && ((p - വരി) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(വരി, (colnr_T)(p - വരി)))
		    നിർത്തുക;	// found!
		++p;
	    }
	}
	അല്ലെങ്കിൽ
	    p = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	വേണ്ടി ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    എങ്കിൽ (p[0] == '/' && p[1] == '/'
				 && (p == വരി || p[-1] != '*' || p[2] != '*'))
		നിർത്തുക;
	}
	എങ്കിൽ (*p == NUL)
	    p = ശൂന്യ;
    }

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ MAXCOL;
    തിരികെ (പൂർണ്ണസംഖ്യ)(p - വരി);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    സ്ഥിതം pos_T *
ind_find_start_comment(ശൂന്യം)	    // XXX
{
    തിരികെ find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(പൂർണ്ണസംഖ്യ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    പൂർണ്ണസംഖ്യ		cur_maxcomment = ind_maxcomment;

    വേണ്ടി (;;)
    {
	pos = findmatchlimit(ശൂന്യ, '*', FM_BACKWARD, cur_maxcomment);
	എങ്കിൽ (pos == ശൂന്യ)
	    നിർത്തുക;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	എങ്കിൽ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    നിർത്തുക;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	എങ്കിൽ (cur_maxcomment <= 0)
	{
	    pos = ശൂന്യ;
	    നിർത്തുക;
	}
    }
    തിരികെ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    സ്ഥിതം pos_T *
find_start_rawstring(പൂർണ്ണസംഖ്യ ind_maxcomment)	// XXX
{
    pos_T	*pos;
    പൂർണ്ണസംഖ്യ		cur_maxcomment = ind_maxcomment;

    വേണ്ടി (;;)
    {
	pos = findmatchlimit(ശൂന്യ, 'R', FM_BACKWARD, cur_maxcomment);
	എങ്കിൽ (pos == ശൂന്യ)
	    നിർത്തുക;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	എങ്കിൽ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    നിർത്തുക;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	എങ്കിൽ (cur_maxcomment <= 0)
	{
	    pos = ശൂന്യ;
	    നിർത്തുക;
	}
    }
    തിരികെ pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    സ്ഥിതം pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    സ്ഥിതം pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    എങ്കിൽ (comment_pos != ശൂന്യ)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    എങ്കിൽ (comment_pos == ശൂന്യ || (rs_pos != ശൂന്യ
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	എങ്കിൽ (is_raw != ശൂന്യ && rs_pos != ശൂന്യ)
	    *is_raw = rs_pos->lnum;
	തിരികെ rs_pos;
    }
    തിരികെ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    പൂർണ്ണസംഖ്യ
cindent_on(ശൂന്യം)
{
    തിരികെ (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    സ്ഥിതം char_u *
cin_skipcomment(char_u *s)
{
    ആകുമ്പോൾ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	എങ്കിൽ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    നിർത്തുക;
	}
	എങ്കിൽ (*s != '/')
	    നിർത്തുക;
	++s;
	എങ്കിൽ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    നിർത്തുക;
	}
	എങ്കിൽ (*s != '*')
	    നിർത്തുക;
	വേണ്ടി (++s; *s; ++s)	// skip slash-star comment
	    എങ്കിൽ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		നിർത്തുക;
	    }
    }
    തിരികെ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_nocode(char_u *s)
{
    തിരികെ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_iscomment(char_u *p)
{
    തിരികെ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_islinecomment(char_u *p)
{
    തിരികെ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    സ്ഥിതം pos_T *
find_വരി_comment(ശൂന്യം) // XXX
{
    സ്ഥിതം pos_T pos;
    char_u	 *വരി;
    char_u	 *p;

    pos = curwin->w_cursor;
    ആകുമ്പോൾ (--pos.lnum > 0)
    {
	വരി = ml_get(pos.lnum);
	p = skipwhite(വരി);
	എങ്കിൽ (cin_islinecomment(p))
	{
	    pos.col = (പൂർണ്ണസംഖ്യ)(p - വരി);
	    തിരികെ &pos;
	}
	എങ്കിൽ (*p != NUL)
	    നിർത്തുക;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_has_js_താക്കോൽ(char_u *text)
{
    char_u *s = skipwhite(text);
    പൂർണ്ണസംഖ്യ	    quote = -1;

    എങ്കിൽ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    എങ്കിൽ (!vim_isIDc(*s))	    // need at least one ID character
	തിരികെ FALSE;

    ആകുമ്പോൾ (vim_isIDc(*s))
	++s;
    എങ്കിൽ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    തിരികെ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_islabel_skip(char_u **s)
{
    എങ്കിൽ (!vim_isIDc(**s))	    // need at least one ID character
	തിരികെ FALSE;

    ആകുമ്പോൾ (vim_isIDc(**s))
    {
	എങ്കിൽ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	അല്ലെങ്കിൽ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    തിരികെ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    പൂർണ്ണസംഖ്യ	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    എങ്കിൽ (cinsd_buf == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (പൂർണ്ണസംഖ്യ)cinsd_len, ",");
	എങ്കിൽ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    എങ്കിൽ (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		നിർത്തുക;
	    }
	}
    }

    vim_free(cinsd_buf);
    തിരികെ found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_ispreproc(char_u *s)
{
    എങ്കിൽ (*skipwhite(s) == '#')
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, പൂർണ്ണസംഖ്യ *amount)
{
    char_u	*വരി = *pp;
    linenr_T	lnum = *lnump;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		candidate_amount = *amount;

    എങ്കിൽ (*വരി != NUL && വരി[STRLEN(വരി) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    വേണ്ടി (;;)
    {
	എങ്കിൽ (cin_ispreproc(വരി))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    നിർത്തുക;
	}
	എങ്കിൽ (lnum == 1)
	    നിർത്തുക;
	വരി = ml_get(--lnum);
	എങ്കിൽ (*വരി == NUL || വരി[STRLEN(വരി) - 1] != '\\')
	    നിർത്തുക;
    }

    എങ്കിൽ (lnum != *lnump)
	*pp = ml_get(*lnump);
    എങ്കിൽ (retval)
	*amount = candidate_amount;
    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_iselse(
    char_u  *p)
{
    എങ്കിൽ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    തിരികെ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isterminated(
    char_u	*s,
    പൂർണ്ണസംഖ്യ		incl_open,	// include '{' at the end as terminator
    പൂർണ്ണസംഖ്യ		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    പൂർണ്ണസംഖ്യ		is_else = FALSE;

    s = cin_skipcomment(s);

    എങ്കിൽ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    എങ്കിൽ (!found_start)
	is_else = cin_iselse(s);

    ആകുമ്പോൾ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	എങ്കിൽ (*s == '}' && n_open > 0)
	    --n_open;
	എങ്കിൽ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    തിരികെ *s;
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '{')
	{
	    എങ്കിൽ (incl_open && cin_nocode(s + 1))
		തിരികെ *s;
	    അല്ലെങ്കിൽ
		++n_open;
	}

	എങ്കിൽ (*s)
	    s++;
    }
    തിരികെ found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_starts_with(char_u *s, അക്ഷരം *word)
{
    പൂർണ്ണസംഖ്യ l = (പൂർണ്ണസംഖ്യ)STRLEN(word);

    തിരികെ (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isdefault(char_u *s)
{
    തിരികെ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_iscase(
    char_u *s,
    പൂർണ്ണസംഖ്യ strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    എങ്കിൽ (cin_starts_with(s, "case"))
    {
	വേണ്ടി (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    എങ്കിൽ (*s == NUL)
		നിർത്തുക;
	    എങ്കിൽ (*s == ':')
	    {
		എങ്കിൽ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		അല്ലെങ്കിൽ
		    തിരികെ TRUE;
	    }
	    എങ്കിൽ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    അല്ലെങ്കിൽ എങ്കിൽ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		തിരികെ FALSE;		// stop at comment
	    അല്ലെങ്കിൽ എങ്കിൽ (*s == '"')
	    {
		// JS etc.
		എങ്കിൽ (strict)
		    തിരികെ FALSE;		// stop at string
		അല്ലെങ്കിൽ
		    തിരികെ TRUE;
	    }
	}
	തിരികെ FALSE;
    }

    എങ്കിൽ (cin_isdefault(s))
	തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_islabel(ശൂന്യം)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    എങ്കിൽ (cin_isdefault(s))
	തിരികെ FALSE;
    എങ്കിൽ (cin_isscopedecl(s))
	തിരികെ FALSE;

    എങ്കിൽ (!cin_islabel_skip(&s))
	തിരികെ FALSE;

    എങ്കിൽ (ind_find_start_CORS(ശൂന്യ))
	തിരികെ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*വരി;

    cursor_save = curwin->w_cursor;
    ആകുമ്പോൾ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	എങ്കിൽ ((trypos = ind_find_start_CORS(ശൂന്യ)) != ശൂന്യ) // XXX
	    curwin->w_cursor = *trypos;

	വരി = ml_get_curline();
	എങ്കിൽ (cin_ispreproc(വരി))	// ignore #defines, #if, etc.
	    തുടരുക;
	എങ്കിൽ (*(വരി = cin_skipcomment(വരി)) == NUL)
	    തുടരുക;

	curwin->w_cursor = cursor_save;
	എങ്കിൽ (cin_isterminated(വരി, TRUE, FALSE)
		|| cin_isscopedecl(വരി)
		|| cin_iscase(വരി, TRUE)
		|| (cin_islabel_skip(&വരി) && cin_nocode(വരി)))
	    തിരികെ TRUE;
	തിരികെ FALSE;
    }
    curwin->w_cursor = cursor_save;
    തിരികെ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    പൂർണ്ണസംഖ്യ		len = (പൂർണ്ണസംഖ്യ)STRLEN(find);

    ആകുമ്പോൾ (*p != NUL)
    {
	p = cin_skipcomment(p);
	എങ്കിൽ (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    എങ്കിൽ (cin_nocode(r))
		തിരികെ TRUE;
	}
	എങ്കിൽ (*p != NUL)
	    ++p;
    }
    തിരികെ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    സ്ഥിതം char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ശൂന്യ, *p = s;
    ചെയ്യുക
    {
	r = p;
	p = cin_skipcomment(p);
	എങ്കിൽ (*p)
	    p = skip_string(p);
    } ആകുമ്പോൾ (p != r);
    തിരികെ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ശൂന്യ;

    ആകുമ്പോൾ (*p)
    {
	എങ്കിൽ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	അല്ലെങ്കിൽ
	    p = cin_skip_comment_and_string(p + 1);
    }
    എങ്കിൽ (!r)
	തിരികെ FALSE;
    p = r; // p points now after '=' or "return"

    എങ്കിൽ (cin_nocode(p))
	തിരികെ TRUE;

    എങ്കിൽ (*p == '&')
	p = cin_skipcomment(p + 1);

    എങ്കിൽ (*p == '(') // skip a typecast
    {
	പൂർണ്ണസംഖ്യ open_എണ്ണം = 1;
	ചെയ്യുക
	{
	    p = cin_skip_comment_and_string(p + 1);
	    എങ്കിൽ (cin_nocode(p))
		തിരികെ TRUE;
	    open_എണ്ണം += (*p == '(') - (*p == ')');
	} ആകുമ്പോൾ (open_എണ്ണം);
	p = cin_skipcomment(p + 1);
	എങ്കിൽ (cin_nocode(p))
	    തിരികെ TRUE;
    }

    ആകുമ്പോൾ (*p == '{')
	p = cin_skipcomment(p + 1);
    തിരികെ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isinit(ശൂന്യം)
{
    char_u	*s;
    സ്ഥിതം അക്ഷരം *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    എങ്കിൽ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ i, l;

	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(skip); ++i)
	{
	    l = (പൂർണ്ണസംഖ്യ)strlen(skip[i]);
	    എങ്കിൽ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (l != 0)
	    നിർത്തുക;
    }

    എങ്കിൽ (cin_starts_with(s, "enum"))
	തിരികെ TRUE;

    തിരികെ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		has_name = FALSE;
    പൂർണ്ണസംഖ്യ		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ആകുമ്പോൾ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    എങ്കിൽ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ആകുമ്പോൾ (*p != NUL)
	{
	    എങ്കിൽ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '{')
	    {
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		എങ്കിൽ (has_name)
		    തിരികെ FALSE; // word character after skipping past name
		++p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		എങ്കിൽ (!has_name_start || has_name)
		    തിരികെ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    അല്ലെങ്കിൽ
	    {
		തിരികെ FALSE;
	    }
	}
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    എങ്കിൽ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ആകുമ്പോൾ (*p != NUL)
	{
	    എങ്കിൽ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '{')
	    {
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		എങ്കിൽ (has_string_literal)
		    തിരികെ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		എങ്കിൽ (has_string_literal)
		    തിരികെ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    അല്ലെങ്കിൽ
	    {
		തിരികെ FALSE;
	    }
	}
	തിരികെ has_string_literal ? TRUE : FALSE;
    }
    തിരികെ FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    സ്ഥിതം char_u *
after_label(char_u *l)
{
    വേണ്ടി ( ; *l; ++l)
    {
	എങ്കിൽ (*l == ':')
	{
	    എങ്കിൽ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    അല്ലെങ്കിൽ എങ്കിൽ (!cin_iscase(l + 1, FALSE))
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    എങ്കിൽ (*l == NUL)
	തിരികെ ശൂന്യ;
    l = cin_skipcomment(l + 1);
    എങ്കിൽ (*l == NUL)
	തിരികെ ശൂന്യ;
    തിരികെ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, ശൂന്യ, ശൂന്യ, 0);
    തിരികെ (പൂർണ്ണസംഖ്യ)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    പൂർണ്ണസംഖ്യ		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    എങ്കിൽ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	എങ്കിൽ (l == ശൂന്യ)		// just in case
	    l = ml_get_curline();
    }
    അല്ലെങ്കിൽ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    തിരികെ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_first_id_amount(ശൂന്യം)
{
    char_u	*വരി, *p, *s;
    പൂർണ്ണസംഖ്യ		len;
    pos_T	fp;
    colnr_T	col;

    വരി = ml_get_curline();
    p = skipwhite(വരി);
    len = (പൂർണ്ണസംഖ്യ)(skiptowhite(p) - p);
    എങ്കിൽ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (പൂർണ്ണസംഖ്യ)(skiptowhite(p) - p);
    }
    എങ്കിൽ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    അല്ലെങ്കിൽ എങ്കിൽ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    അല്ലെങ്കിൽ എങ്കിൽ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	എങ്കിൽ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    വേണ്ടി (len = 0; vim_isIDc(p[len]); ++len)
	;
    എങ്കിൽ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	തിരികെ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - വരി);
    getvcol(curwin, &fp, &col, ശൂന്യ, ശൂന്യ, 0);
    തിരികെ (പൂർണ്ണസംഖ്യ)col;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*വരി;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    എങ്കിൽ (lnum > 1)
    {
	വരി = ml_get(lnum - 1);
	എങ്കിൽ (*വരി != NUL && വരി[STRLEN(വരി) - 1] == '\\')
	    തിരികെ -1;
    }

    വരി = s = ml_get(lnum);
    ആകുമ്പോൾ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ശൂന്യ)
    {
	എങ്കിൽ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	അല്ലെങ്കിൽ
	    ++s;
    }
    എങ്കിൽ (*s != '=')
	തിരികെ 0;

    s = skipwhite(s + 1);
    എങ്കിൽ (cin_nocode(s))
	തിരികെ 0;

    എങ്കിൽ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - വരി);
    getvcol(curwin, &fp, &col, ശൂന്യ, ശൂന്യ, 0);
    തിരികെ (പൂർണ്ണസംഖ്യ)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_skip2pos(pos_T *trypos)
{
    char_u	*വരി;
    char_u	*p;
    char_u	*new_p;

    p = വരി = ml_get(trypos->lnum);
    ആകുമ്പോൾ (*p && (colnr_T)(p - വരി) < trypos->col)
    {
	എങ്കിൽ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	അല്ലെങ്കിൽ
	{
	    new_p = skip_string(p);
	    എങ്കിൽ (new_p == p)
		++p;
	    അല്ലെങ്കിൽ
		p = new_p;
	}
    }
    തിരികെ (പൂർണ്ണസംഖ്യ)(p - വരി);
}

    സ്ഥിതം pos_T *
find_match_char(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    സ്ഥിതം pos_T pos_copy;
    പൂർണ്ണസംഖ്യ		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    എങ്കിൽ ((trypos = findmatchlimit(ശൂന്യ, c, FM_SKIPCOMM, ind_maxp_wk)) != ശൂന്യ)
    {
	// check if the ( is in a // comment
	എങ്കിൽ ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (പൂർണ്ണസംഖ്യ)(cursor_save.lnum - trypos->lnum);
	    എങ്കിൽ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		പോകുക retry;
	    }
	    trypos = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    എങ്കിൽ ((trypos_wk = ind_find_start_CORS(ശൂന്യ)) != ശൂന്യ) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (പൂർണ്ണസംഖ്യ)(cursor_save.lnum
			- trypos_wk->lnum);
		എങ്കിൽ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    പോകുക retry;
		}
		trypos = ശൂന്യ;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    തിരികെ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    സ്ഥിതം pos_T *
find_match_paren(പൂർണ്ണസംഖ്യ ind_maxparen)	// XXX
{
    തിരികെ find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_last_paren(char_u *l, പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ end)
{
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		open_എണ്ണം = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    വേണ്ടി (i = 0; l[i] != NUL; i++)
    {
	i = (പൂർണ്ണസംഖ്യ)(cin_skipcomment(l + i) - l); // ignore parens in comments
	എങ്കിൽ (l[i] == NUL)
	    നിർത്തുക;
	i = (പൂർണ്ണസംഖ്യ)(skip_string(l + i) - l);    // ignore parens in quotes
	എങ്കിൽ (l[i] == start)
	    ++open_എണ്ണം;
	അല്ലെങ്കിൽ എങ്കിൽ (l[i] == end)
	{
	    എങ്കിൽ (open_എണ്ണം > 0)
		--open_എണ്ണം;
	    അല്ലെങ്കിൽ
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    തിരികെ retval;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    pos_T	*trypos;
    പൂർണ്ണസംഖ്യ		just_started = TRUE;

    എങ്കിൽ (sp == ശൂന്യ)
	s = ml_get(lnum);
    അല്ലെങ്കിൽ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    എങ്കിൽ (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ശൂന്യ)
    {
	lnum = trypos->lnum;
	എങ്കിൽ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    തിരികെ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    എങ്കിൽ (cin_ispreproc(s))
	തിരികെ FALSE;

    ആകുമ്പോൾ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	എങ്കിൽ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	അല്ലെങ്കിൽ എങ്കിൽ (*s == ':')
	{
	    എങ്കിൽ (*(s + 1) == ':')
		s += 2;
	    അല്ലെങ്കിൽ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		തിരികെ FALSE;
	}
	അല്ലെങ്കിൽ
	    ++s;
    }
    എങ്കിൽ (*s != '(')
	തിരികെ FALSE;		// ';', ' or "  before any () or no '('

    ആകുമ്പോൾ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	എങ്കിൽ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    എങ്കിൽ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    പോകുക done;
	}
	എങ്കിൽ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    പൂർണ്ണസംഖ്യ comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (lnum >= curbuf->b_ml.ml_വരി_എണ്ണം)
		    നിർത്തുക;
		s = ml_get(++lnum);
		എങ്കിൽ (!cin_ispreproc(s))
		    നിർത്തുക;
	    }
	    എങ്കിൽ (lnum >= curbuf->b_ml.ml_വരി_എണ്ണം)
		നിർത്തുക;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    എങ്കിൽ (!just_started && (!comma && *s != ',' && *s != ')'))
		നിർത്തുക;
	    just_started = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	അല്ലെങ്കിൽ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    എങ്കിൽ (lnum != first_lnum && sp != ശൂന്യ)
	*sp = ml_get(first_lnum);

    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isif(char_u *p)
{
    തിരികെ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isdo(char_u *p)
{
    തിരികെ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    പൂർണ്ണസംഖ്യ		retval = FALSE;

    p = cin_skipcomment(p);
    എങ്കിൽ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    എങ്കിൽ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	ആകുമ്പോൾ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	എങ്കിൽ ((trypos = findmatchlimit(ശൂന്യ, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ശൂന്യ
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    തിരികെ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_is_if_for_while_before_offset(char_u *വരി, പൂർണ്ണസംഖ്യ *poffset)
{
    പൂർണ്ണസംഖ്യ offset = *poffset;

    എങ്കിൽ (offset-- < 2)
	തിരികെ 0;
    ആകുമ്പോൾ (offset > 2 && VIM_ISWHITE(വരി[offset]))
	--offset;

    offset -= 1;
    എങ്കിൽ (STRNCMP(വരി + offset, "if", 2) == 0)
	പോകുക probablyFound;

    എങ്കിൽ (offset >= 1)
    {
	offset -= 1;
	എങ്കിൽ (STRNCMP(വരി + offset, "for", 3) == 0)
	    പോകുക probablyFound;

	എങ്കിൽ (offset >= 2)
	{
	    offset -= 2;
	    എങ്കിൽ (STRNCMP(വരി + offset, "while", 5) == 0)
		പോകുക probablyFound;
	}
    }
    തിരികെ 0;

probablyFound:
    എങ്കിൽ (!offset || !vim_isIDc(വരി[offset - 1]))
    {
	*poffset = offset;
	തിരികെ 1;
    }
    തിരികെ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_iswhileofdo_end(പൂർണ്ണസംഖ്യ terminated)
{
    char_u	*വരി;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (terminated != ';')	// there must be a ';' at the end
	തിരികെ FALSE;

    p = വരി = ml_get_curline();
    ആകുമ്പോൾ (*p != NUL)
    {
	p = cin_skipcomment(p);
	എങ്കിൽ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    എങ്കിൽ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (പൂർണ്ണസംഖ്യ)(p - വരി);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		എങ്കിൽ (trypos != ശൂന്യ)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    എങ്കിൽ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    എങ്കിൽ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			തിരികെ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		വരി = ml_get_curline();
		p = വരി + i;
	    }
	}
	എങ്കിൽ (*p != NUL)
	    ++p;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_isbreak(char_u *p)
{
    തിരികെ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    പൂർണ്ണസംഖ്യ		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*വരി = ml_get_curline();

    എങ്കിൽ (pos->lnum <= lnum)
	തിരികെ cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(വരി);
    എങ്കിൽ (*s == '#')		// skip #define FOO x ? (x) : x
	തിരികെ FALSE;
    s = cin_skipcomment(s);
    എങ്കിൽ (*s == NUL)
	തിരികെ FALSE;

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
    ആകുമ്പോൾ (lnum > 1)
    {
	വരി = ml_get(lnum - 1);
	s = skipwhite(വരി);
	എങ്കിൽ (*s == '#' || *s == NUL)
	    നിർത്തുക;
	ആകുമ്പോൾ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    എങ്കിൽ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		നിർത്തുക;
	    എങ്കിൽ (*s != NUL)
		++s;
	}
	എങ്കിൽ (*s != NUL)
	    നിർത്തുക;
	--lnum;
    }

    pos->lnum = lnum;
    വരി = ml_get(lnum);
    s = വരി;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (*s == NUL)
	{
	    എങ്കിൽ (lnum == curwin->w_cursor.lnum)
		നിർത്തുക;
	    // Continue in the cursor line.
	    വരി = ml_get(++lnum);
	    s = വരി;
	}
	എങ്കിൽ (s == വരി)
	{
	    // don't recognize "case (foo):" as a baseclass
	    എങ്കിൽ (cin_iscase(s, FALSE))
		നിർത്തുക;
	    s = cin_skipcomment(വരി);
	    എങ്കിൽ (*s == NUL)
		തുടരുക;
	}

	എങ്കിൽ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (s[0] == ':')
	{
	    എങ്കിൽ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    അല്ലെങ്കിൽ
		s = cin_skipcomment(s + 1);
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    എങ്കിൽ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    അല്ലെങ്കിൽ
		s = cin_skipcomment(s + 6);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		തിരികെ FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		എങ്കിൽ (cpp_base_class)
		    pos->col = (colnr_T)(s - വരി);
	    }

	    // When the line ends in a comma don't align with it.
	    എങ്കിൽ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    എങ്കിൽ (cpp_base_class)
	pos->lnum = lnum;
    തിരികെ cpp_base_class;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_baseclass_amount(പൂർണ്ണസംഖ്യ col)
{
    പൂർണ്ണസംഖ്യ		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    എങ്കിൽ (col == 0)
    {
	amount = get_indent();
	എങ്കിൽ (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ശൂന്യ)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	എങ്കിൽ (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    അല്ലെങ്കിൽ
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, ശൂന്യ, ശൂന്യ, 0);
	amount = (പൂർണ്ണസംഖ്യ)vcol;
    }
    എങ്കിൽ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    തിരികെ amount;
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

    സ്ഥിതം pos_T *
find_start_brace(ശൂന്യം)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    സ്ഥിതം pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ആകുമ്പോൾ ((trypos = findmatchlimit(ശൂന്യ, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ശൂന്യ)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ശൂന്യ;
	// ignore the { if it's in a // or / *  * / comment
	എങ്കിൽ ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(ശൂന്യ)) == ശൂന്യ) // XXX
	    നിർത്തുക;
	എങ്കിൽ (pos != ശൂന്യ)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    തിരികെ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    സ്ഥിതം pos_T *
find_match_paren_after_brace (പൂർണ്ണസംഖ്യ ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    എങ്കിൽ (trypos == ശൂന്യ)
	തിരികെ ശൂന്യ;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    എങ്കിൽ (tryposBrace != ശൂന്യ
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = ശൂന്യ;
    തിരികെ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    എങ്കിൽ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	തിരികെ curbuf->b_ind_maxparen - (പൂർണ്ണസംഖ്യ)n;
    തിരികെ curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    ശൂന്യം
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    പൂർണ്ണസംഖ്യ		divider;
    പൂർണ്ണസംഖ്യ		fraction = 0;
    പൂർണ്ണസംഖ്യ		sw;
    long	t = get_sw_മൂല്യം(buf);

    // needed for cino-(, it will be multiplied by 2 again
    എങ്കിൽ (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    അല്ലെങ്കിൽ
	sw = (പൂർണ്ണസംഖ്യ)t;

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
    buf->b_ind_open_ഇടത്_imag = 0;

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

    വേണ്ടി (p = buf->b_p_cino; *p; )
    {
	l = p++;
	എങ്കിൽ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	എങ്കിൽ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((അക്ഷരം *)++p);
	    ആകുമ്പോൾ (VIM_ISDIGIT(*p))
	    {
		++p;
		എങ്കിൽ (divider)
		    divider *= 10;
		അല്ലെങ്കിൽ
		    divider = 10;
	    }
	}
	എങ്കിൽ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    എങ്കിൽ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    അല്ലെങ്കിൽ
	    {
		n *= sw;
		എങ്കിൽ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	എങ്കിൽ (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	തിരഞ്ഞെടുപ്പ് (*l)
	{
	    അവസ്ഥ '>': buf->b_ind_level = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'e': buf->b_ind_open_imag = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'n': buf->b_ind_no_brace = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'f': buf->b_ind_first_open = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '{': buf->b_ind_open_extra = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '}': buf->b_ind_close_extra = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '^': buf->b_ind_open_ഇടത്_imag = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'L': buf->b_ind_jump_label = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ ':': buf->b_ind_case = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '=': buf->b_ind_case_code = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'b': buf->b_ind_case_break = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'p': buf->b_ind_param = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 't': buf->b_ind_func_type = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '/': buf->b_ind_comment = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'c': buf->b_ind_in_comment = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'C': buf->b_ind_in_comment2 = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'i': buf->b_ind_cpp_baseclass = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '+': buf->b_ind_continuation = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '(': buf->b_ind_unclosed = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'u': buf->b_ind_unclosed2 = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'U': buf->b_ind_unclosed_noignore = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'W': buf->b_ind_unclosed_wrapped = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'w': buf->b_ind_unclosed_whiteok = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'm': buf->b_ind_matching_paren = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'M': buf->b_ind_paren_prev = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ ')': buf->b_ind_maxparen = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '*': buf->b_ind_maxcomment = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'g': buf->b_ind_scopedecl = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'h': buf->b_ind_scopedecl_code = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'j': buf->b_ind_java = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'J': buf->b_ind_js = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'l': buf->b_ind_keep_case_label = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ '#': buf->b_ind_hash_comment = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'N': buf->b_ind_cpp_namespace = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'k': buf->b_ind_if_for_while = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'E': buf->b_ind_cpp_extern_c = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	    അവസ്ഥ 'P': buf->b_ind_pragma = (പൂർണ്ണസംഖ്യ)n; നിർത്തുക;
	}
	എങ്കിൽ (*p == ',')
	    ++p;
    }
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_match(പൂർണ്ണസംഖ്യ lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    പൂർണ്ണസംഖ്യ		elselevel;
    പൂർണ്ണസംഖ്യ		whilelevel;

    എങ്കിൽ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    അല്ലെങ്കിൽ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    ആകുമ്പോൾ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	എങ്കിൽ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    എങ്കിൽ (theirscope == ശൂന്യ)
		നിർത്തുക;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    എങ്കിൽ (theirscope->lnum < ourscope)
		നിർത്തുക;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    എങ്കിൽ (theirscope->lnum > ourscope)
		തുടരുക;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    എങ്കിൽ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		എങ്കിൽ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    എങ്കിൽ (!cin_isif(mightbeif))
			++elselevel;
		    തുടരുക;
		}

		// If it's an "if" decrement elselevel
		എങ്കിൽ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    എങ്കിൽ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    എങ്കിൽ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		തുടരുക;
	    }

	    // If it's a "do" decrement whilelevel
	    എങ്കിൽ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    എങ്കിൽ (elselevel <= 0 && whilelevel <= 0)
		തിരികെ OK;
	}
    }
    തിരികെ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    പൂർണ്ണസംഖ്യ
get_c_indent(ശൂന്യം)
{
    pos_T	cur_curpos;
    പൂർണ്ണസംഖ്യ		amount;
    പൂർണ്ണസംഖ്യ		scope_amount;
    പൂർണ്ണസംഖ്യ		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ശൂന്യ;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    പൂർണ്ണസംഖ്യ		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    പൂർണ്ണസംഖ്യ		lookfor;
    പൂർണ്ണസംഖ്യ		whilelevel;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		iscase;
    പൂർണ്ണസംഖ്യ		lookfor_break;
    പൂർണ്ണസംഖ്യ		lookfor_cpp_namespace = FALSE;
    പൂർണ്ണസംഖ്യ		cont_amount = 0;    // amount for continuation line
    പൂർണ്ണസംഖ്യ		original_വരി_islabel;
    പൂർണ്ണസംഖ്യ		added_to_amount = 0;
    പൂർണ്ണസംഖ്യ		js_cur_has_താക്കോൽ = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    പൂർണ്ണസംഖ്യ		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    എങ്കിൽ (cur_curpos.lnum == 1)
	തിരികെ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    എങ്കിൽ (linecopy == ശൂന്യ)
	തിരികെ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    എങ്കിൽ ((State & MODE_തിരുകുക)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_വരി_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    എങ്കിൽ (comment_pos != ശൂന്യ)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    എങ്കിൽ (trypos != ശൂന്യ && (comment_pos == ശൂന്യ
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	പോകുക laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    എങ്കിൽ (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	എങ്കിൽ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    പോകുക theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    എങ്കിൽ (original_വരി_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	പോകുക theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    എങ്കിൽ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_വരി_comment(); // XXX
	എങ്കിൽ (trypos == ശൂന്യ && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    എങ്കിൽ (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	എങ്കിൽ (trypos  != ശൂന്യ)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, ശൂന്യ, ശൂന്യ, 0);
	    amount = col;
	    പോകുക theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    എങ്കിൽ (!cin_iscomment(theline) && comment_pos != ശൂന്യ) // XXX
    {
	പൂർണ്ണസംഖ്യ	lead_start_len = 2;
	പൂർണ്ണസംഖ്യ	lead_നടുവ്_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_നടുവ്[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	പൂർണ്ണസംഖ്യ	lead_end_len;
	char_u	*p;
	പൂർണ്ണസംഖ്യ	start_align = 0;
	പൂർണ്ണസംഖ്യ	start_off = 0;
	പൂർണ്ണസംഖ്യ	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, ശൂന്യ, ശൂന്യ, 0);
	amount = col;
	*lead_start = NUL;
	*lead_നടുവ് = NUL;

	p = curbuf->b_p_com;
	ആകുമ്പോൾ (*p != NUL)
	{
	    പൂർണ്ണസംഖ്യ	align = 0;
	    പൂർണ്ണസംഖ്യ	off = 0;
	    പൂർണ്ണസംഖ്യ what = 0;

	    ആകുമ്പോൾ (*p != NUL && *p != ':')
	    {
		എങ്കിൽ (*p == COM_START || *p == COM_END || *p == COM_നടുവ്)
		    what = *p++;
		അല്ലെങ്കിൽ എങ്കിൽ (*p == COM_ഇടത് || *p == COM_വലത്)
		    align = *p++;
		അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		അല്ലെങ്കിൽ
		    ++p;
	    }

	    എങ്കിൽ (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    എങ്കിൽ (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (what == COM_നടുവ്)
	    {
		STRCPY(lead_നടുവ്, lead_end);
		lead_നടുവ്_len = lead_end_len;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		എങ്കിൽ (STRNCMP(theline, lead_നടുവ്, lead_നടുവ്_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    എങ്കിൽ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			എങ്കിൽ (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(look, lead_നടുവ്,
							lead_നടുവ്_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    നിർത്തുക;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    തുടരുക;
		    }
		    എങ്കിൽ (start_off != 0)
			amount += start_off;
		    അല്ലെങ്കിൽ എങ്കിൽ (start_align == COM_വലത്)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_നടുവ്);
		    നിർത്തുക;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		എങ്കിൽ (STRNCMP(theline, lead_നടുവ്, lead_നടുവ്_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    എങ്കിൽ (off != 0)
			amount += off;
		    അല്ലെങ്കിൽ എങ്കിൽ (align == COM_വലത്)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_നടുവ്);
		    done = TRUE;
		    നിർത്തുക;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	എങ്കിൽ (done)
	    ;
	അല്ലെങ്കിൽ എങ്കിൽ (theline[0] == '*')
	    amount += 1;
	അല്ലെങ്കിൽ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    വേണ്ടി (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		എങ്കിൽ (linewhite(lnum))		    // skip blank lines
		    തുടരുക;
		amount = get_indent_lnum(lnum);	    // XXX
		നിർത്തുക;
	    }
	    എങ്കിൽ (amount == -1)			    // use the comment opener
	    {
		എങ്കിൽ (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    എങ്കിൽ (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, ശൂന്യ, ശൂന്യ, 0);
		amount = col;
		എങ്കിൽ (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	പോകുക theend;
    }

    // Are we looking at a ']' that has a match?
    എങ്കിൽ (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != ശൂന്യ)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	പോകുക theend;
    }

    // Are we inside parentheses or braces?  XXX
    എങ്കിൽ (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ശൂന്യ
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != ശൂന്യ
	    || trypos != ശൂന്യ)
    {
      എങ്കിൽ (trypos != ശൂന്യ && tryposBrace != ശൂന്യ)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  എങ്കിൽ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = ശൂന്യ;
	  അല്ലെങ്കിൽ
	      tryposBrace = ശൂന്യ;
      }

      എങ്കിൽ (trypos != ശൂന്യ)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	എങ്കിൽ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	അല്ലെങ്കിൽ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    വേണ്ടി (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		എങ്കിൽ (cin_nocode(l))		// skip comment lines
		    തുടരുക;
		എങ്കിൽ (cin_ispreproc_cont(&l, &lnum, &amount))
		    തുടരുക;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		എങ്കിൽ ((trypos = ind_find_start_CORS(ശൂന്യ)) != ശൂന്യ)
		{
		    lnum = trypos->lnum + 1;
		    തുടരുക;
		}

		// XXX
		എങ്കിൽ ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != ശൂന്യ
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    എങ്കിൽ (theline[0] == ')')
		    {
			എങ്കിൽ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    നിർത്തുക;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	എങ്കിൽ (amount == -1)
	{
	    പൂർണ്ണസംഖ്യ	    ignore_paren_col = 0;
	    പൂർണ്ണസംഖ്യ	    is_if_for_while = 0;

	    എങ്കിൽ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *വരി;

		trypos = &our_paren_pos;
		ചെയ്യുക
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} ആകുമ്പോൾ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		വരി = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(വരി, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    എങ്കിൽ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *വരി;
		പൂർണ്ണസംഖ്യ	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		വരി = ml_get_curline();
		look_col = (പൂർണ്ണസംഖ്യ)(look - വരി);
		curwin->w_cursor.col = look_col + 1;
		എങ്കിൽ ((trypos = findmatchlimit(ശൂന്യ, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ശൂന്യ
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    എങ്കിൽ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		എങ്കിൽ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    എങ്കിൽ (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			വേണ്ടി (col = 0; col < our_paren_pos.col; ++col)
			{
			    തിരഞ്ഞെടുപ്പ് (l[col])
			    {
				അവസ്ഥ '(':
				അവസ്ഥ '{': ++n;
					  നിർത്തുക;

				അവസ്ഥ ')':
				അവസ്ഥ '}': എങ്കിൽ (n > 1)
					      --n;
					  നിർത്തുക;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    അല്ലെങ്കിൽ
		    {
			col = our_paren_pos.col + 1;
			ആകുമ്പോൾ (VIM_ISWHITE(l[col]))
			    col++;
			എങ്കിൽ (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			അല്ലെങ്കിൽ
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		എങ്കിൽ (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, ശൂന്യ, ശൂന്യ, 0);
		    എങ്കിൽ (cur_amount > (പൂർണ്ണസംഖ്യ)col)
			cur_amount = col;
		}
	    }

	    എങ്കിൽ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		എങ്കിൽ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		ആകുമ്പോൾ ((പൂർണ്ണസംഖ്യ)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    തിരഞ്ഞെടുപ്പ് (*ml_get_pos(&our_paren_pos))
		    {
			അവസ്ഥ '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  നിർത്തുക;
			അവസ്ഥ ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  നിർത്തുക;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		എങ്കിൽ (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		അല്ലെങ്കിൽ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    എങ്കിൽ (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != ശൂന്യ)
			amount += curbuf->b_ind_unclosed2;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			അല്ലെങ്കിൽ
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		എങ്കിൽ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	എങ്കിൽ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      അല്ലെങ്കിൽ
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
	എങ്കിൽ (*look == '{')
	{
	    getvcol(curwin, trypos, &col, ശൂന്യ, ശൂന്യ, 0);
	    amount = col;
	    എങ്കിൽ (*start == '{')
		start_brace = BRACE_IN_COL0;
	    അല്ലെങ്കിൽ
		start_brace = BRACE_AT_START;
	}
	അല്ലെങ്കിൽ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    എങ്കിൽ (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != ശൂന്യ)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    എങ്കിൽ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    അല്ലെങ്കിൽ
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	എങ്കിൽ (curbuf->b_ind_js)
	    js_cur_has_താക്കോൽ = cin_has_js_താക്കോൽ(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	എങ്കിൽ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	അല്ലെങ്കിൽ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    എങ്കിൽ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    അല്ലെങ്കിൽ എങ്കിൽ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    എങ്കിൽ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		എങ്കിൽ (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    പോകുക theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    എങ്കിൽ (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ഇടത്_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    എങ്കിൽ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    അല്ലെങ്കിൽ എങ്കിൽ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		അല്ലെങ്കിൽ
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    എങ്കിൽ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    എങ്കിൽ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    വേണ്ടി (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		എങ്കിൽ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    എങ്കിൽ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			എങ്കിൽ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    എങ്കിൽ (cont_amount > 0)
				amount = cont_amount;
			    അല്ലെങ്കിൽ എങ്കിൽ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    നിർത്തുക;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(ശൂന്യ);
			എങ്കിൽ (trypos != ശൂന്യ)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    തുടരുക;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			എങ്കിൽ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    തുടരുക;

			എങ്കിൽ (cin_nocode(l))
			    തുടരുക;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			എങ്കിൽ (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    എങ്കിൽ (terminated == ',')
				നിർത്തുക;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    എങ്കിൽ (terminated != ';' && cin_isinit())
				നിർത്തുക;

			    // nothing useful found
			    എങ്കിൽ (terminated == 0 || terminated == '{')
				തുടരുക;
			}

			എങ്കിൽ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ശൂന്യ;
			    എങ്കിൽ (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    എങ്കിൽ (trypos == ശൂന്യ && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    എങ്കിൽ (trypos != ശൂന്യ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				തുടരുക;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (lookfor == LOOKFOR_UNTERM)
		    {
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    എങ്കിൽ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			എങ്കിൽ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    എങ്കിൽ (curwin->w_cursor.lnum == ourscope)
				തുടരുക;

			    എങ്കിൽ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				നിർത്തുക;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(ശൂന്യ);
			    എങ്കിൽ (trypos != ശൂന്യ)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				തുടരുക;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    എങ്കിൽ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				തുടരുക;

			    // Finally the actual check for "namespace".
			    എങ്കിൽ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				നിർത്തുക;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				നിർത്തുക;
			    }

			    എങ്കിൽ (cin_nocode(l))
				തുടരുക;
			}
		    }
		    നിർത്തുക;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		എങ്കിൽ ((trypos = ind_find_start_CORS(&raw_string_start)) != ശൂന്യ)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    തുടരുക;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		എങ്കിൽ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    എങ്കിൽ (lookfor == LOOKFOR_CPP_BASECLASS)
			നിർത്തുക;

		    // When looking for a "do" we are not interested in
		    // labels.
		    എങ്കിൽ (whilelevel > 0)
			തുടരുക;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    എങ്കിൽ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
			നിർത്തുക;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    എങ്കിൽ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			എങ്കിൽ ((trypos = find_start_brace()) == ശൂന്യ
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    നിർത്തുക;
			}
			തുടരുക;
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
		    എങ്കിൽ (lookfor == LOOKFOR_TERM)
		    {
			എങ്കിൽ (n)
			    amount = n;

			എങ്കിൽ (!lookfor_break)
			    നിർത്തുക;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    എങ്കിൽ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			എങ്കിൽ (l != ശൂന്യ && cin_is_cinword(l))
			{
			    എങ്കിൽ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    അല്ലെങ്കിൽ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			നിർത്തുക;
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
		    തുടരുക;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		എങ്കിൽ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    എങ്കിൽ (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != ശൂന്യ)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    തുടരുക;
		}

		// Ignore jump labels with nothing after them.
		എങ്കിൽ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    എങ്കിൽ (l == ശൂന്യ || cin_nocode(l))
			തുടരുക;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		എങ്കിൽ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    തുടരുക;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		എങ്കിൽ (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		എങ്കിൽ (n)
		{
		    എങ്കിൽ (lookfor == LOOKFOR_UNTERM)
		    {
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			തുടരുക;
		    }
		    അല്ലെങ്കിൽ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    എങ്കിൽ (cin_isterminated(l, TRUE, FALSE))
			നിർത്തുക;
		    അല്ലെങ്കിൽ
			തുടരുക;
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

		എങ്കിൽ (js_cur_has_താക്കോൽ)
		{
		    js_cur_has_താക്കോൽ = 0; // only check the first line
		    എങ്കിൽ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_താക്കോൽ;
		    }
		}
		എങ്കിൽ (lookfor == LOOKFOR_JS_താക്കോൽ && cin_has_js_താക്കോൽ(l))
		{
		    amount = get_indent();
		    നിർത്തുക;
		}
		എങ്കിൽ (lookfor == LOOKFOR_COMMA)
		{
		    എങ്കിൽ (tryposBrace != ശൂന്യ && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			നിർത്തുക;
		    എങ്കിൽ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			നിർത്തുക;
		    അല്ലെങ്കിൽ
		    {
			amount = get_indent();
			എങ്കിൽ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    നിർത്തുക;
		    }
		}

		എങ്കിൽ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    എങ്കിൽ (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (ശൂന്യം)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    എങ്കിൽ (trypos != ശൂന്യ && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = ശൂന്യ;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    എങ്കിൽ (trypos == ശൂന്യ && terminated == ',')
		    {
			എങ്കിൽ (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    എങ്കിൽ (trypos != ശൂന്യ)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			എങ്കിൽ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    തുടരുക;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    എങ്കിൽ (terminated == ',')
		    {
			ആകുമ്പോൾ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    എങ്കിൽ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				നിർത്തുക;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    എങ്കിൽ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    അല്ലെങ്കിൽ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    എങ്കിൽ (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			എങ്കിൽ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			എങ്കിൽ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    തുടരുക;
			}
			നിർത്തുക;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    എങ്കിൽ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			എങ്കിൽ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    എങ്കിൽ (cont_amount > 0)
				amount = cont_amount;
			    അല്ലെങ്കിൽ
				amount += ind_continuation;
			    നിർത്തുക;
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
			എങ്കിൽ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			എങ്കിൽ (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    നിർത്തുക;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			എങ്കിൽ (cin_isdo(l))
			{
			    എങ്കിൽ (whilelevel == 0)
				നിർത്തുക;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			എങ്കിൽ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    എങ്കിൽ (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    എങ്കിൽ ((trypos = find_start_brace()) == ശൂന്യ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				നിർത്തുക;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    അല്ലെങ്കിൽ
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			എങ്കിൽ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    എങ്കിൽ (terminated == ',')
				amount += ind_continuation;
			    നിർത്തുക;
			}

			എങ്കിൽ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    എങ്കിൽ (terminated == ',')
			    {
				എങ്കിൽ (curbuf->b_ind_cpp_baseclass == 0)
				    നിർത്തുക;

				lookfor = LOOKFOR_CPP_BASECLASS;
				തുടരുക;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    എങ്കിൽ (amount > cur_amount)
				amount = cur_amount;
			}
			അല്ലെങ്കിൽ
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (പൂർണ്ണസംഖ്യ)STRLEN(l);
			    എങ്കിൽ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				നിർത്തുക;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    എങ്കിൽ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				എങ്കിൽ (curbuf->b_ind_js)
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
				    എങ്കിൽ (cin_iscomment(skipwhite(l)))
					നിർത്തുക;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    എങ്കിൽ (trypos != ശൂന്യ)
				    {
					എങ്കിൽ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    നിർത്തുക;
					}
					ourscope = trypos->lnum;
				    }
				}
				അല്ലെങ്കിൽ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    അല്ലെങ്കിൽ
			    {
				എങ്കിൽ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				എങ്കിൽ (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_താക്കോൽ
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		അല്ലെങ്കിൽ എങ്കിൽ (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    എങ്കിൽ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
			നിർത്തുക;
		    }

		    എങ്കിൽ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			എങ്കിൽ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		അല്ലെങ്കിൽ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    എങ്കിൽ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			തുടരുക;
		    }

		    // Handle "do {" line.
		    എങ്കിൽ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			എങ്കിൽ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    തുടരുക;
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
		    എങ്കിൽ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			എങ്കിൽ (cont_amount > 0)
			    amount = cont_amount;
			അല്ലെങ്കിൽ
			    amount += ind_continuation;
			നിർത്തുക;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    എങ്കിൽ (lookfor == LOOKFOR_TERM)
		    {
			എങ്കിൽ (!lookfor_break && whilelevel == 0)
			    നിർത്തുക;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    അല്ലെങ്കിൽ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			എങ്കിൽ (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != ശൂന്യ)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    എങ്കിൽ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				തുടരുക;
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

			എങ്കിൽ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			എങ്കിൽ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			എങ്കിൽ (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    എങ്കിൽ ((trypos = find_start_brace()) == ശൂന്യ
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				നിർത്തുക;
			    തുടരുക;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			എങ്കിൽ (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != ശൂന്യ)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    എങ്കിൽ (*l == '}' || !cin_iselse(l))
				പോകുക term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      എങ്കിൽ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      എങ്കിൽ (curbuf->b_ind_jump_label > 0 && original_വരി_islabel)
	  amount -= curbuf->b_ind_jump_label;

      പോകുക theend;
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

    എങ്കിൽ (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	പോകുക theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    എങ്കിൽ (cur_curpos.lnum < curbuf->b_ml.ml_വരി_എണ്ണം
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ശൂന്യ
	    && vim_strchr(theline, '}') == ശൂന്യ
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(ശൂന്യ, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	പോകുക theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ആകുമ്പോൾ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	എങ്കിൽ ((trypos = ind_find_start_CORS(ശൂന്യ)) != ശൂന്യ)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    തുടരുക;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	എങ്കിൽ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	എങ്കിൽ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    നിർത്തുക;
	}

	// Skip preprocessor directives and blank lines.
	എങ്കിൽ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    തുടരുക;

	എങ്കിൽ (cin_nocode(l))
	    തുടരുക;

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
	എങ്കിൽ (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    എങ്കിൽ (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != ശൂന്യ)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ആകുമ്പോൾ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		എങ്കിൽ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    നിർത്തുക;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    എങ്കിൽ (amount == 0)
		amount = cin_first_id_amount();
	    എങ്കിൽ (amount == 0)
		amount = ind_continuation;
	    നിർത്തുക;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	എങ്കിൽ (cin_isfuncdecl(ശൂന്യ, cur_curpos.lnum, 0))  // XXX
	    നിർത്തുക;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	എങ്കിൽ (*skipwhite(l) == '}')
	    നിർത്തുക;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	എങ്കിൽ (cin_ends_in(l, (char_u *)"};"))
	    നിർത്തുക;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	എങ്കിൽ (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    നിർത്തുക;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	എങ്കിൽ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ആകുമ്പോൾ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		എങ്കിൽ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    നിർത്തുക;
	    }
	    എങ്കിൽ (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		നിർത്തുക;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	എങ്കിൽ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    നിർത്തുക;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	എങ്കിൽ (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    എങ്കിൽ (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		നിർത്തുക;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	എങ്കിൽ ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ശൂന്യ)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	നിർത്തുക;
    }

    // add extra indent for a comment
    എങ്കിൽ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    എങ്കിൽ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	എങ്കിൽ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    എങ്കിൽ (cur_amount > 0)
		amount = cur_amount;
	    അല്ലെങ്കിൽ എങ്കിൽ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    എങ്കിൽ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    തിരികെ amount;
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
    പൂർണ്ണസംഖ്യ
in_cinkeys(
    പൂർണ്ണസംഖ്യ		keytyped,
    പൂർണ്ണസംഖ്യ		when,
    പൂർണ്ണസംഖ്യ		വരി_is_empty)
{
    char_u	*look;
    പൂർണ്ണസംഖ്യ		try_match;
    പൂർണ്ണസംഖ്യ		try_match_word;
    char_u	*p;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		icase;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	തിരികെ FALSE;

#ifdef FEAT_EVAL
    എങ്കിൽ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    അല്ലെങ്കിൽ
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ആകുമ്പോൾ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	തിരഞ്ഞെടുപ്പ് (when)
	{
	    അവസ്ഥ '*': try_match = (*look == '*'); നിർത്തുക;
	    അവസ്ഥ '!': try_match = (*look == '!'); നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: try_match = (*look != '*'); നിർത്തുക;
	}
	എങ്കിൽ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	എങ്കിൽ (*look == '0')
	{
	    try_match_word = try_match;
	    എങ്കിൽ (!വരി_is_empty)
		try_match = FALSE;
	    ++look;
	}
	അല്ലെങ്കിൽ
	    try_match_word = FALSE;

	// does it look like a control character?
	എങ്കിൽ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    എങ്കിൽ (try_match && keytyped == Ctrl_chr(look[1]))
		തിരികെ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	അല്ലെങ്കിൽ എങ്കിൽ (*look == 'o')
	{
	    എങ്കിൽ (try_match && keytyped == താക്കോൽ_OPEN_FORW)
		തിരികെ TRUE;
	    ++look;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*look == 'O')
	{
	    എങ്കിൽ (try_match && keytyped == താക്കോൽ_OPEN_BACK)
		തിരികെ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	അല്ലെങ്കിൽ എങ്കിൽ (*look == 'e')
	{
	    എങ്കിൽ (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		എങ്കിൽ (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    തിരികെ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	അല്ലെങ്കിൽ എങ്കിൽ (*look == ':')
	{
	    എങ്കിൽ (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		എങ്കിൽ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    തിരികെ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		എങ്കിൽ (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    എങ്കിൽ (i)
			തിരികെ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	അല്ലെങ്കിൽ എങ്കിൽ (*look == '<')
	{
	    എങ്കിൽ (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		എങ്കിൽ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ശൂന്യ
						       && keytyped == look[1])
		    തിരികെ TRUE;

		എങ്കിൽ (keytyped == get_special_താക്കോൽ_code(look + 1))
		    തിരികെ TRUE;
	    }
	    ആകുമ്പോൾ (*look && *look != '>')
		look++;
	    ആകുമ്പോൾ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	അല്ലെങ്കിൽ എങ്കിൽ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    എങ്കിൽ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    അല്ലെങ്കിൽ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    എങ്കിൽ (p == ശൂന്യ)
		p = look + STRLEN(look);
	    എങ്കിൽ ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		പൂർണ്ണസംഖ്യ		match = FALSE;

		എങ്കിൽ (keytyped == താക്കോൽ_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    വരി = ml_get_curline();
		    എങ്കിൽ (has_mbyte)
		    {
			char_u	*n;

			വേണ്ടി (s = വരി + curwin->w_cursor.col; s > വരി; s = n)
			{
			    n = mb_prevptr(വരി, s);
			    എങ്കിൽ (!vim_iswordp(n))
				നിർത്തുക;
			}
		    }
		    അല്ലെങ്കിൽ
			വേണ്ടി (s = വരി + curwin->w_cursor.col; s > വരി; --s)
			    എങ്കിൽ (!vim_iswordc(s[-1]))
				നിർത്തുക;
		    എങ്കിൽ (s + (p - look) <= വരി + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		അല്ലെങ്കിൽ
		    // TODO: multi-byte
		    എങ്കിൽ (keytyped == (പൂർണ്ണസംഖ്യ)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((പൂർണ്ണസംഖ്യ)p[-1])))
		{
		    വരി = ml_get_cursor();
		    എങ്കിൽ ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(വരി[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(വരി - (p - look), look, p - look)
				: STRNCMP(വരി - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		എങ്കിൽ (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    എങ്കിൽ (getwhitecols_curline() !=
				     (പൂർണ്ണസംഖ്യ)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		എങ്കിൽ (match)
		    തിരികെ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (try_match && *look == keytyped)
		തിരികെ TRUE;
	    എങ്കിൽ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    തിരികെ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    ശൂന്യം
do_c_വ്യഞ്ജകം_indent(ശൂന്യം)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (*curbuf->b_p_inde != NUL)
	fixthisline(get_വ്യഞ്ജകം_indent);
    അല്ലെങ്കിൽ
#endif
	fixthisline(get_c_indent);
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    ശൂന്യം
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (lnum >= 1 && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_സംഖ്യ = get_c_indent();
	curwin->w_cursor = pos;
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = -1;
}
#endif
