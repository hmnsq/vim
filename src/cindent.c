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

#અન્તર્ભાવ "vim.h"

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
#define LOOKFOR_JS_ચાવી		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    પૂર્ણાંક
cin_is_cinword(char_u *લીટી)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    પૂર્ણાંક		cinw_len;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		len;

    cinw_len = (પૂર્ણાંક)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    જો (cinw_buf == ખાલી)
	પાછા FALSE;

    લીટી = skipwhite(લીટી);
    માટે (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	જો (STRNCMP(લીટી, cinw_buf, len) == 0
		&& (!vim_iswordc(લીટી[len]) || !vim_iswordc(લીટી[len - 1])))
	{
	    retval = TRUE;
	    તોડો;
	}
    }
    vim_free(cinw_buf);
    પાછા retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    સ્થિર char_u *
skip_string(char_u *p)
{
    પૂર્ણાંક	    i;

    // We loop, because strings may be concatenated: "date""time".
    માટે ( ; ; ++p)
    {
	જો (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    જો (p[1] == NUL)		    // ' at end of line
		તોડો;
	    i = 2;
	    જો (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		જ્યારે (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    જો (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		ચાલુ;
	    }
	}
	નહીંતર જો (p[0] == '"')		    // start of string
	{
	    માટે (++p; p[0]; ++p)
	    {
		જો (p[0] == '\\' && p[1] != NUL)
		    ++p;
		નહીંતર જો (p[0] == '"')	    // end of string
		    તોડો;
	    }
	    જો (p[0] == '"')
		ચાલુ; // continue for another string
	}
	નહીંતર જો (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    જો (paren != ખાલી)
	    {
		size_t delim_len = paren - delim;

		માટે (p += 3; *p; ++p)
		    જો (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			તોડો;
		    }
		જો (p[0] == '"')
		    ચાલુ; // continue for another string
	    }
	}
	તોડો;				    // no string found
    }
    જો (!*p)
	--p;				    // backup from NUL
    પાછા p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    પૂર્ણાંક
is_pos_in_string(char_u *લીટી, colnr_T col)
{
    char_u *p;

    માટે (p = લીટી; *p && (colnr_T)(p - લીટી) < col; ++p)
	p = skip_string(p);
    પાછા !((colnr_T)(p - લીટી) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    પૂર્ણાંક
check_linecomment(char_u *લીટી)
{
    char_u  *p;

    p = લીટી;
    // skip Lispish one-line comments
    જો (curbuf->b_p_lisp)
    {
	જો (vim_strchr(p, ';') != ખાલી) // there may be comments
	{
	    પૂર્ણાંક in_str = FALSE;	// inside of string

	    p = લીટી;		// scan from start
	    જ્યારે ((p = vim_strpbrk(p, (char_u *)"\";")) != ખાલી)
	    {
		જો (*p == '"')
		{
		    જો (in_str)
		    {
			જો (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    નહીંતર જો (p == લીટી || ((p - લીટી) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		નહીંતર જો (!in_str && ((p - લીટી) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(લીટી, (colnr_T)(p - લીટી)))
		    તોડો;	// found!
		++p;
	    }
	}
	નહીંતર
	    p = ખાલી;
    }
    નહીંતર
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	માટે ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    જો (*p == NUL)
		તોડો;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    જો (p[0] == '/' && p[1] == '/'
				 && (p == લીટી || p[-1] != '*' || p[2] != '*'))
		તોડો;
	}
	જો (*p == NUL)
	    p = ખાલી;
    }

    જો (p == ખાલી)
	પાછા MAXCOL;
    પાછા (પૂર્ણાંક)(p - લીટી);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    સ્થિર pos_T *
ind_find_start_comment(શૂન્ય)	    // XXX
{
    પાછા find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(પૂર્ણાંક ind_maxcomment)	// XXX
{
    pos_T	*pos;
    પૂર્ણાંક		cur_maxcomment = ind_maxcomment;

    માટે (;;)
    {
	pos = findmatchlimit(ખાલી, '*', FM_BACKWARD, cur_maxcomment);
	જો (pos == ખાલી)
	    તોડો;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	જો (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    તોડો;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	જો (cur_maxcomment <= 0)
	{
	    pos = ખાલી;
	    તોડો;
	}
    }
    પાછા pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    સ્થિર pos_T *
find_start_rawstring(પૂર્ણાંક ind_maxcomment)	// XXX
{
    pos_T	*pos;
    પૂર્ણાંક		cur_maxcomment = ind_maxcomment;

    માટે (;;)
    {
	pos = findmatchlimit(ખાલી, 'R', FM_BACKWARD, cur_maxcomment);
	જો (pos == ખાલી)
	    તોડો;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	જો (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    તોડો;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	જો (cur_maxcomment <= 0)
	{
	    pos = ખાલી;
	    તોડો;
	}
    }
    પાછા pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    સ્થિર pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    સ્થિર pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    જો (comment_pos != ખાલી)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    જો (comment_pos == ખાલી || (rs_pos != ખાલી
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	જો (is_raw != ખાલી && rs_pos != ખાલી)
	    *is_raw = rs_pos->lnum;
	પાછા rs_pos;
    }
    પાછા comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    પૂર્ણાંક
cindent_on(શૂન્ય)
{
    પાછા (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    સ્થિર char_u *
cin_skipcomment(char_u *s)
{
    જ્યારે (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	જો (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    તોડો;
	}
	જો (*s != '/')
	    તોડો;
	++s;
	જો (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    તોડો;
	}
	જો (*s != '*')
	    તોડો;
	માટે (++s; *s; ++s)	// skip slash-star comment
	    જો (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		તોડો;
	    }
    }
    પાછા s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    સ્થિર પૂર્ણાંક
cin_nocode(char_u *s)
{
    પાછા *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    સ્થિર પૂર્ણાંક
cin_iscomment(char_u *p)
{
    પાછા (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    સ્થિર પૂર્ણાંક
cin_islinecomment(char_u *p)
{
    પાછા (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    સ્થિર pos_T *
find_લીટી_comment(શૂન્ય) // XXX
{
    સ્થિર pos_T pos;
    char_u	 *લીટી;
    char_u	 *p;

    pos = curwin->w_cursor;
    જ્યારે (--pos.lnum > 0)
    {
	લીટી = ml_get(pos.lnum);
	p = skipwhite(લીટી);
	જો (cin_islinecomment(p))
	{
	    pos.col = (પૂર્ણાંક)(p - લીટી);
	    પાછા &pos;
	}
	જો (*p != NUL)
	    તોડો;
    }
    પાછા ખાલી;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    સ્થિર પૂર્ણાંક
cin_has_js_ચાવી(char_u *text)
{
    char_u *s = skipwhite(text);
    પૂર્ણાંક	    quote = -1;

    જો (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    જો (!vim_isIDc(*s))	    // need at least one ID character
	પાછા FALSE;

    જ્યારે (vim_isIDc(*s))
	++s;
    જો (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    પાછા (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    સ્થિર પૂર્ણાંક
cin_islabel_skip(char_u **s)
{
    જો (!vim_isIDc(**s))	    // need at least one ID character
	પાછા FALSE;

    જ્યારે (vim_isIDc(**s))
    {
	જો (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	નહીંતર
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    પાછા (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    સ્થિર પૂર્ણાંક
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    પૂર્ણાંક	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    જો (cinsd_buf == ખાલી)
	પાછા FALSE;

    માટે (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (પૂર્ણાંક)cinsd_len, ",");
	જો (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    જો (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		તોડો;
	    }
	}
    }

    vim_free(cinsd_buf);
    પાછા found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    સ્થિર પૂર્ણાંક
cin_ispreproc(char_u *s)
{
    જો (*skipwhite(s) == '#')
	પાછા TRUE;
    પાછા FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    સ્થિર પૂર્ણાંક
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, પૂર્ણાંક *amount)
{
    char_u	*લીટી = *pp;
    linenr_T	lnum = *lnump;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		candidate_amount = *amount;

    જો (*લીટી != NUL && લીટી[STRLEN(લીટી) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    માટે (;;)
    {
	જો (cin_ispreproc(લીટી))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    તોડો;
	}
	જો (lnum == 1)
	    તોડો;
	લીટી = ml_get(--lnum);
	જો (*લીટી == NUL || લીટી[STRLEN(લીટી) - 1] != '\\')
	    તોડો;
    }

    જો (lnum != *lnump)
	*pp = ml_get(*lnump);
    જો (retval)
	*amount = candidate_amount;
    પાછા retval;
}

    સ્થિર પૂર્ણાંક
cin_iselse(
    char_u  *p)
{
    જો (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    પાછા (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    સ્થિર પૂર્ણાંક
cin_isterminated(
    char_u	*s,
    પૂર્ણાંક		incl_open,	// include '{' at the end as terminator
    પૂર્ણાંક		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    પૂર્ણાંક		is_else = FALSE;

    s = cin_skipcomment(s);

    જો (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    જો (!found_start)
	is_else = cin_iselse(s);

    જ્યારે (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	જો (*s == '}' && n_open > 0)
	    --n_open;
	જો ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    પાછા *s;
	નહીંતર જો (*s == '{')
	{
	    જો (incl_open && cin_nocode(s + 1))
		પાછા *s;
	    નહીંતર
		++n_open;
	}

	જો (*s)
	    s++;
    }
    પાછા found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    સ્થિર પૂર્ણાંક
cin_starts_with(char_u *s, અક્ષર *word)
{
    પૂર્ણાંક l = (પૂર્ણાંક)STRLEN(word);

    પાછા (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    સ્થિર પૂર્ણાંક
cin_isdefault(char_u *s)
{
    પાછા (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    સ્થિર પૂર્ણાંક
cin_iscase(
    char_u *s,
    પૂર્ણાંક strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    જો (cin_starts_with(s, "case"))
    {
	માટે (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    જો (*s == NUL)
		તોડો;
	    જો (*s == ':')
	    {
		જો (s[1] == ':')	// skip over "::" for C++
		    ++s;
		નહીંતર
		    પાછા TRUE;
	    }
	    જો (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    નહીંતર જો (*s == '/' && (s[1] == '*' || s[1] == '/'))
		પાછા FALSE;		// stop at comment
	    નહીંતર જો (*s == '"')
	    {
		// JS etc.
		જો (strict)
		    પાછા FALSE;		// stop at string
		નહીંતર
		    પાછા TRUE;
	    }
	}
	પાછા FALSE;
    }

    જો (cin_isdefault(s))
	પાછા TRUE;
    પાછા FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    સ્થિર પૂર્ણાંક
cin_islabel(શૂન્ય)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    જો (cin_isdefault(s))
	પાછા FALSE;
    જો (cin_isscopedecl(s))
	પાછા FALSE;

    જો (!cin_islabel_skip(&s))
	પાછા FALSE;

    જો (ind_find_start_CORS(ખાલી))
	પાછા FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*લીટી;

    cursor_save = curwin->w_cursor;
    જ્યારે (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	જો ((trypos = ind_find_start_CORS(ખાલી)) != ખાલી) // XXX
	    curwin->w_cursor = *trypos;

	લીટી = ml_get_curline();
	જો (cin_ispreproc(લીટી))	// ignore #defines, #if, etc.
	    ચાલુ;
	જો (*(લીટી = cin_skipcomment(લીટી)) == NUL)
	    ચાલુ;

	curwin->w_cursor = cursor_save;
	જો (cin_isterminated(લીટી, TRUE, FALSE)
		|| cin_isscopedecl(લીટી)
		|| cin_iscase(લીટી, TRUE)
		|| (cin_islabel_skip(&લીટી) && cin_nocode(લીટી)))
	    પાછા TRUE;
	પાછા FALSE;
    }
    curwin->w_cursor = cursor_save;
    પાછા TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    સ્થિર પૂર્ણાંક
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(find);

    જ્યારે (*p != NUL)
    {
	p = cin_skipcomment(p);
	જો (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    જો (cin_nocode(r))
		પાછા TRUE;
	}
	જો (*p != NUL)
	    ++p;
    }
    પાછા FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    સ્થિર char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = ખાલી, *p = s;
    કરો
    {
	r = p;
	p = cin_skipcomment(p);
	જો (*p)
	    p = skip_string(p);
    } જ્યારે (p != r);
    પાછા p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    સ્થિર પૂર્ણાંક
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = ખાલી;

    જ્યારે (*p)
    {
	જો (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	નહીંતર જો (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	નહીંતર
	    p = cin_skip_comment_and_string(p + 1);
    }
    જો (!r)
	પાછા FALSE;
    p = r; // p points now after '=' or "return"

    જો (cin_nocode(p))
	પાછા TRUE;

    જો (*p == '&')
	p = cin_skipcomment(p + 1);

    જો (*p == '(') // skip a typecast
    {
	પૂર્ણાંક open_ગણતરી = 1;
	કરો
	{
	    p = cin_skip_comment_and_string(p + 1);
	    જો (cin_nocode(p))
		પાછા TRUE;
	    open_ગણતરી += (*p == '(') - (*p == ')');
	} જ્યારે (open_ગણતરી);
	p = cin_skipcomment(p + 1);
	જો (cin_nocode(p))
	    પાછા TRUE;
    }

    જ્યારે (*p == '{')
	p = cin_skipcomment(p + 1);
    પાછા cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    સ્થિર પૂર્ણાંક
cin_isinit(શૂન્ય)
{
    char_u	*s;
    સ્થિર અક્ષર *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    જો (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    માટે (;;)
    {
	પૂર્ણાંક i, l;

	માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(skip); ++i)
	{
	    l = (પૂર્ણાંક)strlen(skip[i]);
	    જો (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		તોડો;
	    }
	}
	જો (l != 0)
	    તોડો;
    }

    જો (cin_starts_with(s, "enum"))
	પાછા TRUE;

    પાછા cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    સ્થિર પૂર્ણાંક
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    પૂર્ણાંક		has_name = FALSE;
    પૂર્ણાંક		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    જ્યારે ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    જો (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	જ્યારે (*p != NUL)
	{
	    જો (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    નહીંતર જો (*p == '{')
	    {
		તોડો;
	    }
	    નહીંતર જો (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		જો (has_name)
		    પાછા FALSE; // word character after skipping past name
		++p;
	    }
	    નહીંતર જો (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		જો (!has_name_start || has_name)
		    પાછા FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    નહીંતર
	    {
		પાછા FALSE;
	    }
	}
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    સ્થિર પૂર્ણાંક
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    પૂર્ણાંક		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    જો (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	જ્યારે (*p != NUL)
	{
	    જો (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    નહીંતર જો (*p == '{')
	    {
		તોડો;
	    }
	    નહીંતર જો (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		જો (has_string_literal)
		    પાછા FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    નહીંતર જો (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		જો (has_string_literal)
		    પાછા FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    નહીંતર
	    {
		પાછા FALSE;
	    }
	}
	પાછા has_string_literal ? TRUE : FALSE;
    }
    પાછા FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    સ્થિર char_u *
after_label(char_u *l)
{
    માટે ( ; *l; ++l)
    {
	જો (*l == ':')
	{
	    જો (l[1] == ':')	    // skip over "::" for C++
		++l;
	    નહીંતર જો (!cin_iscase(l + 1, FALSE))
		તોડો;
	}
	નહીંતર જો (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    જો (*l == NUL)
	પાછા ખાલી;
    l = cin_skipcomment(l + 1);
    જો (*l == NUL)
	પાછા ખાલી;
    પાછા l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    સ્થિર પૂર્ણાંક
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    જો (p == ખાલી)
	પાછા 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, ખાલી, ખાલી, 0);
    પાછા (પૂર્ણાંક)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    સ્થિર પૂર્ણાંક
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    પૂર્ણાંક		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    જો (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	જો (l == ખાલી)		// just in case
	    l = ml_get_curline();
    }
    નહીંતર
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    પાછા amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    સ્થિર પૂર્ણાંક
cin_first_id_amount(શૂન્ય)
{
    char_u	*લીટી, *p, *s;
    પૂર્ણાંક		len;
    pos_T	fp;
    colnr_T	col;

    લીટી = ml_get_curline();
    p = skipwhite(લીટી);
    len = (પૂર્ણાંક)(skiptowhite(p) - p);
    જો (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (પૂર્ણાંક)(skiptowhite(p) - p);
    }
    જો (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    નહીંતર જો (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    નહીંતર જો ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	જો ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    માટે (len = 0; vim_isIDc(p[len]); ++len)
	;
    જો (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	પાછા 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - લીટી);
    getvcol(curwin, &fp, &col, ખાલી, ખાલી, 0);
    પાછા (પૂર્ણાંક)col;
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
    સ્થિર પૂર્ણાંક
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*લીટી;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    જો (lnum > 1)
    {
	લીટી = ml_get(lnum - 1);
	જો (*લીટી != NUL && લીટી[STRLEN(લીટી) - 1] == '\\')
	    પાછા -1;
    }

    લીટી = s = ml_get(lnum);
    જ્યારે (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == ખાલી)
    {
	જો (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	નહીંતર
	    ++s;
    }
    જો (*s != '=')
	પાછા 0;

    s = skipwhite(s + 1);
    જો (cin_nocode(s))
	પાછા 0;

    જો (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - લીટી);
    getvcol(curwin, &fp, &col, ખાલી, ખાલી, 0);
    પાછા (પૂર્ણાંક)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    સ્થિર પૂર્ણાંક
cin_skip2pos(pos_T *trypos)
{
    char_u	*લીટી;
    char_u	*p;
    char_u	*new_p;

    p = લીટી = ml_get(trypos->lnum);
    જ્યારે (*p && (colnr_T)(p - લીટી) < trypos->col)
    {
	જો (cin_iscomment(p))
	    p = cin_skipcomment(p);
	નહીંતર
	{
	    new_p = skip_string(p);
	    જો (new_p == p)
		++p;
	    નહીંતર
		p = new_p;
	}
    }
    પાછા (પૂર્ણાંક)(p - લીટી);
}

    સ્થિર pos_T *
find_match_char(પૂર્ણાંક c, પૂર્ણાંક ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    સ્થિર pos_T pos_copy;
    પૂર્ણાંક		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    જો ((trypos = findmatchlimit(ખાલી, c, FM_SKIPCOMM, ind_maxp_wk)) != ખાલી)
    {
	// check if the ( is in a // comment
	જો ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (પૂર્ણાંક)(cursor_save.lnum - trypos->lnum);
	    જો (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		જાઓ retry;
	    }
	    trypos = ખાલી;
	}
	નહીંતર
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    જો ((trypos_wk = ind_find_start_CORS(ખાલી)) != ખાલી) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (પૂર્ણાંક)(cursor_save.lnum
			- trypos_wk->lnum);
		જો (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    જાઓ retry;
		}
		trypos = ખાલી;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    પાછા trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    સ્થિર pos_T *
find_match_paren(પૂર્ણાંક ind_maxparen)	// XXX
{
    પાછા find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    સ્થિર પૂર્ણાંક
find_last_paren(char_u *l, પૂર્ણાંક start, પૂર્ણાંક end)
{
    પૂર્ણાંક		i;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		open_ગણતરી = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    માટે (i = 0; l[i] != NUL; i++)
    {
	i = (પૂર્ણાંક)(cin_skipcomment(l + i) - l); // ignore parens in comments
	જો (l[i] == NUL)
	    તોડો;
	i = (પૂર્ણાંક)(skip_string(l + i) - l);    // ignore parens in quotes
	જો (l[i] == start)
	    ++open_ગણતરી;
	નહીંતર જો (l[i] == end)
	{
	    જો (open_ગણતરી > 0)
		--open_ગણતરી;
	    નહીંતર
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    પાછા retval;
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
    સ્થિર પૂર્ણાંક
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    પૂર્ણાંક		retval = FALSE;
    pos_T	*trypos;
    પૂર્ણાંક		just_started = TRUE;

    જો (sp == ખાલી)
	s = ml_get(lnum);
    નહીંતર
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    જો (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ખાલી)
    {
	lnum = trypos->lnum;
	જો (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    પાછા FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    જો (cin_ispreproc(s))
	પાછા FALSE;

    જ્યારે (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	જો (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	નહીંતર જો (*s == ':')
	{
	    જો (*(s + 1) == ':')
		s += 2;
	    નહીંતર
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		પાછા FALSE;
	}
	નહીંતર
	    ++s;
    }
    જો (*s != '(')
	પાછા FALSE;		// ';', ' or "  before any () or no '('

    જ્યારે (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	જો (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    જો (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    જાઓ done;
	}
	જો ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    પૂર્ણાંક comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    માટે (;;)
	    {
		જો (lnum >= curbuf->b_ml.ml_લીટી_ગણતરી)
		    તોડો;
		s = ml_get(++lnum);
		જો (!cin_ispreproc(s))
		    તોડો;
	    }
	    જો (lnum >= curbuf->b_ml.ml_લીટી_ગણતરી)
		તોડો;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    જો (!just_started && (!comma && *s != ',' && *s != ')'))
		તોડો;
	    just_started = FALSE;
	}
	નહીંતર જો (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	નહીંતર
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    જો (lnum != first_lnum && sp != ખાલી)
	*sp = ml_get(first_lnum);

    પાછા retval;
}

    સ્થિર પૂર્ણાંક
cin_isif(char_u *p)
{
    પાછા (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    સ્થિર પૂર્ણાંક
cin_isdo(char_u *p)
{
    પાછા (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    સ્થિર પૂર્ણાંક
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    પૂર્ણાંક		retval = FALSE;

    p = cin_skipcomment(p);
    જો (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    જો (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	જ્યારે (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	જો ((trypos = findmatchlimit(ખાલી, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != ખાલી
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    પાછા retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    સ્થિર પૂર્ણાંક
cin_is_if_for_while_before_offset(char_u *લીટી, પૂર્ણાંક *poffset)
{
    પૂર્ણાંક offset = *poffset;

    જો (offset-- < 2)
	પાછા 0;
    જ્યારે (offset > 2 && VIM_ISWHITE(લીટી[offset]))
	--offset;

    offset -= 1;
    જો (STRNCMP(લીટી + offset, "if", 2) == 0)
	જાઓ probablyFound;

    જો (offset >= 1)
    {
	offset -= 1;
	જો (STRNCMP(લીટી + offset, "for", 3) == 0)
	    જાઓ probablyFound;

	જો (offset >= 2)
	{
	    offset -= 2;
	    જો (STRNCMP(લીટી + offset, "while", 5) == 0)
		જાઓ probablyFound;
	}
    }
    પાછા 0;

probablyFound:
    જો (!offset || !vim_isIDc(લીટી[offset - 1]))
    {
	*poffset = offset;
	પાછા 1;
    }
    પાછા 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    સ્થિર પૂર્ણાંક
cin_iswhileofdo_end(પૂર્ણાંક terminated)
{
    char_u	*લીટી;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    પૂર્ણાંક		i;

    જો (terminated != ';')	// there must be a ';' at the end
	પાછા FALSE;

    p = લીટી = ml_get_curline();
    જ્યારે (*p != NUL)
    {
	p = cin_skipcomment(p);
	જો (*p == ')')
	{
	    s = skipwhite(p + 1);
	    જો (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (પૂર્ણાંક)(p - લીટી);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		જો (trypos != ખાલી)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    જો (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    જો (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			પાછા TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		લીટી = ml_get_curline();
		p = લીટી + i;
	    }
	}
	જો (*p != NUL)
	    ++p;
    }
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
cin_isbreak(char_u *p)
{
    પાછા (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    સ્થિર પૂર્ણાંક
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    પૂર્ણાંક		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*લીટી = ml_get_curline();

    જો (pos->lnum <= lnum)
	પાછા cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(લીટી);
    જો (*s == '#')		// skip #define FOO x ? (x) : x
	પાછા FALSE;
    s = cin_skipcomment(s);
    જો (*s == NUL)
	પાછા FALSE;

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
    જ્યારે (lnum > 1)
    {
	લીટી = ml_get(lnum - 1);
	s = skipwhite(લીટી);
	જો (*s == '#' || *s == NUL)
	    તોડો;
	જ્યારે (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    જો (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		તોડો;
	    જો (*s != NUL)
		++s;
	}
	જો (*s != NUL)
	    તોડો;
	--lnum;
    }

    pos->lnum = lnum;
    લીટી = ml_get(lnum);
    s = લીટી;
    માટે (;;)
    {
	જો (*s == NUL)
	{
	    જો (lnum == curwin->w_cursor.lnum)
		તોડો;
	    // Continue in the cursor line.
	    લીટી = ml_get(++lnum);
	    s = લીટી;
	}
	જો (s == લીટી)
	{
	    // don't recognize "case (foo):" as a baseclass
	    જો (cin_iscase(s, FALSE))
		તોડો;
	    s = cin_skipcomment(લીટી);
	    જો (*s == NUL)
		ચાલુ;
	}

	જો (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	નહીંતર જો (s[0] == ':')
	{
	    જો (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    નહીંતર જો (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    નહીંતર
		s = cin_skipcomment(s + 1);
	}
	નહીંતર જો ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    જો (*s == 'c')
		s = cin_skipcomment(s + 5);
	    નહીંતર
		s = cin_skipcomment(s + 6);
	}
	નહીંતર
	{
	    જો (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    નહીંતર જો (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    નહીંતર જો (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		પાછા FALSE;
	    }
	    નહીંતર જો (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    નહીંતર જો (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		જો (cpp_base_class)
		    pos->col = (colnr_T)(s - લીટી);
	    }

	    // When the line ends in a comma don't align with it.
	    જો (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    જો (cpp_base_class)
	pos->lnum = lnum;
    પાછા cpp_base_class;
}

    સ્થિર પૂર્ણાંક
get_baseclass_amount(પૂર્ણાંક col)
{
    પૂર્ણાંક		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    જો (col == 0)
    {
	amount = get_indent();
	જો (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != ખાલી)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	જો (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    નહીંતર
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, ખાલી, ખાલી, 0);
	amount = (પૂર્ણાંક)vcol;
    }
    જો (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    પાછા amount;
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

    સ્થિર pos_T *
find_start_brace(શૂન્ય)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    સ્થિર pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    જ્યારે ((trypos = findmatchlimit(ખાલી, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != ખાલી)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = ખાલી;
	// ignore the { if it's in a // or / *  * / comment
	જો ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(ખાલી)) == ખાલી) // XXX
	    તોડો;
	જો (pos != ખાલી)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    પાછા trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    સ્થિર pos_T *
find_match_paren_after_brace (પૂર્ણાંક ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    જો (trypos == ખાલી)
	પાછા ખાલી;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    જો (tryposBrace != ખાલી
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = ખાલી;
    પાછા trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    સ્થિર પૂર્ણાંક
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    જો (n > 0 && n < curbuf->b_ind_maxparen / 2)
	પાછા curbuf->b_ind_maxparen - (પૂર્ણાંક)n;
    પાછા curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    શૂન્ય
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    પૂર્ણાંક		divider;
    પૂર્ણાંક		fraction = 0;
    પૂર્ણાંક		sw;
    long	t = get_sw_કિંમત(buf);

    // needed for cino-(, it will be multiplied by 2 again
    જો (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    નહીંતર
	sw = (પૂર્ણાંક)t;

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
    buf->b_ind_open_ડાબું_imag = 0;

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

    માટે (p = buf->b_p_cino; *p; )
    {
	l = p++;
	જો (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	જો (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((અક્ષર *)++p);
	    જ્યારે (VIM_ISDIGIT(*p))
	    {
		++p;
		જો (divider)
		    divider *= 10;
		નહીંતર
		    divider = 10;
	    }
	}
	જો (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    જો (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    નહીંતર
	    {
		n *= sw;
		જો (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	જો (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	પસંદગી (*l)
	{
	    સ્થિતિ '>': buf->b_ind_level = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'e': buf->b_ind_open_imag = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'n': buf->b_ind_no_brace = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'f': buf->b_ind_first_open = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '{': buf->b_ind_open_extra = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '}': buf->b_ind_close_extra = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '^': buf->b_ind_open_ડાબું_imag = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'L': buf->b_ind_jump_label = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ ':': buf->b_ind_case = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '=': buf->b_ind_case_code = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'b': buf->b_ind_case_break = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'p': buf->b_ind_param = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 't': buf->b_ind_func_type = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '/': buf->b_ind_comment = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'c': buf->b_ind_in_comment = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'C': buf->b_ind_in_comment2 = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'i': buf->b_ind_cpp_baseclass = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '+': buf->b_ind_continuation = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '(': buf->b_ind_unclosed = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'u': buf->b_ind_unclosed2 = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'U': buf->b_ind_unclosed_noignore = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'W': buf->b_ind_unclosed_wrapped = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'w': buf->b_ind_unclosed_whiteok = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'm': buf->b_ind_matching_paren = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'M': buf->b_ind_paren_prev = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ ')': buf->b_ind_maxparen = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '*': buf->b_ind_maxcomment = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'g': buf->b_ind_scopedecl = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'h': buf->b_ind_scopedecl_code = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'j': buf->b_ind_java = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'J': buf->b_ind_js = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'l': buf->b_ind_keep_case_label = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ '#': buf->b_ind_hash_comment = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'N': buf->b_ind_cpp_namespace = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'k': buf->b_ind_if_for_while = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'E': buf->b_ind_cpp_extern_c = (પૂર્ણાંક)n; તોડો;
	    સ્થિતિ 'P': buf->b_ind_pragma = (પૂર્ણાંક)n; તોડો;
	}
	જો (*p == ',')
	    ++p;
    }
}

    સ્થિર પૂર્ણાંક
find_match(પૂર્ણાંક lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    પૂર્ણાંક		elselevel;
    પૂર્ણાંક		whilelevel;

    જો (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    નહીંતર
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    જ્યારે (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	જો (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    જો (theirscope == ખાલી)
		તોડો;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    જો (theirscope->lnum < ourscope)
		તોડો;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    જો (theirscope->lnum > ourscope)
		ચાલુ;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    જો (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		જો (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    જો (!cin_isif(mightbeif))
			++elselevel;
		    ચાલુ;
		}

		// If it's an "if" decrement elselevel
		જો (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    જો (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    જો (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		ચાલુ;
	    }

	    // If it's a "do" decrement whilelevel
	    જો (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    જો (elselevel <= 0 && whilelevel <= 0)
		પાછા OK;
	}
    }
    પાછા FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    પૂર્ણાંક
get_c_indent(શૂન્ય)
{
    pos_T	cur_curpos;
    પૂર્ણાંક		amount;
    પૂર્ણાંક		scope_amount;
    પૂર્ણાંક		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = ખાલી;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    પૂર્ણાંક		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    પૂર્ણાંક		lookfor;
    પૂર્ણાંક		whilelevel;
    linenr_T	lnum;
    પૂર્ણાંક		n;
    પૂર્ણાંક		iscase;
    પૂર્ણાંક		lookfor_break;
    પૂર્ણાંક		lookfor_cpp_namespace = FALSE;
    પૂર્ણાંક		cont_amount = 0;    // amount for continuation line
    પૂર્ણાંક		original_લીટી_islabel;
    પૂર્ણાંક		added_to_amount = 0;
    પૂર્ણાંક		js_cur_has_ચાવી = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    પૂર્ણાંક		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    જો (cur_curpos.lnum == 1)
	પાછા 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    જો (linecopy == ખાલી)
	પાછા 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    જો ((State & MODE_ઉમેરો)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_લીટી_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    જો (comment_pos != ખાલી)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    જો (trypos != ખાલી && (comment_pos == ખાલી
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	જાઓ laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    જો (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	જો (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    જાઓ theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    જો (original_લીટી_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	જાઓ theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    જો (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_લીટી_comment(); // XXX
	જો (trypos == ખાલી && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    જો (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	જો (trypos  != ખાલી)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, ખાલી, ખાલી, 0);
	    amount = col;
	    જાઓ theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    જો (!cin_iscomment(theline) && comment_pos != ખાલી) // XXX
    {
	પૂર્ણાંક	lead_start_len = 2;
	પૂર્ણાંક	lead_મધ્ય_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_મધ્ય[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	પૂર્ણાંક	lead_end_len;
	char_u	*p;
	પૂર્ણાંક	start_align = 0;
	પૂર્ણાંક	start_off = 0;
	પૂર્ણાંક	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, ખાલી, ખાલી, 0);
	amount = col;
	*lead_start = NUL;
	*lead_મધ્ય = NUL;

	p = curbuf->b_p_com;
	જ્યારે (*p != NUL)
	{
	    પૂર્ણાંક	align = 0;
	    પૂર્ણાંક	off = 0;
	    પૂર્ણાંક what = 0;

	    જ્યારે (*p != NUL && *p != ':')
	    {
		જો (*p == COM_START || *p == COM_END || *p == COM_મધ્ય)
		    what = *p++;
		નહીંતર જો (*p == COM_ડાબું || *p == COM_જમણું)
		    align = *p++;
		નહીંતર જો (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		નહીંતર
		    ++p;
	    }

	    જો (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    જો (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    નહીંતર જો (what == COM_મધ્ય)
	    {
		STRCPY(lead_મધ્ય, lead_end);
		lead_મધ્ય_len = lead_end_len;
	    }
	    નહીંતર જો (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		જો (STRNCMP(theline, lead_મધ્ય, lead_મધ્ય_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    જો (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			જો (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			નહીંતર જો (STRNCMP(look, lead_મધ્ય,
							lead_મધ્ય_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    તોડો;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			નહીંતર જો (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    ચાલુ;
		    }
		    જો (start_off != 0)
			amount += start_off;
		    નહીંતર જો (start_align == COM_જમણું)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_મધ્ય);
		    તોડો;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		જો (STRNCMP(theline, lead_મધ્ય, lead_મધ્ય_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    જો (off != 0)
			amount += off;
		    નહીંતર જો (align == COM_જમણું)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_મધ્ય);
		    done = TRUE;
		    તોડો;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	જો (done)
	    ;
	નહીંતર જો (theline[0] == '*')
	    amount += 1;
	નહીંતર
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    માટે (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		જો (linewhite(lnum))		    // skip blank lines
		    ચાલુ;
		amount = get_indent_lnum(lnum);	    // XXX
		તોડો;
	    }
	    જો (amount == -1)			    // use the comment opener
	    {
		જો (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    જો (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, ખાલી, ખાલી, 0);
		amount = col;
		જો (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	જાઓ theend;
    }

    // Are we looking at a ']' that has a match?
    જો (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != ખાલી)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	જાઓ theend;
    }

    // Are we inside parentheses or braces?  XXX
    જો (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ખાલી
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != ખાલી
	    || trypos != ખાલી)
    {
      જો (trypos != ખાલી && tryposBrace != ખાલી)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  જો (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = ખાલી;
	  નહીંતર
	      tryposBrace = ખાલી;
      }

      જો (trypos != ખાલી)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	જો (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	નહીંતર
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    માટે (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		જો (cin_nocode(l))		// skip comment lines
		    ચાલુ;
		જો (cin_ispreproc_cont(&l, &lnum, &amount))
		    ચાલુ;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		જો ((trypos = ind_find_start_CORS(ખાલી)) != ખાલી)
		{
		    lnum = trypos->lnum + 1;
		    ચાલુ;
		}

		// XXX
		જો ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != ખાલી
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    જો (theline[0] == ')')
		    {
			જો (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    તોડો;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	જો (amount == -1)
	{
	    પૂર્ણાંક	    ignore_paren_col = 0;
	    પૂર્ણાંક	    is_if_for_while = 0;

	    જો (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *લીટી;

		trypos = &our_paren_pos;
		કરો
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} જ્યારે (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		લીટી = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(લીટી, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    જો (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *લીટી;
		પૂર્ણાંક	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		લીટી = ml_get_curline();
		look_col = (પૂર્ણાંક)(look - લીટી);
		curwin->w_cursor.col = look_col + 1;
		જો ((trypos = findmatchlimit(ખાલી, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != ખાલી
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    જો (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		જો (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    જો (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			માટે (col = 0; col < our_paren_pos.col; ++col)
			{
			    પસંદગી (l[col])
			    {
				સ્થિતિ '(':
				સ્થિતિ '{': ++n;
					  તોડો;

				સ્થિતિ ')':
				સ્થિતિ '}': જો (n > 1)
					      --n;
					  તોડો;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    નહીંતર જો (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    નહીંતર
		    {
			col = our_paren_pos.col + 1;
			જ્યારે (VIM_ISWHITE(l[col]))
			    col++;
			જો (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			નહીંતર
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		જો (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, ખાલી, ખાલી, 0);
		    જો (cur_amount > (પૂર્ણાંક)col)
			cur_amount = col;
		}
	    }

	    જો (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    નહીંતર જો ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		જો (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    નહીંતર
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		જ્યારે ((પૂર્ણાંક)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    પસંદગી (*ml_get_pos(&our_paren_pos))
		    {
			સ્થિતિ '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  તોડો;
			સ્થિતિ ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  તોડો;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		જો (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		નહીંતર
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    જો (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != ખાલી)
			amount += curbuf->b_ind_unclosed2;
		    નહીંતર
		    {
			જો (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			નહીંતર
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		જો (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	જો (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      નહીંતર
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
	જો (*look == '{')
	{
	    getvcol(curwin, trypos, &col, ખાલી, ખાલી, 0);
	    amount = col;
	    જો (*start == '{')
		start_brace = BRACE_IN_COL0;
	    નહીંતર
		start_brace = BRACE_AT_START;
	}
	નહીંતર
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    જો (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != ખાલી)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    જો ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    નહીંતર જો (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    નહીંતર
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	જો (curbuf->b_ind_js)
	    js_cur_has_ચાવી = cin_has_js_ચાવી(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	જો (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	નહીંતર
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    જો (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    નહીંતર જો (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    જો (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		જો (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    જાઓ theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    જો (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_ડાબું_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    નહીંતર જો (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    નહીંતર
	    {
		જો (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    જો (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    નહીંતર જો (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		નહીંતર
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    જો (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    જો (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    નહીંતર જો (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    નહીંતર
	    {
		જો (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    માટે (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		જો (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    જો (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			જો (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    જો (cont_amount > 0)
				amount = cont_amount;
			    નહીંતર જો (!curbuf->b_ind_js)
				amount += ind_continuation;
			    તોડો;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(ખાલી);
			જો (trypos != ખાલી)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    ચાલુ;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			જો (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    ચાલુ;

			જો (cin_nocode(l))
			    ચાલુ;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			જો (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    જો (terminated == ',')
				તોડો;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    જો (terminated != ';' && cin_isinit())
				તોડો;

			    // nothing useful found
			    જો (terminated == 0 || terminated == '{')
				ચાલુ;
			}

			જો (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = ખાલી;
			    જો (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    જો (trypos == ખાલી && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    જો (trypos != ખાલી)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ચાલુ;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
		    }
		    નહીંતર જો (lookfor == LOOKFOR_UNTERM)
		    {
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
		    }
		    નહીંતર
		    {
			જો (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    જો (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			જો (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    જો (curwin->w_cursor.lnum == ourscope)
				ચાલુ;

			    જો (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				તોડો;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(ખાલી);
			    જો (trypos != ખાલી)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				ચાલુ;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    જો (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				ચાલુ;

			    // Finally the actual check for "namespace".
			    જો (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				તોડો;
			    }
			    નહીંતર જો (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				તોડો;
			    }

			    જો (cin_nocode(l))
				ચાલુ;
			}
		    }
		    તોડો;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		જો ((trypos = ind_find_start_CORS(&raw_string_start)) != ખાલી)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    ચાલુ;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		જો (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    જો (lookfor == LOOKFOR_CPP_BASECLASS)
			તોડો;

		    // When looking for a "do" we are not interested in
		    // labels.
		    જો (whilelevel > 0)
			ચાલુ;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    જો (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
			તોડો;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    જો (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			જો ((trypos = find_start_brace()) == ખાલી
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    તોડો;
			}
			ચાલુ;
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
		    જો (lookfor == LOOKFOR_TERM)
		    {
			જો (n)
			    amount = n;

			જો (!lookfor_break)
			    તોડો;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    જો (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			જો (l != ખાલી && cin_is_cinword(l))
			{
			    જો (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    નહીંતર
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			તોડો;
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
		    ચાલુ;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		જો (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    જો (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != ખાલી)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    ચાલુ;
		}

		// Ignore jump labels with nothing after them.
		જો (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    જો (l == ખાલી || cin_nocode(l))
			ચાલુ;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		જો (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    ચાલુ;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		જો (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		જો (n)
		{
		    જો (lookfor == LOOKFOR_UNTERM)
		    {
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
		    }
		    નહીંતર જો (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			ચાલુ;
		    }
		    નહીંતર
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    તોડો;
		}
		નહીંતર જો (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    જો (cin_isterminated(l, TRUE, FALSE))
			તોડો;
		    નહીંતર
			ચાલુ;
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

		જો (js_cur_has_ચાવી)
		{
		    js_cur_has_ચાવી = 0; // only check the first line
		    જો (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_ચાવી;
		    }
		}
		જો (lookfor == LOOKFOR_JS_ચાવી && cin_has_js_ચાવી(l))
		{
		    amount = get_indent();
		    તોડો;
		}
		જો (lookfor == LOOKFOR_COMMA)
		{
		    જો (tryposBrace != ખાલી && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			તોડો;
		    જો (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			તોડો;
		    નહીંતર
		    {
			amount = get_indent();
			જો (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    તોડો;
		    }
		}

		જો (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    જો (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (શૂન્ય)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    જો (trypos != ખાલી && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = ખાલી;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    જો (trypos == ખાલી && terminated == ',')
		    {
			જો (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    જો (trypos != ખાલી)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			જો (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    ચાલુ;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    જો (terminated == ',')
		    {
			જ્યારે (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    જો (*l == NUL || l[STRLEN(l) - 1] != '\\')
				તોડો;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    જો (curbuf->b_ind_js)
			cur_amount = get_indent();
		    નહીંતર
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    જો (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			જો (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			જો (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    ચાલુ;
			}
			તોડો;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    જો (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			જો (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    જો (cont_amount > 0)
				amount = cont_amount;
			    નહીંતર
				amount += ind_continuation;
			    તોડો;
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
			જો (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			જો (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    તોડો;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			જો (cin_isdo(l))
			{
			    જો (whilelevel == 0)
				તોડો;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			જો (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    જો (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    જો ((trypos = find_start_brace()) == ખાલી
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				તોડો;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    નહીંતર
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			જો (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    જો (terminated == ',')
				amount += ind_continuation;
			    તોડો;
			}

			જો (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    જો (terminated == ',')
			    {
				જો (curbuf->b_ind_cpp_baseclass == 0)
				    તોડો;

				lookfor = LOOKFOR_CPP_BASECLASS;
				ચાલુ;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    જો (amount > cur_amount)
				amount = cur_amount;
			}
			નહીંતર
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (પૂર્ણાંક)STRLEN(l);
			    જો (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				તોડો;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    જો (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				જો (curbuf->b_ind_js)
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
				    જો (cin_iscomment(skipwhite(l)))
					તોડો;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    જો (trypos != ખાલી)
				    {
					જો (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    તોડો;
					}
					ourscope = trypos->lnum;
				    }
				}
				નહીંતર
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    નહીંતર
			    {
				જો (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				જો (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_ચાવી
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		નહીંતર જો (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    જો (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
			તોડો;
		    }

		    જો (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			જો (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		નહીંતર
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    જો (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			ચાલુ;
		    }

		    // Handle "do {" line.
		    જો (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			જો (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    ચાલુ;
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
		    જો (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			જો (cont_amount > 0)
			    amount = cont_amount;
			નહીંતર
			    amount += ind_continuation;
			તોડો;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    જો (lookfor == LOOKFOR_TERM)
		    {
			જો (!lookfor_break && whilelevel == 0)
			    તોડો;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    નહીંતર
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			જો (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != ખાલી)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    જો (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				ચાલુ;
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

			જો (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			જો (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			જો (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    જો ((trypos = find_start_brace()) == ખાલી
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				તોડો;
			    ચાલુ;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			જો (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != ખાલી)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    જો (*l == '}' || !cin_iselse(l))
				જાઓ term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      જો (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      જો (curbuf->b_ind_jump_label > 0 && original_લીટી_islabel)
	  amount -= curbuf->b_ind_jump_label;

      જાઓ theend;
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

    જો (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	જાઓ theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    જો (cur_curpos.lnum < curbuf->b_ml.ml_લીટી_ગણતરી
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == ખાલી
	    && vim_strchr(theline, '}') == ખાલી
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(ખાલી, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	જાઓ theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    જ્યારે (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	જો ((trypos = ind_find_start_CORS(ખાલી)) != ખાલી)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    ચાલુ;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	જો (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	જો (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    તોડો;
	}

	// Skip preprocessor directives and blank lines.
	જો (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    ચાલુ;

	જો (cin_nocode(l))
	    ચાલુ;

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
	જો (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    જો (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != ખાલી)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    જ્યારે (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		જો (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    તોડો;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    જો (amount == 0)
		amount = cin_first_id_amount();
	    જો (amount == 0)
		amount = ind_continuation;
	    તોડો;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	જો (cin_isfuncdecl(ખાલી, cur_curpos.lnum, 0))  // XXX
	    તોડો;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	જો (*skipwhite(l) == '}')
	    તોડો;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	જો (cin_ends_in(l, (char_u *)"};"))
	    તોડો;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	જો (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    તોડો;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	જો (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    જ્યારે (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		જો (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    તોડો;
	    }
	    જો (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		તોડો;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	જો (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    તોડો;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	જો (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    જો (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		તોડો;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	જો ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != ખાલી)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	તોડો;
    }

    // add extra indent for a comment
    જો (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    જો (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	જો (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    જો (cur_amount > 0)
		amount = cur_amount;
	    નહીંતર જો (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    જો (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    પાછા amount;
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
    પૂર્ણાંક
in_cinkeys(
    પૂર્ણાંક		keytyped,
    પૂર્ણાંક		when,
    પૂર્ણાંક		લીટી_is_empty)
{
    char_u	*look;
    પૂર્ણાંક		try_match;
    પૂર્ણાંક		try_match_word;
    char_u	*p;
    char_u	*લીટી;
    પૂર્ણાંક		icase;
    પૂર્ણાંક		i;

    જો (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	પાછા FALSE;

#ifdef FEAT_EVAL
    જો (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    નહીંતર
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    જ્યારે (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	પસંદગી (when)
	{
	    સ્થિતિ '*': try_match = (*look == '*'); તોડો;
	    સ્થિતિ '!': try_match = (*look == '!'); તોડો;
	    મૂળભૂત: try_match = (*look != '*'); તોડો;
	}
	જો (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	જો (*look == '0')
	{
	    try_match_word = try_match;
	    જો (!લીટી_is_empty)
		try_match = FALSE;
	    ++look;
	}
	નહીંતર
	    try_match_word = FALSE;

	// does it look like a control character?
	જો (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    જો (try_match && keytyped == Ctrl_chr(look[1]))
		પાછા TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	નહીંતર જો (*look == 'o')
	{
	    જો (try_match && keytyped == ચાવી_OPEN_FORW)
		પાછા TRUE;
	    ++look;
	}
	નહીંતર જો (*look == 'O')
	{
	    જો (try_match && keytyped == ચાવી_OPEN_BACK)
		પાછા TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	નહીંતર જો (*look == 'e')
	{
	    જો (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		જો (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    પાછા TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	નહીંતર જો (*look == ':')
	{
	    જો (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		જો (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    પાછા TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		જો (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    જો (i)
			પાછા TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	નહીંતર જો (*look == '<')
	{
	    જો (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		જો (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != ખાલી
						       && keytyped == look[1])
		    પાછા TRUE;

		જો (keytyped == get_special_ચાવી_code(look + 1))
		    પાછા TRUE;
	    }
	    જ્યારે (*look && *look != '>')
		look++;
	    જ્યારે (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	નહીંતર જો (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    જો (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    નહીંતર
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    જો (p == ખાલી)
		p = look + STRLEN(look);
	    જો ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		પૂર્ણાંક		match = FALSE;

		જો (keytyped == ચાવી_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    લીટી = ml_get_curline();
		    જો (has_mbyte)
		    {
			char_u	*n;

			માટે (s = લીટી + curwin->w_cursor.col; s > લીટી; s = n)
			{
			    n = mb_prevptr(લીટી, s);
			    જો (!vim_iswordp(n))
				તોડો;
			}
		    }
		    નહીંતર
			માટે (s = લીટી + curwin->w_cursor.col; s > લીટી; --s)
			    જો (!vim_iswordc(s[-1]))
				તોડો;
		    જો (s + (p - look) <= લીટી + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		નહીંતર
		    // TODO: multi-byte
		    જો (keytyped == (પૂર્ણાંક)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((પૂર્ણાંક)p[-1])))
		{
		    લીટી = ml_get_cursor();
		    જો ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(લીટી[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(લીટી - (p - look), look, p - look)
				: STRNCMP(લીટી - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		જો (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    જો (getwhitecols_curline() !=
				     (પૂર્ણાંક)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		જો (match)
		    પાછા TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	નહીંતર
	{
	    જો (try_match && *look == keytyped)
		પાછા TRUE;
	    જો (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    પાછા FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    શૂન્ય
do_c_પદાવલિ_indent(શૂન્ય)
{
#ifdef FEAT_EVAL
    જો (*curbuf->b_p_inde != NUL)
	fixthisline(get_પદાવલિ_indent);
    નહીંતર
#endif
	fixthisline(get_c_indent);
}

#જો defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    શૂન્ય
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    જો (lnum >= 1 && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_સંખ્યા = get_c_indent();
	curwin->w_cursor = pos;
    }
    નહીંતર
	rettv->vval.v_સંખ્યા = -1;
}
#endif
