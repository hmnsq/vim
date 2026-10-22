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

#include "vim.h"

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
#define LOOKFOR_JS_KEY		11
#define LOOKFOR_COMMA		12

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    puur_naa_nka
cin_is_cinword(char_u *line)
{
    char_u	*cinw;
    char_u	*cinw_buf;
    puur_naa_nka		cinw_len;
    puur_naa_nka		retval = FALSE;
    puur_naa_nka		len;

    cinw_len = (puur_naa_nka)STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc(cinw_len);
    ᯀᯮᯝ (cinw_buf == NULL)
	ᯀᯮᯞ FALSE;

    line = skipwhite(line);
    for (cinw = curbuf->b_p_cinw; *cinw; )
    {
	len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	ᯀᯮᯝ (STRNCMP(line, cinw_buf, len) == 0
		&& (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	{
	    retval = TRUE;
	    bha_nga;
	}
    }
    vim_free(cinw_buf);
    ᯀᯮᯞ retval;
}

/*
 * Skip to the end of a "string" and a 'c' character.
 * If there is no string or character, return argument unmodified.
 */
    static char_u *
skip_string(char_u *p)
{
    puur_naa_nka	    i;

    // We loop, because strings may be concatenated: "date""time".
    for ( ; ; ++p)
    {
	ᯀᯮᯝ (p[0] == '\'')		    // 'c' or '\n' or '\000'
	{
	    ᯀᯮᯝ (p[1] == NUL)		    // ' at end of line
		bha_nga;
	    i = 2;
	    ᯀᯮᯝ (p[1] == '\\' && p[2] != NUL)    // '\n' or '\000'
	    {
		++i;
		ᯀᯬᯝ (vim_isdigit(p[i - 1]))   // '\000'
		    ++i;
	    }
	    ᯀᯮᯝ (p[i - 1] != NUL && p[i] == '\'')    // check for trailing '
	    {
		p += i;
		continue;
	    }
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == '"')		    // start of string
	{
	    for (++p; p[0]; ++p)
	    {
		ᯀᯮᯝ (p[0] == '\\' && p[1] != NUL)
		    ++p;
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == '"')	    // end of string
		    bha_nga;
	    }
	    ᯀᯮᯝ (p[0] == '"')
		continue; // continue for another string
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == 'R' && p[1] == '"')
	{
	    // Raw string: R"[delim](...)[delim]"
	    char_u *delim = p + 2;
	    char_u *paren = vim_strchr(delim, '(');

	    ᯀᯮᯝ (paren != NULL)
	    {
		size_t delim_len = paren - delim;

		for (p += 3; *p; ++p)
		    ᯀᯮᯝ (p[0] == ')' && STRNCMP(p + 1, delim, delim_len) == 0
			    && p[delim_len + 1] == '"')
		    {
			p += delim_len + 1;
			bha_nga;
		    }
		ᯀᯮᯝ (p[0] == '"')
		    continue; // continue for another string
	    }
	}
	bha_nga;				    // no string found
    }
    ᯀᯮᯝ (!*p)
	--p;				    // backup from NUL
    ᯀᯮᯞ p;
}

/*
 * Return TRUE if "line[col]" is inside a C string.
 */
    puur_naa_nka
is_pos_in_string(char_u *line, colnr_T col)
{
    char_u *p;

    for (p = line; *p && (colnr_T)(p - line) < col; ++p)
	p = skip_string(p);
    ᯀᯮᯞ !((colnr_T)(p - line) <= col);
}

/*
 * Check if line[] contains a "//" comment, ignoring matches inside strings.
 * Return MAXCOL if not, otherwise return the column.
 * The line is scanned once (skipping strings), so this stays linear even on
 * lines with many slashes (e.g. base64 data).
 */
    puur_naa_nka
check_linecomment(char_u *line)
{
    char_u  *p;

    p = line;
    // skip Lispish one-line comments
    ᯀᯮᯝ (curbuf->b_p_lisp)
    {
	ᯀᯮᯝ (vim_strchr(p, ';') != NULL) // there may be comments
	{
	    puur_naa_nka in_str = FALSE;	// inside of string

	    p = line;		// scan from start
	    ᯀᯬᯝ ((p = vim_strpbrk(p, (char_u *)"\";")) != NULL)
	    {
		ᯀᯮᯝ (*p == '"')
		{
		    ᯀᯮᯝ (in_str)
		    {
			ᯀᯮᯝ (*(p - 1) != '\\') // skip escaped quote
			    in_str = FALSE;
		    }
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p == line || ((p - line) >= 2
				      // skip #\" form
				      && *(p - 1) != '\\' && *(p - 2) != '#'))
			in_str = TRUE;
		}
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!in_str && ((p - line) < 2
				    || (*(p - 1) != '\\' && *(p - 2) != '#'))
			       && !is_pos_in_string(line, (colnr_T)(p - line)))
		    bha_nga;	// found!
		++p;
	    }
	}
	ᯀᯮᯝᯂᯪ
	    p = NULL;
    }
    ᯀᯮᯝᯂᯪ
    {
	// Scan the line once, skipping over strings, char constants and raw
	// strings, instead of testing each '/' with is_pos_in_string() (which
	// rescans from the start, making this quadratic on lines with many
	// slashes).
	for ( ; *p != NUL; ++p)
	{
	    p = skip_string(p);
	    ᯀᯮᯝ (*p == NUL)
		bha_nga;
	    // Accept a double /, unless it's preceded with * and followed by
	    // *, because * / / * is an end and start of a C comment.
	    ᯀᯮᯝ (p[0] == '/' && p[1] == '/'
				 && (p == line || p[-1] != '*' || p[2] != '*'))
		bha_nga;
	}
	ᯀᯮᯝ (*p == NUL)
	    p = NULL;
    }

    ᯀᯮᯝ (p == NULL)
	ᯀᯮᯞ MAXCOL;
    ᯀᯮᯞ (puur_naa_nka)(p - line);
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a comment.
 */
    static pos_T *
ind_find_start_comment(void)	    // XXX
{
    ᯀᯮᯞ find_start_comment(curbuf->b_ind_maxcomment);
}

    pos_T *
find_start_comment(puur_naa_nka ind_maxcomment)	// XXX
{
    pos_T	*pos;
    puur_naa_nka		cur_maxcomment = ind_maxcomment;

    for (;;)
    {
	pos = findmatchlimit(NULL, '*', FM_BACKWARD, cur_maxcomment);
	ᯀᯮᯝ (pos == NULL)
	    bha_nga;

	// Check if the comment start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ᯀᯮᯝ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ᯀᯮᯝ (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ᯀᯮᯞ pos;
}

/*
 * Find the start of a raw string, not knowing if we are in one right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * Return NULL when not inside a raw string.
 */
    static pos_T *
find_start_rawstring(puur_naa_nka ind_maxcomment)	// XXX
{
    pos_T	*pos;
    puur_naa_nka		cur_maxcomment = ind_maxcomment;

    for (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	ᯀᯮᯝ (pos == NULL)
	    bha_nga;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	ᯀᯮᯝ (!is_pos_in_string(ml_get(pos->lnum), pos->col))
	    bha_nga;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	ᯀᯮᯝ (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    bha_nga;
	}
    }
    ᯀᯮᯞ pos;
}

/*
 * Find the start of a comment or raw string, not knowing if we are in a
 * comment or raw string right now.
 * Search starts at w_cursor.lnum and goes backwards.
 * If is_raw is given and returns start of raw_string, sets it to true.
 * Return NULL when not inside a comment or raw string.
 * "CORS" -> Comment Or Raw String
 */
    static pos_T *
ind_find_start_CORS(linenr_T *is_raw)	    // XXX
{
    static pos_T comment_pos_copy;
    pos_T	*comment_pos;
    pos_T	*rs_pos;

    comment_pos = find_start_comment(curbuf->b_ind_maxcomment);
    ᯀᯮᯝ (comment_pos != NULL)
    {
	// Need to make a copy of the static pos in findmatchlimit(),
	// calling find_start_rawstring() may change it.
	comment_pos_copy = *comment_pos;
	comment_pos = &comment_pos_copy;
    }
    rs_pos = find_start_rawstring(curbuf->b_ind_maxcomment);

    // If comment_pos is before rs_pos the raw string is inside the comment.
    // If rs_pos is before comment_pos the comment is inside the raw string.
    ᯀᯮᯝ (comment_pos == NULL || (rs_pos != NULL
					     && LT_POS(*rs_pos, *comment_pos)))
    {
	ᯀᯮᯝ (is_raw != NULL && rs_pos != NULL)
	    *is_raw = rs_pos->lnum;
	ᯀᯮᯞ rs_pos;
    }
    ᯀᯮᯞ comment_pos;
}


/*
 * Return TRUE if C-indenting is on.
 */
    puur_naa_nka
cindent_on(void)
{
    ᯀᯮᯞ (!p_paste && (curbuf->b_p_cin
#ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
#endif
		    ));
}

// Find result cache for cpp_baseclass
typedef sa_nracanaa {
    puur_naa_nka	    found;
    lpos_T  lpos;
} cpp_baseclass_cache_T;

/*
 * Skip over white space and C comments within the line.
 * Also skip over Perl/shell comments if desired.
 */
    static char_u *
cin_skipcomment(char_u *s)
{
    ᯀᯬᯝ (*s)
    {
	char_u *prev_s = s;

	s = skipwhite(s);

	// Perl/shell # comment comment continues until eol.  Require a space
	// before # to avoid recognizing $#array.
	ᯀᯮᯝ (curbuf->b_ind_hash_comment != 0 && s != prev_s && *s == '#')
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	ᯀᯮᯝ (*s != '/')
	    bha_nga;
	++s;
	ᯀᯮᯝ (*s == '/')		// slash-slash comment continues till eol
	{
	    s += STRLEN(s);
	    bha_nga;
	}
	ᯀᯮᯝ (*s != '*')
	    bha_nga;
	for (++s; *s; ++s)	// skip slash-star comment
	    ᯀᯮᯝ (s[0] == '*' && s[1] == '/')
	    {
		s += 2;
		bha_nga;
	    }
    }
    ᯀᯮᯞ s;
}

/*
 * Return TRUE if there is no code at *s.  White space and comments are
 * not considered code.
 */
    static puur_naa_nka
cin_nocode(char_u *s)
{
    ᯀᯮᯞ *cin_skipcomment(s) == NUL;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    static puur_naa_nka
cin_iscomment(char_u *p)
{
    ᯀᯮᯞ (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize the start of a "//" comment.
 */
    static puur_naa_nka
cin_islinecomment(char_u *p)
{
    ᯀᯮᯞ (p[0] == '/' && p[1] == '/');
}

/*
 * Check previous lines for a "//" line comment, skipping over blank lines.
 */
    static pos_T *
find_line_comment(void) // XXX
{
    static pos_T pos;
    char_u	 *line;
    char_u	 *p;

    pos = curwin->w_cursor;
    ᯀᯬᯝ (--pos.lnum > 0)
    {
	line = ml_get(pos.lnum);
	p = skipwhite(line);
	ᯀᯮᯝ (cin_islinecomment(p))
	{
	    pos.col = (puur_naa_nka)(p - line);
	    ᯀᯮᯞ &pos;
	}
	ᯀᯮᯝ (*p != NUL)
	    bha_nga;
    }
    ᯀᯮᯞ NULL;
}

/*
 * Return TRUE if "text" starts with "key:".
 */
    static puur_naa_nka
cin_has_js_key(char_u *text)
{
    char_u *s = skipwhite(text);
    puur_naa_nka	    quote = -1;

    ᯀᯮᯝ (*s == '\'' || *s == '"')
    {
	// can be 'key': or "key":
	quote = *s;
	++s;
    }
    ᯀᯮᯝ (!vim_isIDc(*s))	    // need at least one ID character
	ᯀᯮᯞ FALSE;

    ᯀᯬᯝ (vim_isIDc(*s))
	++s;
    ᯀᯮᯝ (*s == quote)
	++s;

    s = cin_skipcomment(s);

    // "::" is not a label, it's C++
    ᯀᯮᯞ (*s == ':' && s[1] != ':');
}

/*
 * Check if string matches "label:"; move to character after ':' if true.
 * "*s" must point to the start of the label, if there is one.
 */
    static puur_naa_nka
cin_islabel_skip(char_u **s)
{
    ᯀᯮᯝ (!vim_isIDc(**s))	    // need at least one ID character
	ᯀᯮᯞ FALSE;

    ᯀᯬᯝ (vim_isIDc(**s))
    {
	ᯀᯮᯝ (has_mbyte)
	    (*s) += (*mb_ptr2len)(*s);
	ᯀᯮᯝᯂᯪ
	    (*s)++;
    }

    *s = cin_skipcomment(*s);

    // "::" is not a label, it's C++
    ᯀᯮᯞ (**s == ':' && *++*s != ':');
}

/*
 * Recognize a scope declaration label from the 'cinscopedecls' option.
 */
    static puur_naa_nka
cin_isscopedecl(char_u *p)
{
    size_t  cinsd_len;
    char_u  *cinsd_buf;
    char_u  *cinsd;
    size_t  len;
    char_u  *skip;
    char_u  *s = cin_skipcomment(p);
    puur_naa_nka	    found = FALSE;

    cinsd_len = STRLEN(curbuf->b_p_cinsd) + 1;
    cinsd_buf = alloc(cinsd_len);
    ᯀᯮᯝ (cinsd_buf == NULL)
	ᯀᯮᯞ FALSE;

    for (cinsd = curbuf->b_p_cinsd; *cinsd; )
    {
	len = copy_option_part(&cinsd, cinsd_buf, (puur_naa_nka)cinsd_len, ",");
	ᯀᯮᯝ (STRNCMP(s, cinsd_buf, len) == 0)
	{
	    skip = cin_skipcomment(s + len);
	    ᯀᯮᯝ (*skip == ':' && skip[1] != ':')
	    {
		found = TRUE;
		bha_nga;
	    }
	}
    }

    vim_free(cinsd_buf);
    ᯀᯮᯞ found;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    static puur_naa_nka
cin_ispreproc(char_u *s)
{
    ᯀᯮᯝ (*skipwhite(s) == '#')
	ᯀᯮᯞ TRUE;
    ᯀᯮᯞ FALSE;
}

/*
 * Return TRUE if line "*pp" at "*lnump" is a preprocessor statement or a
 * continuation line of a preprocessor statement.  Decrease "*lnump" to the
 * start and return the line in "*pp".
 * Put the amount of indent in "*amount".
 */
    static puur_naa_nka
cin_ispreproc_cont(char_u **pp, linenr_T *lnump, puur_naa_nka *amount)
{
    char_u	*line = *pp;
    linenr_T	lnum = *lnump;
    puur_naa_nka		retval = FALSE;
    puur_naa_nka		candidate_amount = *amount;

    ᯀᯮᯝ (*line != NUL && line[STRLEN(line) - 1] == '\\')
	candidate_amount = get_indent_lnum(lnum);

    for (;;)
    {
	ᯀᯮᯝ (cin_ispreproc(line))
	{
	    retval = TRUE;
	    *lnump = lnum;
	    bha_nga;
	}
	ᯀᯮᯝ (lnum == 1)
	    bha_nga;
	line = ml_get(--lnum);
	ᯀᯮᯝ (*line == NUL || line[STRLEN(line) - 1] != '\\')
	    bha_nga;
    }

    ᯀᯮᯝ (lnum != *lnump)
	*pp = ml_get(*lnump);
    ᯀᯮᯝ (retval)
	*amount = candidate_amount;
    ᯀᯮᯞ retval;
}

    static puur_naa_nka
cin_iselse(
    char_u  *p)
{
    ᯀᯮᯝ (*p == '}')	    // accept "} else"
	p = cin_skipcomment(p + 1);
    ᯀᯮᯞ (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
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
    static puur_naa_nka
cin_isterminated(
    char_u	*s,
    puur_naa_nka		incl_open,	// include '{' at the end as terminator
    puur_naa_nka		incl_comma)	// recognize a trailing comma
{
    char_u	found_start = 0;
    unsigned	n_open = 0;
    puur_naa_nka		is_else = FALSE;

    s = cin_skipcomment(s);

    ᯀᯮᯝ (*s == '{' || (*s == '}' && !cin_iselse(s)))
	found_start = *s;

    ᯀᯮᯝ (!found_start)
	is_else = cin_iselse(s);

    ᯀᯬᯝ (*s)
    {
	// skip over comments, "" strings and 'c'haracters
	s = skip_string(cin_skipcomment(s));
	ᯀᯮᯝ (*s == '}' && n_open > 0)
	    --n_open;
	ᯀᯮᯝ ((!is_else || n_open == 0)
		&& (*s == ';' || *s == '}' || (incl_comma && *s == ','))
		&& cin_nocode(s + 1))
	    ᯀᯮᯞ *s;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*s == '{')
	{
	    ᯀᯮᯝ (incl_open && cin_nocode(s + 1))
		ᯀᯮᯞ *s;
	    ᯀᯮᯝᯂᯪ
		++n_open;
	}

	ᯀᯮᯝ (*s)
	    s++;
    }
    ᯀᯮᯞ found_start;
}

/*
 * Return TRUE when "s" starts with "word" and then a non-ID character.
 */
    static puur_naa_nka
cin_starts_with(char_u *s, var_na *word)
{
    puur_naa_nka l = (puur_naa_nka)STRLEN(word);

    ᯀᯮᯞ (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}

/*
 * Recognize a "default" switch label.
 */
    static puur_naa_nka
cin_isdefault(char_u *s)
{
    ᯀᯮᯞ (STRNCMP(s, "default", 7) == 0
	    && *(s = cin_skipcomment(s + 7)) == ':'
	    && s[1] != ':');
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
    static puur_naa_nka
cin_iscase(
    char_u *s,
    puur_naa_nka strict) // Allow relaxed check of case statement for JS
{
    s = cin_skipcomment(s);
    ᯀᯮᯝ (cin_starts_with(s, "case"))
    {
	for (s += 4; *s; ++s)
	{
	    s = cin_skipcomment(s);
	    ᯀᯮᯝ (*s == NUL)
		bha_nga;
	    ᯀᯮᯝ (*s == ':')
	    {
		ᯀᯮᯝ (s[1] == ':')	// skip over "::" for C++
		    ++s;
		ᯀᯮᯝᯂᯪ
		    ᯀᯮᯞ TRUE;
	    }
	    ᯀᯮᯝ (*s == '\'' && s[1] && s[2] == '\'')
		s += 2;			// skip over ':'
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*s == '/' && (s[1] == '*' || s[1] == '/'))
		ᯀᯮᯞ FALSE;		// stop at comment
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*s == '"')
	    {
		// JS etc.
		ᯀᯮᯝ (strict)
		    ᯀᯮᯞ FALSE;		// stop at string
		ᯀᯮᯝᯂᯪ
		    ᯀᯮᯞ TRUE;
	    }
	}
	ᯀᯮᯞ FALSE;
    }

    ᯀᯮᯝ (cin_isdefault(s))
	ᯀᯮᯞ TRUE;
    ᯀᯮᯞ FALSE;
}

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    static puur_naa_nka
cin_islabel(void)		// XXX
{
    char_u	*s;

    s = cin_skipcomment(ml_get_curline());

    // Exclude "default" from labels, since it should be indented
    // like a switch label.  Same for C++ scope declarations.
    ᯀᯮᯝ (cin_isdefault(s))
	ᯀᯮᯞ FALSE;
    ᯀᯮᯝ (cin_isscopedecl(s))
	ᯀᯮᯞ FALSE;

    ᯀᯮᯝ (!cin_islabel_skip(&s))
	ᯀᯮᯞ FALSE;

    ᯀᯮᯝ (ind_find_start_CORS(NULL))
	ᯀᯮᯞ FALSE; // Don't accept a label in a comment or a raw string.

    // Only accept a label if the previous line is terminated or is a case
    // label.
    pos_T	cursor_save;
    pos_T	*trypos;
    char_u	*line;

    cursor_save = curwin->w_cursor;
    ᯀᯬᯝ (curwin->w_cursor.lnum > 1)
    {
	--curwin->w_cursor.lnum;

	// If we're in a comment or raw string now, skip to the start of
	// it.
	curwin->w_cursor.col = 0;
	ᯀᯮᯝ ((trypos = ind_find_start_CORS(NULL)) != NULL) // XXX
	    curwin->w_cursor = *trypos;

	line = ml_get_curline();
	ᯀᯮᯝ (cin_ispreproc(line))	// ignore #defines, #if, etc.
	    continue;
	ᯀᯮᯝ (*(line = cin_skipcomment(line)) == NUL)
	    continue;

	curwin->w_cursor = cursor_save;
	ᯀᯮᯝ (cin_isterminated(line, TRUE, FALSE)
		|| cin_isscopedecl(line)
		|| cin_iscase(line, TRUE)
		|| (cin_islabel_skip(&line) && cin_nocode(line)))
	    ᯀᯮᯞ TRUE;
	ᯀᯮᯞ FALSE;
    }
    curwin->w_cursor = cursor_save;
    ᯀᯮᯞ TRUE;		// label at start of file???
}

/*
 * Return TRUE if string "s" ends with the string "find", possibly followed by
 * white space and comments.  Skip strings and comments.
 */
    static puur_naa_nka
cin_ends_in(char_u *s, char_u *find)
{
    char_u	*p = s;
    char_u	*r;
    puur_naa_nka		len = (puur_naa_nka)STRLEN(find);

    ᯀᯬᯝ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ᯀᯮᯝ (STRNCMP(p, find, len) == 0)
	{
	    r = skipwhite(p + len);
	    ᯀᯮᯝ (cin_nocode(r))
		ᯀᯮᯞ TRUE;
	}
	ᯀᯮᯝ (*p != NUL)
	    ++p;
    }
    ᯀᯮᯞ FALSE;
}

/*
 * Strings can be concatenated with comments between:
 * "string0" |*comment*| "string1"
 */
    static char_u *
cin_skip_comment_and_string(char_u *s)
{
    char_u *r = NULL, *p = s;
    do
    {
	r = p;
	p = cin_skipcomment(p);
	ᯀᯮᯝ (*p)
	    p = skip_string(p);
    } ᯀᯬᯝ (p != r);
    ᯀᯮᯞ p;
}

/*
 * Recognize structure or compound literal initialization:
 * =|return [&][(typecast)] [{]
 * The number of opening braces is arbitrary.
 */
    static puur_naa_nka
cin_is_compound_init(char_u *s)
{
    char_u *p = s, *r = NULL;

    ᯀᯬᯝ (*p)
    {
	ᯀᯮᯝ (*p == '=')
	    p = r = cin_skipcomment(p + 1);
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(p, "return", 6) == 0 && !vim_isIDc(p[6])
		&& (p == s || (p > s && !vim_isIDc(p[-1]))))
	    p = r = cin_skipcomment(p + 6);
	ᯀᯮᯝᯂᯪ
	    p = cin_skip_comment_and_string(p + 1);
    }
    ᯀᯮᯝ (!r)
	ᯀᯮᯞ FALSE;
    p = r; // p points now after '=' or "return"

    ᯀᯮᯝ (cin_nocode(p))
	ᯀᯮᯞ TRUE;

    ᯀᯮᯝ (*p == '&')
	p = cin_skipcomment(p + 1);

    ᯀᯮᯝ (*p == '(') // skip a typecast
    {
	puur_naa_nka open_count = 1;
	do
	{
	    p = cin_skip_comment_and_string(p + 1);
	    ᯀᯮᯝ (cin_nocode(p))
		ᯀᯮᯞ TRUE;
	    open_count += (*p == '(') - (*p == ')');
	} ᯀᯬᯝ (open_count);
	p = cin_skipcomment(p + 1);
	ᯀᯮᯝ (cin_nocode(p))
	    ᯀᯮᯞ TRUE;
    }

    ᯀᯬᯝ (*p == '{')
	p = cin_skipcomment(p + 1);
    ᯀᯮᯞ cin_nocode(p);
}

/*
 * Recognize enumerations:
 * "[typedef] [static|public|protected|private] enum"
 * Call another function to recognize structure initialization.
 */
    static puur_naa_nka
cin_isinit(void)
{
    char_u	*s;
    static var_na *skip[] = {"static", "public", "protected", "private"};

    s = cin_skipcomment(ml_get_curline());

    ᯀᯮᯝ (cin_starts_with(s, "typedef"))
	s = cin_skipcomment(s + 7);

    for (;;)
    {
	puur_naa_nka i, l;

	for (i = 0; i < (puur_naa_nka)ARRAY_LENGTH(skip); ++i)
	{
	    l = (puur_naa_nka)strlen(skip[i]);
	    ᯀᯮᯝ (cin_starts_with(s, skip[i]))
	    {
		s = cin_skipcomment(s + l);
		l = 0;
		bha_nga;
	    }
	}
	ᯀᯮᯝ (l != 0)
	    bha_nga;
    }

    ᯀᯮᯝ (cin_starts_with(s, "enum"))
	ᯀᯮᯞ TRUE;

    ᯀᯮᯞ cin_is_compound_init(s);
}

// Maximum number of lines to search back for a "namespace" line.
#define FIND_NAMESPACE_LIM 20

/*
 * Recognize a "namespace" scope declaration.
 */
    static puur_naa_nka
cin_is_cpp_namespace(char_u *s)
{
    char_u	*p;
    puur_naa_nka		has_name = FALSE;
    puur_naa_nka		has_name_start = FALSE;

    s = cin_skipcomment(s);

    // skip over "inline" and "export" in any order
    ᯀᯬᯝ ((STRNCMP(s, "inline", 6) == 0 || STRNCMP(s, "export", 6) == 0)
					&& (s[6] == NUL || !vim_iswordc(s[6])))
	s = cin_skipcomment(skipwhite(s + 6));

    ᯀᯮᯝ (STRNCMP(s, "namespace", 9) == 0 && (s[9] == NUL || !vim_iswordc(s[9])))
    {
	p = cin_skipcomment(skipwhite(s + 9));
	ᯀᯬᯝ (*p != NUL)
	{
	    ᯀᯮᯝ (VIM_ISWHITE(*p))
	    {
		has_name = TRUE; // found end of a name
		p = cin_skipcomment(skipwhite(p));
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*p == '{')
	    {
		bha_nga;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (vim_iswordc(*p))
	    {
		has_name_start = TRUE;
		ᯀᯮᯝ (has_name)
		    ᯀᯮᯞ FALSE; // word character after skipping past name
		++p;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == ':' && p[1] == ':' && vim_iswordc(p[2]))
	    {
		ᯀᯮᯝ (!has_name_start || has_name)
		    ᯀᯮᯞ FALSE;
		// C++ 17 nested namespace
		p += 3;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		ᯀᯮᯞ FALSE;
	    }
	}
	ᯀᯮᯞ TRUE;
    }
    ᯀᯮᯞ FALSE;
}

/*
 * Recognize a `extern "C"` or `extern "C++"` linkage specifications.
 */
    static puur_naa_nka
cin_is_cpp_extern_c(char_u *s)
{
    char_u	*p;
    puur_naa_nka		has_string_literal = FALSE;

    s = cin_skipcomment(s);
    ᯀᯮᯝ (STRNCMP(s, "extern", 6) == 0 && (s[6] == NUL || !vim_iswordc(s[6])))
    {
	p = cin_skipcomment(skipwhite(s + 6));
	ᯀᯬᯝ (*p != NUL)
	{
	    ᯀᯮᯝ (VIM_ISWHITE(*p))
	    {
		p = cin_skipcomment(skipwhite(p));
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*p == '{')
	    {
		bha_nga;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == '"' && p[1] == 'C' && p[2] == '"')
	    {
		ᯀᯮᯝ (has_string_literal)
		    ᯀᯮᯞ FALSE;
		has_string_literal = TRUE;
		p += 3;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (p[0] == '"' && p[1] == 'C' && p[2] == '+' && p[3] == '+'
		    && p[4] == '"')
	    {
		ᯀᯮᯝ (has_string_literal)
		    ᯀᯮᯞ FALSE;
		has_string_literal = TRUE;
		p += 5;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		ᯀᯮᯞ FALSE;
	    }
	}
	ᯀᯮᯞ has_string_literal ? TRUE : FALSE;
    }
    ᯀᯮᯞ FALSE;
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    static char_u *
after_label(char_u *l)
{
    for ( ; *l; ++l)
    {
	ᯀᯮᯝ (*l == ':')
	{
	    ᯀᯮᯝ (l[1] == ':')	    // skip over "::" for C++
		++l;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!cin_iscase(l + 1, FALSE))
		bha_nga;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*l == '\'' && l[1] && l[2] == '\'')
	    l += 2;		    // skip over 'x'
    }
    ᯀᯮᯝ (*l == NUL)
	ᯀᯮᯞ NULL;
    l = cin_skipcomment(l + 1);
    ᯀᯮᯝ (*l == NUL)
	ᯀᯮᯞ NULL;
    ᯀᯮᯞ l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    static puur_naa_nka
get_indent_nolabel (linenr_T lnum)	// XXX
{
    char_u	*l;
    pos_T	fp;
    colnr_T	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    ᯀᯮᯝ (p == NULL)
	ᯀᯮᯞ 0;

    fp.col = (colnr_T)(p - l);
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ᯀᯮᯞ (puur_naa_nka)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label) in "pp".
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    static puur_naa_nka
skip_label(linenr_T lnum, char_u **pp)
{
    char_u	*l;
    puur_naa_nka		amount;
    pos_T	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    // XXX
    ᯀᯮᯝ (cin_iscase(l, FALSE) || cin_isscopedecl(l) || cin_islabel())
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	ᯀᯮᯝ (l == NULL)		// just in case
	    l = ml_get_curline();
    }
    ᯀᯮᯝᯂᯪ
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    ᯀᯮᯞ amount;
}

/*
 * Return the indent of the first variable name after a type in a declaration.
 *  int	    a,			indent of "a"
 *  static struct foo    b,	indent of "b"
 *  enum bla    c,		indent of "c"
 * Returns zero when it doesn't look like a declaration.
 */
    static puur_naa_nka
cin_first_id_amount(void)
{
    char_u	*line, *p, *s;
    puur_naa_nka		len;
    pos_T	fp;
    colnr_T	col;

    line = ml_get_curline();
    p = skipwhite(line);
    len = (puur_naa_nka)(skiptowhite(p) - p);
    ᯀᯮᯝ (len == 6 && STRNCMP(p, "static", 6) == 0)
    {
	p = skipwhite(p + 6);
	len = (puur_naa_nka)(skiptowhite(p) - p);
    }
    ᯀᯮᯝ (len == 6 && STRNCMP(p, "struct", 6) == 0)
	p = skipwhite(p + 6);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (len == 4 && STRNCMP(p, "enum", 4) == 0)
	p = skipwhite(p + 4);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((len == 8 && STRNCMP(p, "unsigned", 8) == 0)
	    || (len == 6 && STRNCMP(p, "signed", 6) == 0))
    {
	s = skipwhite(p + len);
	ᯀᯮᯝ ((STRNCMP(s, "int", 3) == 0 && VIM_ISWHITE(s[3]))
		|| (STRNCMP(s, "long", 4) == 0 && VIM_ISWHITE(s[4]))
		|| (STRNCMP(s, "short", 5) == 0 && VIM_ISWHITE(s[5]))
		|| (STRNCMP(s, "char", 4) == 0 && VIM_ISWHITE(s[4])))
	    p = s;
    }
    for (len = 0; vim_isIDc(p[len]); ++len)
	;
    ᯀᯮᯝ (len == 0 || !VIM_ISWHITE(p[len]) || cin_nocode(p))
	ᯀᯮᯞ 0;

    p = skipwhite(p + len);
    fp.lnum = curwin->w_cursor.lnum;
    fp.col = (colnr_T)(p - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ᯀᯮᯞ (puur_naa_nka)col;
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
    static puur_naa_nka
cin_get_equal_amount(linenr_T lnum)
{
    char_u	*line;
    char_u	*s;
    colnr_T	col;
    pos_T	fp;

    ᯀᯮᯝ (lnum > 1)
    {
	line = ml_get(lnum - 1);
	ᯀᯮᯝ (*line != NUL && line[STRLEN(line) - 1] == '\\')
	    ᯀᯮᯞ -1;
    }

    line = s = ml_get(lnum);
    ᯀᯬᯝ (*s != NUL && vim_strchr((char_u *)"=;{}\"'", *s) == NULL)
    {
	ᯀᯮᯝ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ᯀᯮᯝᯂᯪ
	    ++s;
    }
    ᯀᯮᯝ (*s != '=')
	ᯀᯮᯞ 0;

    s = skipwhite(s + 1);
    ᯀᯮᯝ (cin_nocode(s))
	ᯀᯮᯞ 0;

    ᯀᯮᯝ (*s == '"')	// nice alignment for continued strings
	++s;

    fp.lnum = lnum;
    fp.col = (colnr_T)(s - line);
    getvcol(curwin, &fp, &col, NULL, NULL, 0);
    ᯀᯮᯞ (puur_naa_nka)col;
}

/*
 * Skip strings, chars and comments until at or past "trypos".
 * Return the column found.
 */
    static puur_naa_nka
cin_skip2pos(pos_T *trypos)
{
    char_u	*line;
    char_u	*p;
    char_u	*new_p;

    p = line = ml_get(trypos->lnum);
    ᯀᯬᯝ (*p && (colnr_T)(p - line) < trypos->col)
    {
	ᯀᯮᯝ (cin_iscomment(p))
	    p = cin_skipcomment(p);
	ᯀᯮᯝᯂᯪ
	{
	    new_p = skip_string(p);
	    ᯀᯮᯝ (new_p == p)
		++p;
	    ᯀᯮᯝᯂᯪ
		p = new_p;
	}
    }
    ᯀᯮᯞ (puur_naa_nka)(p - line);
}

    static pos_T *
find_match_char(puur_naa_nka c, puur_naa_nka ind_maxparen)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    static pos_T pos_copy;
    puur_naa_nka		ind_maxp_wk;

    cursor_save = curwin->w_cursor;
    ind_maxp_wk = ind_maxparen;
retry:
    ᯀᯮᯝ ((trypos = findmatchlimit(NULL, c, FM_SKIPCOMM, ind_maxp_wk)) != NULL)
    {
	// check if the ( is in a // comment
	ᯀᯮᯝ ((colnr_T)cin_skip2pos(trypos) > trypos->col)
	{
	    ind_maxp_wk = ind_maxparen - (puur_naa_nka)(cursor_save.lnum - trypos->lnum);
	    ᯀᯮᯝ (ind_maxp_wk > 0)
	    {
		curwin->w_cursor = *trypos;
		curwin->w_cursor.col = 0;	// XXX
		goto retry;
	    }
	    trypos = NULL;
	}
	ᯀᯮᯝᯂᯪ
	{
	    pos_T	*trypos_wk;

	    pos_copy = *trypos;	    // copy trypos, findmatch will change it
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    ᯀᯮᯝ ((trypos_wk = ind_find_start_CORS(NULL)) != NULL) // XXX
	    {
		ind_maxp_wk = ind_maxparen - (puur_naa_nka)(cursor_save.lnum
			- trypos_wk->lnum);
		ᯀᯮᯝ (ind_maxp_wk > 0)
		{
		    curwin->w_cursor = *trypos_wk;
		    goto retry;
		}
		trypos = NULL;
	    }
	}
    }
    curwin->w_cursor = cursor_save;
    ᯀᯮᯞ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren(puur_naa_nka ind_maxparen)	// XXX
{
    ᯀᯮᯞ find_match_char('(', ind_maxparen);
}

/*
 * Set w_cursor.col to the column number of the last unmatched ')' or '{' in
 * line "l".  "l" must point to the start of the line.
 */
    static puur_naa_nka
find_last_paren(char_u *l, puur_naa_nka start, puur_naa_nka end)
{
    puur_naa_nka		i;
    puur_naa_nka		retval = FALSE;
    puur_naa_nka		open_count = 0;

    curwin->w_cursor.col = 0;		    // default is start of line

    for (i = 0; l[i] != NUL; i++)
    {
	i = (puur_naa_nka)(cin_skipcomment(l + i) - l); // ignore parens in comments
	ᯀᯮᯝ (l[i] == NUL)
	    bha_nga;
	i = (puur_naa_nka)(skip_string(l + i) - l);    // ignore parens in quotes
	ᯀᯮᯝ (l[i] == start)
	    ++open_count;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (l[i] == end)
	{
	    ᯀᯮᯝ (open_count > 0)
		--open_count;
	    ᯀᯮᯝᯂᯪ
	    {
		curwin->w_cursor.col = i;
		retval = TRUE;
	    }
	}
    }
    ᯀᯮᯞ retval;
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
    static puur_naa_nka
cin_isfuncdecl(
    char_u	**sp,
    linenr_T	first_lnum,
    linenr_T	min_lnum)
{
    char_u	*s;
    linenr_T	lnum = first_lnum;
    linenr_T	save_lnum = curwin->w_cursor.lnum;
    puur_naa_nka		retval = FALSE;
    pos_T	*trypos;
    puur_naa_nka		just_started = TRUE;

    ᯀᯮᯝ (sp == NULL)
	s = ml_get(lnum);
    ᯀᯮᯝᯂᯪ
	s = *sp;

    curwin->w_cursor.lnum = lnum;
    ᯀᯮᯝ (find_last_paren(s, '(', ')')
	&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
    {
	lnum = trypos->lnum;
	ᯀᯮᯝ (lnum < min_lnum)
	{
	    curwin->w_cursor.lnum = save_lnum;
	    ᯀᯮᯞ FALSE;
	}

	s = ml_get(lnum);
    }
    curwin->w_cursor.lnum = save_lnum;

    // Ignore line starting with #.
    ᯀᯮᯝ (cin_ispreproc(s))
	ᯀᯮᯞ FALSE;

    ᯀᯬᯝ (*s && *s != '(' && *s != ';' && *s != '\'' && *s != '"')
    {
	ᯀᯮᯝ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*s == ':')
	{
	    ᯀᯮᯝ (*(s + 1) == ':')
		s += 2;
	    ᯀᯮᯝᯂᯪ
		// To avoid a mistake in the following situation:
		// A::A(int a, int b)
		//     : a(0)  // <--not a function decl
		//     , b(0)
		// {...
		ᯀᯮᯞ FALSE;
	}
	ᯀᯮᯝᯂᯪ
	    ++s;
    }
    ᯀᯮᯝ (*s != '(')
	ᯀᯮᯞ FALSE;		// ';', ' or "  before any () or no '('

    ᯀᯬᯝ (*s && *s != ';' && *s != '\'' && *s != '"')
    {
	ᯀᯮᯝ (*s == ')' && cin_nocode(s + 1))
	{
	    // ')' at the end: may have found a match
	    // Check for the previous line not to end in a backslash:
	    //       #if defined(x) && {backslash}
	    //		 defined(y)
	    lnum = first_lnum - 1;
	    s = ml_get(lnum);
	    ᯀᯮᯝ (*s == NUL || s[STRLEN(s) - 1] != '\\')
		retval = TRUE;
	    goto done;
	}
	ᯀᯮᯝ ((*s == ',' && cin_nocode(s + 1)) || s[1] == NUL || cin_nocode(s))
	{
	    puur_naa_nka comma = (*s == ',');

	    // ',' at the end: continue looking in the next line.
	    // At the end: check for ',' in the next line, for this style:
	    // func(arg1
	    //       , arg2)
	    for (;;)
	    {
		ᯀᯮᯝ (lnum >= curbuf->b_ml.ml_line_count)
		    bha_nga;
		s = ml_get(++lnum);
		ᯀᯮᯝ (!cin_ispreproc(s))
		    bha_nga;
	    }
	    ᯀᯮᯝ (lnum >= curbuf->b_ml.ml_line_count)
		bha_nga;
	    // Require a comma at end of the line or a comma or ')' at the
	    // start of next line.
	    s = skipwhite(s);
	    ᯀᯮᯝ (!just_started && (!comma && *s != ',' && *s != ')'))
		bha_nga;
	    just_started = FALSE;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_iscomment(s))	// ignore comments
	    s = cin_skipcomment(s);
	ᯀᯮᯝᯂᯪ
	{
	    ++s;
	    just_started = FALSE;
	}
    }

done:
    ᯀᯮᯝ (lnum != first_lnum && sp != NULL)
	*sp = ml_get(first_lnum);

    ᯀᯮᯞ retval;
}

    static puur_naa_nka
cin_isif(char_u *p)
{
    ᯀᯮᯞ (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    static puur_naa_nka
cin_isdo(char_u *p)
{
    ᯀᯮᯞ (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    static puur_naa_nka
cin_iswhileofdo (char_u *p, linenr_T lnum)	// XXX
{
    pos_T	cursor_save;
    pos_T	*trypos;
    puur_naa_nka		retval = FALSE;

    p = cin_skipcomment(p);
    ᯀᯮᯝ (*p == '}')		// accept "} while (cond);"
	p = cin_skipcomment(p + 1);
    ᯀᯮᯝ (cin_starts_with(p, "while"))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	ᯀᯬᯝ (*p && *p != 'w')	// skip any '}', until the 'w' of the "while"
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	ᯀᯮᯝ ((trypos = findmatchlimit(NULL, 0, FM_SKIPCOMM,
					      curbuf->b_ind_maxparen)) != NULL
		&& *cin_skipcomment(ml_get_pos(trypos) + 1) == ';')
	    retval = TRUE;
	curwin->w_cursor = cursor_save;
    }
    ᯀᯮᯞ retval;
}

/*
 * Check whether in "p" there is an "if", "for" or "while" before "*poffset".
 * Return 0 if there is none.
 * Otherwise return !0 and update "*poffset" to point to the place where the
 * string was found.
 */
    static puur_naa_nka
cin_is_if_for_while_before_offset(char_u *line, puur_naa_nka *poffset)
{
    puur_naa_nka offset = *poffset;

    ᯀᯮᯝ (offset-- < 2)
	ᯀᯮᯞ 0;
    ᯀᯬᯝ (offset > 2 && VIM_ISWHITE(line[offset]))
	--offset;

    offset -= 1;
    ᯀᯮᯝ (STRNCMP(line + offset, "if", 2) == 0)
	goto probablyFound;

    ᯀᯮᯝ (offset >= 1)
    {
	offset -= 1;
	ᯀᯮᯝ (STRNCMP(line + offset, "for", 3) == 0)
	    goto probablyFound;

	ᯀᯮᯝ (offset >= 2)
	{
	    offset -= 2;
	    ᯀᯮᯝ (STRNCMP(line + offset, "while", 5) == 0)
		goto probablyFound;
	}
    }
    ᯀᯮᯞ 0;

probablyFound:
    ᯀᯮᯝ (!offset || !vim_isIDc(line[offset - 1]))
    {
	*poffset = offset;
	ᯀᯮᯞ 1;
    }
    ᯀᯮᯞ 0;
}

/*
 * Return TRUE if we are at the end of a do-while.
 *    do
 *       nothing;
 *    while (foo
 *	       && bar);  <-- here
 * Adjust the cursor to the line with "while".
 */
    static puur_naa_nka
cin_iswhileofdo_end(puur_naa_nka terminated)
{
    char_u	*line;
    char_u	*p;
    char_u	*s;
    pos_T	*trypos;
    puur_naa_nka		i;

    ᯀᯮᯝ (terminated != ';')	// there must be a ';' at the end
	ᯀᯮᯞ FALSE;

    p = line = ml_get_curline();
    ᯀᯬᯝ (*p != NUL)
    {
	p = cin_skipcomment(p);
	ᯀᯮᯝ (*p == ')')
	{
	    s = skipwhite(p + 1);
	    ᯀᯮᯝ (*s == ';' && cin_nocode(s + 1))
	    {
		// Found ");" at end of the line, now check there is "while"
		// before the matching '('.  XXX
		i = (puur_naa_nka)(p - line);
		curwin->w_cursor.col = i;
		trypos = find_match_paren(curbuf->b_ind_maxparen);
		ᯀᯮᯝ (trypos != NULL)
		{
		    s = cin_skipcomment(ml_get(trypos->lnum));
		    ᯀᯮᯝ (*s == '}')		// accept "} while (cond);"
			s = cin_skipcomment(s + 1);
		    ᯀᯮᯝ (cin_starts_with(s, "while"))
		    {
			curwin->w_cursor.lnum = trypos->lnum;
			ᯀᯮᯞ TRUE;
		    }
		}

		// Searching may have made "line" invalid, get it again.
		line = ml_get_curline();
		p = line + i;
	    }
	}
	ᯀᯮᯝ (*p != NUL)
	    ++p;
    }
    ᯀᯮᯞ FALSE;
}

    static puur_naa_nka
cin_isbreak(char_u *p)
{
    ᯀᯮᯞ (STRNCMP(p, "break", 5) == 0 && !vim_isIDc(p[5]));
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
    static puur_naa_nka
cin_is_cpp_baseclass(
    cpp_baseclass_cache_T *cached) // input and output
{
    lpos_T	*pos = &cached->lpos;	    // find position
    char_u	*s;
    puur_naa_nka		class_or_struct, lookfor_ctor_init, cpp_base_class;
    linenr_T	lnum = curwin->w_cursor.lnum;
    char_u	*line = ml_get_curline();

    ᯀᯮᯝ (pos->lnum <= lnum)
	ᯀᯮᯞ cached->found;	// Use the cached result

    pos->col = 0;

    s = skipwhite(line);
    ᯀᯮᯝ (*s == '#')		// skip #define FOO x ? (x) : x
	ᯀᯮᯞ FALSE;
    s = cin_skipcomment(s);
    ᯀᯮᯝ (*s == NUL)
	ᯀᯮᯞ FALSE;

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
    ᯀᯬᯝ (lnum > 1)
    {
	line = ml_get(lnum - 1);
	s = skipwhite(line);
	ᯀᯮᯝ (*s == '#' || *s == NUL)
	    bha_nga;
	ᯀᯬᯝ (*s != NUL)
	{
	    s = cin_skipcomment(s);
	    ᯀᯮᯝ (*s == '{' || *s == '}'
		    || (*s == ';' && cin_nocode(s + 1)))
		bha_nga;
	    ᯀᯮᯝ (*s != NUL)
		++s;
	}
	ᯀᯮᯝ (*s != NUL)
	    bha_nga;
	--lnum;
    }

    pos->lnum = lnum;
    line = ml_get(lnum);
    s = line;
    for (;;)
    {
	ᯀᯮᯝ (*s == NUL)
	{
	    ᯀᯮᯝ (lnum == curwin->w_cursor.lnum)
		bha_nga;
	    // Continue in the cursor line.
	    line = ml_get(++lnum);
	    s = line;
	}
	ᯀᯮᯝ (s == line)
	{
	    // don't recognize "case (foo):" as a baseclass
	    ᯀᯮᯝ (cin_iscase(s, FALSE))
		bha_nga;
	    s = cin_skipcomment(line);
	    ᯀᯮᯝ (*s == NUL)
		continue;
	}

	ᯀᯮᯝ (s[0] == '"' || (s[0] == 'R' && s[1] == '"'))
	    s = skip_string(s) + 1;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (s[0] == ':')
	{
	    ᯀᯮᯝ (s[1] == ':')
	    {
		// skip double colon. It can't be a constructor
		// initialization any more
		lookfor_ctor_init = FALSE;
		s = cin_skipcomment(s + 2);
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (lookfor_ctor_init || class_or_struct)
	    {
		// we have something found, that looks like the start of
		// cpp-base-class-declaration or constructor-initialization
		cpp_base_class = TRUE;
		lookfor_ctor_init = class_or_struct = FALSE;
		pos->col = 0;
		s = cin_skipcomment(s + 1);
	    }
	    ᯀᯮᯝᯂᯪ
		s = cin_skipcomment(s + 1);
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((STRNCMP(s, "class", 5) == 0 && !vim_isIDc(s[5]))
		|| (STRNCMP(s, "struct", 6) == 0 && !vim_isIDc(s[6])))
	{
	    class_or_struct = TRUE;
	    lookfor_ctor_init = FALSE;

	    ᯀᯮᯝ (*s == 'c')
		s = cin_skipcomment(s + 5);
	    ᯀᯮᯝᯂᯪ
		s = cin_skipcomment(s + 6);
	}
	ᯀᯮᯝᯂᯪ
	{
	    ᯀᯮᯝ (s[0] == '{' || s[0] == '}' || s[0] == ';')
	    {
		cpp_base_class = lookfor_ctor_init = class_or_struct = FALSE;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (s[0] == ')')
	    {
		// Constructor-initialization is assumed if we come across
		// something like "):"
		class_or_struct = FALSE;
		lookfor_ctor_init = TRUE;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (s[0] == '?')
	    {
		// Avoid seeing '() :' after '?' as constructor init.
		ᯀᯮᯞ FALSE;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!vim_isIDc(s[0]))
	    {
		// if it is not an identifier, we are wrong
		class_or_struct = FALSE;
		lookfor_ctor_init = FALSE;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (pos->col == 0)
	    {
		// it can't be a constructor-initialization any more
		lookfor_ctor_init = FALSE;

		// the first statement starts here: lineup with this one...
		ᯀᯮᯝ (cpp_base_class)
		    pos->col = (colnr_T)(s - line);
	    }

	    // When the line ends in a comma don't align with it.
	    ᯀᯮᯝ (lnum == curwin->w_cursor.lnum && *s == ',' && cin_nocode(s + 1))
		pos->col = 0;

	    s = cin_skipcomment(s + 1);
	}
    }

    cached->found = cpp_base_class;
    ᯀᯮᯝ (cpp_base_class)
	pos->lnum = lnum;
    ᯀᯮᯞ cpp_base_class;
}

    static puur_naa_nka
get_baseclass_amount(puur_naa_nka col)
{
    puur_naa_nka		amount;
    colnr_T	vcol;
    pos_T	*trypos;

    ᯀᯮᯝ (col == 0)
    {
	amount = get_indent();
	ᯀᯮᯝ (find_last_paren(ml_get_curline(), '(', ')')
		&& (trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    amount = get_indent_lnum(trypos->lnum); // XXX
	ᯀᯮᯝ (!cin_ends_in(ml_get_curline(), (char_u *)","))
	    amount += curbuf->b_ind_cpp_baseclass;
    }
    ᯀᯮᯝᯂᯪ
    {
	curwin->w_cursor.col = col;
	getvcol(curwin, &curwin->w_cursor, &vcol, NULL, NULL, 0);
	amount = (puur_naa_nka)vcol;
    }
    ᯀᯮᯝ (amount < curbuf->b_ind_cpp_baseclass)
	amount = curbuf->b_ind_cpp_baseclass;
    ᯀᯮᯞ amount;
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

    static pos_T *
find_start_brace(void)	    // XXX
{
    pos_T	    cursor_save;
    pos_T	    *trypos;
    pos_T	    *pos;
    static pos_T    pos_copy;

    cursor_save = curwin->w_cursor;
    ᯀᯬᯝ ((trypos = findmatchlimit(NULL, '{', FM_BLOCKSTOP | FM_SKIPCOMM, 0)) != NULL)
    {
	pos_copy = *trypos;	// copy pos_T, next findmatch will change it
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = NULL;
	// ignore the { if it's in a // or / *  * / comment
	ᯀᯮᯝ ((colnr_T)cin_skip2pos(trypos) == trypos->col
		       && (pos = ind_find_start_CORS(NULL)) == NULL) // XXX
	    bha_nga;
	ᯀᯮᯝ (pos != NULL)
	    curwin->w_cursor = *pos;
    }
    curwin->w_cursor = cursor_save;
    ᯀᯮᯞ trypos;
}

/*
 * Find the matching '(', ignoring it if it is in a comment or before an
 * unmatched {.
 * Return NULL if no match found.
 */
    static pos_T *
find_match_paren_after_brace (puur_naa_nka ind_maxparen)	    // XXX
{
    pos_T	*trypos = find_match_paren(ind_maxparen);

    ᯀᯮᯝ (trypos == NULL)
	ᯀᯮᯞ NULL;

    pos_T	*tryposBrace = find_start_brace();

    // If both an unmatched '(' and '{' is found.  Ignore the '('
    // position if the '{' is further down.
    ᯀᯮᯝ (tryposBrace != NULL
	    && (trypos->lnum != tryposBrace->lnum
		? trypos->lnum < tryposBrace->lnum
		: trypos->col < tryposBrace->col))
	trypos = NULL;
    ᯀᯮᯞ trypos;
}

/*
 * Return ind_maxparen corrected for the difference in line number between the
 * cursor position and "startpos".  This makes sure that searching for a
 * matching paren above the cursor line doesn't find a match because of
 * looking a few lines further.
 */
    static puur_naa_nka
corr_ind_maxparen(pos_T *startpos)
{
    long	n = (long)startpos->lnum - (long)curwin->w_cursor.lnum;

    ᯀᯮᯝ (n > 0 && n < curbuf->b_ind_maxparen / 2)
	ᯀᯮᯞ curbuf->b_ind_maxparen - (puur_naa_nka)n;
    ᯀᯮᯞ curbuf->b_ind_maxparen;
}

/*
 * Parse 'cinoptions' and set the values in "curbuf".
 * Must be called when 'cinoptions', 'shiftwidth' and/or 'tabstop' changes.
 */
    void
parse_cino(buf_T *buf)
{
    char_u	*p;
    char_u	*l;
    char_u	*digits;
    vimlong_T	n;
    puur_naa_nka		divider;
    puur_naa_nka		fraction = 0;
    puur_naa_nka		sw;
    long	t = get_sw_value(buf);

    // needed for cino-(, it will be multiplied by 2 again
    ᯀᯮᯝ (t > INT_MAX / 2)
	sw = INT_MAX / 2;
    ᯀᯮᯝᯂᯪ
	sw = (puur_naa_nka)t;

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
    buf->b_ind_open_left_imag = 0;

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

    for (p = buf->b_p_cino; *p; )
    {
	l = p++;
	ᯀᯮᯝ (*p == '-')
	    ++p;
	digits = p;	    // remember where the digits start
	n = getdigits(&p);
	divider = 0;
	ᯀᯮᯝ (*p == '.')	    // ".5s" means a fraction
	{
	    fraction = atol((var_na *)++p);
	    ᯀᯬᯝ (VIM_ISDIGIT(*p))
	    {
		++p;
		ᯀᯮᯝ (divider)
		    divider *= 10;
		ᯀᯮᯝᯂᯪ
		    divider = 10;
	    }
	}
	ᯀᯮᯝ (*p == 's')	    // "2s" means two times 'shiftwidth'
	{
	    ᯀᯮᯝ (p == digits)
		n = sw;	// just "s" is one 'shiftwidth'
	    ᯀᯮᯝᯂᯪ
	    {
		n *= sw;
		ᯀᯮᯝ (divider)
		    n += ((vimlong_T)sw * fraction + divider / 2) / divider;
	    }
	    ++p;
	}
	ᯀᯮᯝ (l[1] == '-')
	    n = -n;

	n = trim_to_int(n);

	// When adding an entry here, also update the default 'cinoptions' in
	// doc/indent.txt, and add explanation for it!
	switch (*l)
	{
	    pak_sa '>': buf->b_ind_level = (puur_naa_nka)n; bha_nga;
	    pak_sa 'e': buf->b_ind_open_imag = (puur_naa_nka)n; bha_nga;
	    pak_sa 'n': buf->b_ind_no_brace = (puur_naa_nka)n; bha_nga;
	    pak_sa 'f': buf->b_ind_first_open = (puur_naa_nka)n; bha_nga;
	    pak_sa '{': buf->b_ind_open_extra = (puur_naa_nka)n; bha_nga;
	    pak_sa '}': buf->b_ind_close_extra = (puur_naa_nka)n; bha_nga;
	    pak_sa '^': buf->b_ind_open_left_imag = (puur_naa_nka)n; bha_nga;
	    pak_sa 'L': buf->b_ind_jump_label = (puur_naa_nka)n; bha_nga;
	    pak_sa ':': buf->b_ind_case = (puur_naa_nka)n; bha_nga;
	    pak_sa '=': buf->b_ind_case_code = (puur_naa_nka)n; bha_nga;
	    pak_sa 'b': buf->b_ind_case_break = (puur_naa_nka)n; bha_nga;
	    pak_sa 'p': buf->b_ind_param = (puur_naa_nka)n; bha_nga;
	    pak_sa 't': buf->b_ind_func_type = (puur_naa_nka)n; bha_nga;
	    pak_sa '/': buf->b_ind_comment = (puur_naa_nka)n; bha_nga;
	    pak_sa 'c': buf->b_ind_in_comment = (puur_naa_nka)n; bha_nga;
	    pak_sa 'C': buf->b_ind_in_comment2 = (puur_naa_nka)n; bha_nga;
	    pak_sa 'i': buf->b_ind_cpp_baseclass = (puur_naa_nka)n; bha_nga;
	    pak_sa '+': buf->b_ind_continuation = (puur_naa_nka)n; bha_nga;
	    pak_sa '(': buf->b_ind_unclosed = (puur_naa_nka)n; bha_nga;
	    pak_sa 'u': buf->b_ind_unclosed2 = (puur_naa_nka)n; bha_nga;
	    pak_sa 'U': buf->b_ind_unclosed_noignore = (puur_naa_nka)n; bha_nga;
	    pak_sa 'W': buf->b_ind_unclosed_wrapped = (puur_naa_nka)n; bha_nga;
	    pak_sa 'w': buf->b_ind_unclosed_whiteok = (puur_naa_nka)n; bha_nga;
	    pak_sa 'm': buf->b_ind_matching_paren = (puur_naa_nka)n; bha_nga;
	    pak_sa 'M': buf->b_ind_paren_prev = (puur_naa_nka)n; bha_nga;
	    pak_sa ')': buf->b_ind_maxparen = (puur_naa_nka)n; bha_nga;
	    pak_sa '*': buf->b_ind_maxcomment = (puur_naa_nka)n; bha_nga;
	    pak_sa 'g': buf->b_ind_scopedecl = (puur_naa_nka)n; bha_nga;
	    pak_sa 'h': buf->b_ind_scopedecl_code = (puur_naa_nka)n; bha_nga;
	    pak_sa 'j': buf->b_ind_java = (puur_naa_nka)n; bha_nga;
	    pak_sa 'J': buf->b_ind_js = (puur_naa_nka)n; bha_nga;
	    pak_sa 'l': buf->b_ind_keep_case_label = (puur_naa_nka)n; bha_nga;
	    pak_sa '#': buf->b_ind_hash_comment = (puur_naa_nka)n; bha_nga;
	    pak_sa 'N': buf->b_ind_cpp_namespace = (puur_naa_nka)n; bha_nga;
	    pak_sa 'k': buf->b_ind_if_for_while = (puur_naa_nka)n; bha_nga;
	    pak_sa 'E': buf->b_ind_cpp_extern_c = (puur_naa_nka)n; bha_nga;
	    pak_sa 'P': buf->b_ind_pragma = (puur_naa_nka)n; bha_nga;
	}
	ᯀᯮᯝ (*p == ',')
	    ++p;
    }
}

    static puur_naa_nka
find_match(puur_naa_nka lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    puur_naa_nka		elselevel;
    puur_naa_nka		whilelevel;

    ᯀᯮᯝ (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    ᯀᯮᯝᯂᯪ
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    ᯀᯬᯝ (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	ᯀᯮᯝ (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    ᯀᯮᯝ (theirscope == NULL)
		bha_nga;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    ᯀᯮᯝ (theirscope->lnum < ourscope)
		bha_nga;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    ᯀᯮᯝ (theirscope->lnum > ourscope)
		continue;

	    look = cin_skipcomment(ml_get_curline());
	    // When looking for if, we ignore "if" and "else" in a deeper do-while loop.
	    ᯀᯮᯝ (!(lookfor == LOOKFOR_IF && whilelevel))
	    {
		// if it was an "else" (that's not an "else if")
		// then we need to go back to another if, so
		// increment elselevel
		ᯀᯮᯝ (cin_iselse(look))
		{
		    mightbeif = cin_skipcomment(look + 4);
		    ᯀᯮᯝ (!cin_isif(mightbeif))
			++elselevel;
		    continue;
		}

		// If it's an "if" decrement elselevel
		ᯀᯮᯝ (cin_isif(look))
		{
		    elselevel--;
		    // When looking for an "if" ignore "while"s that
		    // get in the way.
		    ᯀᯮᯝ (elselevel == 0 && lookfor == LOOKFOR_IF)
			whilelevel = 0;
		}
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    ᯀᯮᯝ (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		continue;
	    }

	    // If it's a "do" decrement whilelevel
	    ᯀᯮᯝ (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    ᯀᯮᯝ (elselevel <= 0 && whilelevel <= 0)
		ᯀᯮᯞ OK;
	}
    }
    ᯀᯮᯞ FAIL;
}

/*
 * Return the desired indent for C code.
 * Return -1 if the indent should be left alone (inside a raw string).
 */
    puur_naa_nka
get_c_indent(void)
{
    pos_T	cur_curpos;
    puur_naa_nka		amount;
    puur_naa_nka		scope_amount;
    puur_naa_nka		cur_amount = MAXCOL;
    colnr_T	col;
    char_u	*theline;
    char_u	*linecopy;
    pos_T	*trypos;
    pos_T	*comment_pos;
    pos_T	*tryposBrace = NULL;
    pos_T	tryposCopy;
    pos_T	our_paren_pos;
    char_u	*start;
    puur_naa_nka		start_brace;
#define BRACE_IN_COL0		1	    // '{' is in column 0
#define BRACE_AT_START		2	    // '{' is at start of line
#define BRACE_AT_END		3	    // '{' is at end of line
    linenr_T	ourscope;
    char_u	*l;
    char_u	*look;
    char_u	terminated;
    puur_naa_nka		lookfor;
    puur_naa_nka		whilelevel;
    linenr_T	lnum;
    puur_naa_nka		n;
    puur_naa_nka		iscase;
    puur_naa_nka		lookfor_break;
    puur_naa_nka		lookfor_cpp_namespace = FALSE;
    puur_naa_nka		cont_amount = 0;    // amount for continuation line
    puur_naa_nka		original_line_islabel;
    puur_naa_nka		added_to_amount = 0;
    puur_naa_nka		js_cur_has_key = 0;
    linenr_T	raw_string_start = 0;
    cpp_baseclass_cache_T cache_cpp_baseclass = { FALSE, { MAXLNUM, 0 } };

    // make a copy, value is changed below
    puur_naa_nka		ind_continuation = curbuf->b_ind_continuation;

    // remember where the cursor was when we started
    cur_curpos = curwin->w_cursor;

    // if we are at line 1 zero indent is fine, right?
    ᯀᯮᯝ (cur_curpos.lnum == 1)
	ᯀᯮᯞ 0;

    // Get a copy of the current contents of the line.
    // This is required, because only the most recent line obtained with
    // ml_get is valid!
    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    ᯀᯮᯝ (linecopy == NULL)
	ᯀᯮᯞ 0;

    // In insert mode and the cursor is on a ')' truncate the line at the
    // cursor position.  We don't want to line up with the matching '(' when
    // inserting new stuff.
    // For unknown reasons the cursor might be past the end of the line, thus
    // check for that.
    ᯀᯮᯝ ((State & MODE_INSERT)
	    && curwin->w_cursor.col < (colnr_T)STRLEN(linecopy)
	    && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    // move the cursor to the start of the line

    curwin->w_cursor.col = 0;

    original_line_islabel = cin_islabel();  // XXX

    // If we are inside a raw string don't change the indent.
    // Ignore a raw string inside a comment.
    comment_pos = ind_find_start_comment();
    ᯀᯮᯝ (comment_pos != NULL)
    {
	// findmatchlimit() static pos is overwritten, make a copy
	tryposCopy = *comment_pos;
	comment_pos = &tryposCopy;
    }
    trypos = find_start_rawstring(curbuf->b_ind_maxcomment);
    ᯀᯮᯝ (trypos != NULL && (comment_pos == NULL
					     || LT_POS(*trypos, *comment_pos)))
    {
	amount = -1;
	goto laterend;
    }

    // #defines and so on go at the left when included in 'cinkeys',
    // excluding pragmas when customized in 'cinoptions'
    ᯀᯮᯝ (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	char_u *directive = skipwhite(theline + 1);
	ᯀᯮᯝ (curbuf->b_ind_pragma == 0 || STRNCMP(directive, "pragma", 6) != 0)
	{
	    amount = curbuf->b_ind_hash_comment;
	    goto theend;
	}
    }

    // Is it a non-case label?	Then that goes at the left margin too unless:
    //  - JS flag is set.
    //  - 'L' item has a positive value.
    ᯀᯮᯝ (original_line_islabel && !curbuf->b_ind_js
					      && curbuf->b_ind_jump_label < 0)
    {
	amount = 0;
	goto theend;
    }

    // If we're inside a "//" comment and there is a "//" comment in a
    // previous line, lineup with that one.
    ᯀᯮᯝ (cin_islinecomment(theline))
    {
	pos_T	linecomment_pos;

	trypos = find_line_comment(); // XXX
	ᯀᯮᯝ (trypos == NULL && curwin->w_cursor.lnum > 1)
	{
	    // There may be a statement before the comment, search from the end
	    // of the line for a comment start.
	    linecomment_pos.col =
			  check_linecomment(ml_get(curwin->w_cursor.lnum - 1));
	    ᯀᯮᯝ (linecomment_pos.col != MAXCOL)
	    {
		trypos = &linecomment_pos;
		trypos->lnum = curwin->w_cursor.lnum - 1;
	    }
	}
	ᯀᯮᯝ (trypos  != NULL)
	{
	    // find how indented the line beginning the comment is
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    goto theend;
	}
    }

    // If we're inside a comment and not looking at the start of the
    // comment, try using the 'comments' option.
    ᯀᯮᯝ (!cin_iscomment(theline) && comment_pos != NULL) // XXX
    {
	puur_naa_nka	lead_start_len = 2;
	puur_naa_nka	lead_middle_len = 1;
	char_u	lead_start[COM_MAX_LEN];	// start-comment string
	char_u	lead_middle[COM_MAX_LEN];	// middle-comment string
	char_u	lead_end[COM_MAX_LEN];		// end-comment string
	puur_naa_nka	lead_end_len;
	char_u	*p;
	puur_naa_nka	start_align = 0;
	puur_naa_nka	start_off = 0;
	puur_naa_nka	done = FALSE;

	// find how indented the line beginning the comment is
	getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
	amount = col;
	*lead_start = NUL;
	*lead_middle = NUL;

	p = curbuf->b_p_com;
	ᯀᯬᯝ (*p != NUL)
	{
	    puur_naa_nka	align = 0;
	    puur_naa_nka	off = 0;
	    puur_naa_nka what = 0;

	    ᯀᯬᯝ (*p != NUL && *p != ':')
	    {
		ᯀᯮᯝ (*p == COM_START || *p == COM_END || *p == COM_MIDDLE)
		    what = *p++;
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*p == COM_LEFT || *p == COM_RIGHT)
		    align = *p++;
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (VIM_ISDIGIT(*p) || *p == '-')
		    off = getdigits(&p);
		ᯀᯮᯝᯂᯪ
		    ++p;
	    }

	    ᯀᯮᯝ (*p == ':')
		++p;
	    lead_end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    ᯀᯮᯝ (what == COM_START)
	    {
		STRCPY(lead_start, lead_end);
		lead_start_len = lead_end_len;
		start_off = off;
		start_align = align;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (what == COM_MIDDLE)
	    {
		STRCPY(lead_middle, lead_end);
		lead_middle_len = lead_end_len;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (what == COM_END)
	    {
		// If our line starts with the middle comment string, line it
		// up with the comment opener per the 'comments' option.
		ᯀᯮᯝ (STRNCMP(theline, lead_middle, lead_middle_len) == 0
			&& STRNCMP(theline, lead_end, lead_end_len) != 0)
		{
		    done = TRUE;
		    ᯀᯮᯝ (curwin->w_cursor.lnum > 1)
		    {
			// If the start comment string matches in the previous
			// line, use the indent of that line plus offset.  If
			// the middle comment string matches in the previous
			// line, use the indent of that line.  XXX
			look = skipwhite(ml_get(curwin->w_cursor.lnum - 1));
			ᯀᯮᯝ (STRNCMP(look, lead_start, lead_start_len) == 0)
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(look, lead_middle,
							lead_middle_len) == 0)
			{
			    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
			    bha_nga;
			}
			// If the start comment string doesn't match with the
			// start of the comment, skip this entry.  XXX
			ᯀᯮᯝᯂᯪ ᯀᯮᯝ (STRNCMP(ml_get(comment_pos->lnum)
							    + comment_pos->col,
					     lead_start, lead_start_len) != 0)
			    continue;
		    }
		    ᯀᯮᯝ (start_off != 0)
			amount += start_off;
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (start_align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    bha_nga;
		}

		// If our line starts with the end comment string, line it up
		// with the middle comment
		ᯀᯮᯝ (STRNCMP(theline, lead_middle, lead_middle_len) != 0
			&& STRNCMP(theline, lead_end, lead_end_len) == 0)
		{
		    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);
								     // XXX
		    ᯀᯮᯝ (off != 0)
			amount += off;
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (align == COM_RIGHT)
			amount += vim_strsize(lead_start)
						   - vim_strsize(lead_middle);
		    done = TRUE;
		    bha_nga;
		}
	    }
	}

	// If our line starts with an asterisk, line up with the
	// asterisk in the comment opener; otherwise, line up
	// with the first character of the comment text.
	ᯀᯮᯝ (done)
	    ;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (theline[0] == '*')
	    amount += 1;
	ᯀᯮᯝᯂᯪ
	{
	    // If we are more than one line away from the comment opener, take
	    // the indent of the previous non-empty line.  If 'cino' has "CO"
	    // and we are just below the comment opener and there are any
	    // white characters after it line up with the text after it;
	    // otherwise, add the amount specified by "c" in 'cino'
	    amount = -1;
	    for (lnum = cur_curpos.lnum - 1; lnum > comment_pos->lnum; --lnum)
	    {
		ᯀᯮᯝ (linewhite(lnum))		    // skip blank lines
		    continue;
		amount = get_indent_lnum(lnum);	    // XXX
		bha_nga;
	    }
	    ᯀᯮᯝ (amount == -1)			    // use the comment opener
	    {
		ᯀᯮᯝ (!curbuf->b_ind_in_comment2)
		{
		    start = ml_get(comment_pos->lnum);
		    look = start + comment_pos->col + 2; // skip / and *
		    ᯀᯮᯝ (*look != NUL)		    // if something after it
			comment_pos->col = (colnr_T)(skipwhite(look) - start);
		}
		getvcol(curwin, comment_pos, &col, NULL, NULL, 0);
		amount = col;
		ᯀᯮᯝ (curbuf->b_ind_in_comment2 || *look == NUL)
		    amount += curbuf->b_ind_in_comment;
	    }
	}
	goto theend;
    }

    // Are we looking at a ']' that has a match?
    ᯀᯮᯝ (*skipwhite(theline) == ']'
	    && (trypos = find_match_char('[', curbuf->b_ind_maxparen)) != NULL)
    {
	// align with the line containing the '['.
	amount = get_indent_lnum(trypos->lnum);
	goto theend;
    }

    // Are we inside parentheses or braces?  XXX
    ᯀᯮᯝ (((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL
		&& curbuf->b_ind_java == 0)
	    || (tryposBrace = find_start_brace()) != NULL
	    || trypos != NULL)
    {
      ᯀᯮᯝ (trypos != NULL && tryposBrace != NULL)
      {
	  // Both an unmatched '(' and '{' is found.  Use the one which is
	  // closer to the current cursor position, set the other to NULL.
	  ᯀᯮᯝ (trypos->lnum != tryposBrace->lnum
		  ? trypos->lnum < tryposBrace->lnum
		  : trypos->col < tryposBrace->col)
	      trypos = NULL;
	  ᯀᯮᯝᯂᯪ
	      tryposBrace = NULL;
      }

      ᯀᯮᯝ (trypos != NULL)
      {
	// If the matching paren is more than one line away, use the indent of
	// a previous non-empty line that matches the same paren.
	ᯀᯮᯝ (theline[0] == ')' && curbuf->b_ind_paren_prev)
	{
	    // Line up with the start of the matching paren line.
	    amount = get_indent_lnum(curwin->w_cursor.lnum - 1);  // XXX
	}
	ᯀᯮᯝᯂᯪ
	{
	    amount = -1;
	    our_paren_pos = *trypos;
	    for (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		ᯀᯮᯝ (cin_nocode(l))		// skip comment lines
		    continue;
		ᯀᯮᯝ (cin_ispreproc_cont(&l, &lnum, &amount))
		    continue;			// ignore #define, #if, etc.
		curwin->w_cursor.lnum = lnum;

		// Skip a comment or raw string.  XXX
		ᯀᯮᯝ ((trypos = ind_find_start_CORS(NULL)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		// XXX
		ᯀᯮᯝ ((trypos = find_match_paren(
			corr_ind_maxparen(&cur_curpos))) != NULL
			&& trypos->lnum == our_paren_pos.lnum
			&& trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	// XXX

		    ᯀᯮᯝ (theline[0] == ')')
		    {
			ᯀᯮᯝ (our_paren_pos.lnum != lnum
				&& cur_amount > amount)
			    cur_amount = amount;
			amount = -1;
		    }
		    bha_nga;
		}
	    }
	}

	// Line up with line where the matching paren is. XXX
	// If the line starts with a '(' or the indent for unclosed
	// parentheses is zero, line up with the unclosed parentheses.
	ᯀᯮᯝ (amount == -1)
	{
	    puur_naa_nka	    ignore_paren_col = 0;
	    puur_naa_nka	    is_if_for_while = 0;

	    ᯀᯮᯝ (curbuf->b_ind_if_for_while)
	    {
		// Look for the outermost opening parenthesis on this line
		// and check whether it belongs to an "if", "for" or "while".

		pos_T	    cursor_save = curwin->w_cursor;
		pos_T	    outermost;
		char_u	    *line;

		trypos = &our_paren_pos;
		do
		{
		    outermost = *trypos;
		    curwin->w_cursor.lnum = outermost.lnum;
		    curwin->w_cursor.col = outermost.col;

		    trypos = find_match_paren(curbuf->b_ind_maxparen);
		} ᯀᯬᯝ (trypos && trypos->lnum == outermost.lnum);

		curwin->w_cursor = cursor_save;

		line = ml_get(outermost.lnum);

		is_if_for_while =
		    cin_is_if_for_while_before_offset(line, &outermost.col);
	    }

	    amount = skip_label(our_paren_pos.lnum, &look);
	    look = skipwhite(look);
	    ᯀᯮᯝ (*look == '(')
	    {
		linenr_T    save_lnum = curwin->w_cursor.lnum;
		char_u	    *line;
		puur_naa_nka	    look_col;

		// Ignore a '(' in front of the line that has a match before
		// our matching '('.
		curwin->w_cursor.lnum = our_paren_pos.lnum;
		line = ml_get_curline();
		look_col = (puur_naa_nka)(look - line);
		curwin->w_cursor.col = look_col + 1;
		ᯀᯮᯝ ((trypos = findmatchlimit(NULL, ')', FM_SKIPCOMM,
						      curbuf->b_ind_maxparen))
								      != NULL
			  && trypos->lnum == our_paren_pos.lnum
			  && trypos->col < our_paren_pos.col)
		    ignore_paren_col = trypos->col + 1;

		curwin->w_cursor.lnum = save_lnum;
		look = ml_get(our_paren_pos.lnum) + look_col;
	    }
	    ᯀᯮᯝ (theline[0] == ')' || (curbuf->b_ind_unclosed == 0
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
		ᯀᯮᯝ (theline[0] != ')')
		{
		    cur_amount = MAXCOL;
		    l = ml_get(our_paren_pos.lnum);
		    ᯀᯮᯝ (curbuf->b_ind_unclosed_wrapped
				       && cin_ends_in(l, (char_u *)"("))
		    {
			// look for opening unmatched paren, indent one level
			// for each additional level
			n = 1;
			for (col = 0; col < our_paren_pos.col; ++col)
			{
			    switch (l[col])
			    {
				pak_sa '(':
				pak_sa '{': ++n;
					  bha_nga;

				pak_sa ')':
				pak_sa '}': ᯀᯮᯝ (n > 1)
					      --n;
					  bha_nga;
			    }
			}

			our_paren_pos.col = 0;
			amount += n * curbuf->b_ind_unclosed_wrapped;
		    }
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (curbuf->b_ind_unclosed_whiteok)
			our_paren_pos.col++;
		    ᯀᯮᯝᯂᯪ
		    {
			col = our_paren_pos.col + 1;
			ᯀᯬᯝ (VIM_ISWHITE(l[col]))
			    col++;
			ᯀᯮᯝ (l[col] != NUL)	// In case of trailing space
			    our_paren_pos.col = col;
			ᯀᯮᯝᯂᯪ
			    our_paren_pos.col++;
		    }
		}

		// Find how indented the paren is, or the character after it
		// if we did the above "if".
		ᯀᯮᯝ (our_paren_pos.col > 0)
		{
		    getvcol(curwin, &our_paren_pos, &col, NULL, NULL, 0);
		    ᯀᯮᯝ (cur_amount > (puur_naa_nka)col)
			cur_amount = col;
		}
	    }

	    ᯀᯮᯝ (theline[0] == ')' && curbuf->b_ind_matching_paren)
	    {
		// Line up with the start of the matching paren line.
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((curbuf->b_ind_unclosed == 0 && is_if_for_while == 0)
		     || (!curbuf->b_ind_unclosed_noignore
				    && *look == '(' && ignore_paren_col == 0))
	    {
		ᯀᯮᯝ (cur_amount != MAXCOL)
		    amount = cur_amount;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		// Add b_ind_unclosed2 for each '(' before our matching one,
		// but ignore (void) before the line (ignore_paren_col).
		col = our_paren_pos.col;
		ᯀᯬᯝ ((puur_naa_nka)our_paren_pos.col > ignore_paren_col)
		{
		    --our_paren_pos.col;
		    switch (*ml_get_pos(&our_paren_pos))
		    {
			pak_sa '(': amount += curbuf->b_ind_unclosed2;
				  col = our_paren_pos.col;
				  bha_nga;
			pak_sa ')': amount -= curbuf->b_ind_unclosed2;
				  col = MAXCOL;
				  bha_nga;
		    }
		}

		// Use b_ind_unclosed once, when the first '(' is not inside
		// braces
		ᯀᯮᯝ (col == MAXCOL)
		    amount += curbuf->b_ind_unclosed;
		ᯀᯮᯝᯂᯪ
		{
		    curwin->w_cursor.lnum = our_paren_pos.lnum;
		    curwin->w_cursor.col = col;
		    ᯀᯮᯝ (find_match_paren_after_brace(curbuf->b_ind_maxparen)
								      != NULL)
			amount += curbuf->b_ind_unclosed2;
		    ᯀᯮᯝᯂᯪ
		    {
			ᯀᯮᯝ (is_if_for_while)
			    amount += curbuf->b_ind_if_for_while;
			ᯀᯮᯝᯂᯪ
			    amount += curbuf->b_ind_unclosed;
		    }
		}
		// For a line starting with ')' use the minimum of the two
		// positions, to avoid giving it more indent than the previous
		// lines:
		//  func_long_name(		    if (x
		//	arg				    && yy
		//	)	  ^ not here	       )    ^ not here
		ᯀᯮᯝ (cur_amount < amount)
		    amount = cur_amount;
	    }
	}

	// add extra indent for a comment
	ᯀᯮᯝ (cin_iscomment(theline))
	    amount += curbuf->b_ind_comment;
      }
      ᯀᯮᯝᯂᯪ
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
	ᯀᯮᯝ (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL, 0);
	    amount = col;
	    ᯀᯮᯝ (*start == '{')
		start_brace = BRACE_IN_COL0;
	    ᯀᯮᯝᯂᯪ
		start_brace = BRACE_AT_START;
	}
	ᯀᯮᯝᯂᯪ
	{
	    // That opening brace might have been on a continuation
	    // line.  if so, find the start of the line.
	    curwin->w_cursor.lnum = ourscope;

	    // Position the cursor over the rightmost paren, so that
	    // matching it will take us back to the start of the line.
	    lnum = ourscope;
	    ᯀᯮᯝ (find_last_paren(start, '(', ')')
			&& (trypos = find_match_paren(curbuf->b_ind_maxparen))
								      != NULL)
		lnum = trypos->lnum;

	    // It could have been something like
	    //	   case 1: if (asdf &&
	    //			condition) {
	    //		    }
	    ᯀᯮᯝ ((curbuf->b_ind_js || curbuf->b_ind_keep_case_label)
			   && cin_iscase(skipwhite(ml_get_curline()), FALSE))
		amount = get_indent();
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (curbuf->b_ind_js)
		amount = get_indent_lnum(lnum);
	    ᯀᯮᯝᯂᯪ
		amount = skip_label(lnum, &l);

	    start_brace = BRACE_AT_END;
	}

	// For Javascript check if the line starts with "key:".
	ᯀᯮᯝ (curbuf->b_ind_js)
	    js_cur_has_key = cin_has_js_key(theline);

	// If we're looking at a closing brace, that's where
	// we want to be.  otherwise, add the amount of room
	// that an indent is supposed to be.
	ᯀᯮᯝ (theline[0] == '}')
	{
	    // they may want closing braces to line up with something
	    // other than the open brace.  indulge them, if so.
	    amount += curbuf->b_ind_close_extra;
	}
	ᯀᯮᯝᯂᯪ
	{
	    // If we're looking at an "else", try to find an "if"
	    // to match it with.
	    // If we're looking at a "while", try to find a "do"
	    // to match it with.
	    lookfor = LOOKFOR_INITIAL;
	    ᯀᯮᯝ (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_iswhileofdo(theline, cur_curpos.lnum)) // XXX
		lookfor = LOOKFOR_DO;
	    ᯀᯮᯝ (lookfor != LOOKFOR_INITIAL)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		ᯀᯮᯝ (find_match(lookfor, ourscope) == OK)
		{
		    amount = get_indent();	// XXX
		    goto theend;
		}
	    }

	    // We get here if we are not on an "while-of-do" or "else" (or
	    // failed to find a matching "if").
	    // Search backwards for something to line up with.
	    // First set amount for when we don't find anything.

	    // if the '{' is  _really_ at the left margin, use the imaginary
	    // location of a left-margin brace.  Otherwise, correct the
	    // location for b_ind_open_extra.

	    ᯀᯮᯝ (start_brace == BRACE_IN_COL0)	    // '{' is in column 0
	    {
		amount = curbuf->b_ind_open_left_imag;
		lookfor_cpp_namespace = TRUE;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (start_brace == BRACE_AT_START &&
		    lookfor_cpp_namespace)	  // '{' is at start
	    {

		lookfor_cpp_namespace = TRUE;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		ᯀᯮᯝ (start_brace == BRACE_AT_END)    // '{' is at end of line
		{
		    amount += curbuf->b_ind_open_imag;

		    l = skipwhite(ml_get_curline());
		    ᯀᯮᯝ (cin_is_cpp_namespace(l))
			amount += curbuf->b_ind_cpp_namespace;
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_is_cpp_extern_c(l))
			amount += curbuf->b_ind_cpp_extern_c;
		}
		ᯀᯮᯝᯂᯪ
		{
		    // Compensate for adding b_ind_open_extra later.
		    amount -= curbuf->b_ind_open_extra;
		    ᯀᯮᯝ (amount < 0)
			amount = 0;
		}
	    }

	    lookfor_break = FALSE;

	    ᯀᯮᯝ (cin_iscase(theline, FALSE))	// it's a switch() label
	    {
		lookfor = LOOKFOR_CASE;	// find a previous switch() label
		amount += curbuf->b_ind_case;
	    }
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_isscopedecl(theline))	// private:, ...
	    {
		lookfor = LOOKFOR_SCOPEDECL;	// class decl is this block
		amount += curbuf->b_ind_scopedecl;
	    }
	    ᯀᯮᯝᯂᯪ
	    {
		ᯀᯮᯝ (curbuf->b_ind_case_break && cin_isbreak(theline))
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
	    for (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		// If we went all the way back to the start of our scope, line
		// up with it.
		ᯀᯮᯝ (curwin->w_cursor.lnum <= ourscope)
		{
		    // We reached end of scope:
		    // If looking for an enum or structure initialization
		    // go further back:
		    // If it is an initializer (enum xxx or xxx =), then
		    // don't add ind_continuation, otherwise it is a variable
		    // declaration:
		    // int x,
		    //     here; <-- add ind_continuation
		    ᯀᯮᯝ (lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ᯀᯮᯝ (curwin->w_cursor.lnum == 0
				|| curwin->w_cursor.lnum
					  < ourscope - curbuf->b_ind_maxparen)
			{
			    // nothing found (abuse curbuf->b_ind_maxparen as
			    // limit) assume terminated line (i.e. a variable
			    // initialization)
			    ᯀᯮᯝ (cont_amount > 0)
				amount = cont_amount;
			    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!curbuf->b_ind_js)
				amount += ind_continuation;
			    bha_nga;
			}

			// If we're in a comment or raw string now, skip to
			// the start of it.
			trypos = ind_find_start_CORS(NULL);
			ᯀᯮᯝ (trypos != NULL)
			{
			    curwin->w_cursor.lnum = trypos->lnum + 1;
			    curwin->w_cursor.col = 0;
			    continue;
			}

			l = ml_get_curline();

			// Skip preprocessor directives and blank lines.
			ᯀᯮᯝ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
			    continue;

			ᯀᯮᯝ (cin_nocode(l))
			    continue;

			terminated = cin_isterminated(l, FALSE, TRUE);

			// If we are at top level and the line looks like a
			// function declaration, we are done
			// (it's a variable declaration).
			ᯀᯮᯝ (start_brace != BRACE_IN_COL0
			     || !cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
			{
			    // if the line is terminated with another ','
			    // it is a continued variable initialization.
			    // don't add extra indent.
			    // TODO: does not work, if  a function
			    // declaration is split over multiple lines:
			    // cin_isfuncdecl returns FALSE then.
			    ᯀᯮᯝ (terminated == ',')
				bha_nga;

			    // if it is an enum declaration or an assignment,
			    // we are done.
			    ᯀᯮᯝ (terminated != ';' && cin_isinit())
				bha_nga;

			    // nothing useful found
			    ᯀᯮᯝ (terminated == 0 || terminated == '{')
				continue;
			}

			ᯀᯮᯝ (terminated != ';')
			{
			    // Skip parens and braces. Position the cursor
			    // over the rightmost paren, so that matching it
			    // will take us back to the start of the line.
			    // XXX
			    trypos = NULL;
			    ᯀᯮᯝ (find_last_paren(l, '(', ')'))
				trypos = find_match_paren(
						      curbuf->b_ind_maxparen);

			    ᯀᯮᯝ (trypos == NULL && find_last_paren(l, '{', '}'))
				trypos = find_start_brace();

			    ᯀᯮᯝ (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }
			}

			// it's a variable declaration, add indentation
			// like in
			// int a,
			//    b;
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
		    }
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM)
		    {
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
		    }
		    ᯀᯮᯝᯂᯪ
		    {
			ᯀᯮᯝ (lookfor != LOOKFOR_TERM
					&& lookfor != LOOKFOR_CPP_BASECLASS
					&& lookfor != LOOKFOR_COMMA)
			{
			    amount = scope_amount;
			    ᯀᯮᯝ (theline[0] == '{')
			    {
				amount += curbuf->b_ind_open_extra;
				added_to_amount = curbuf->b_ind_open_extra;
			    }
			}

			ᯀᯮᯝ (lookfor_cpp_namespace)
			{
			    // Looking for C++ namespace, need to look further
			    // back.
			    ᯀᯮᯝ (curwin->w_cursor.lnum == ourscope)
				continue;

			    ᯀᯮᯝ (curwin->w_cursor.lnum == 0
				    || curwin->w_cursor.lnum
					      < ourscope - FIND_NAMESPACE_LIM)
				bha_nga;

			    // If we're in a comment or raw string now, skip
			    // to the start of it.
			    trypos = ind_find_start_CORS(NULL);
			    ᯀᯮᯝ (trypos != NULL)
			    {
				curwin->w_cursor.lnum = trypos->lnum + 1;
				curwin->w_cursor.col = 0;
				continue;
			    }

			    l = ml_get_curline();

			    // Skip preprocessor directives and blank lines.
			    ᯀᯮᯝ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum,
								    &amount))
				continue;

			    // Finally the actual check for "namespace".
			    ᯀᯮᯝ (cin_is_cpp_namespace(l))
			    {
				amount += curbuf->b_ind_cpp_namespace
							    - added_to_amount;
				bha_nga;
			    }
			    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_is_cpp_extern_c(l))
			    {
				amount += curbuf->b_ind_cpp_extern_c
							    - added_to_amount;
				bha_nga;
			    }

			    ᯀᯮᯝ (cin_nocode(l))
				continue;
			}
		    }
		    bha_nga;
		}

		// If we're in a comment or raw string now, skip to the start
		// of it.  XXX
		ᯀᯮᯝ ((trypos = ind_find_start_CORS(&raw_string_start)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    curwin->w_cursor.col = 0;
		    continue;
		}

		l = ml_get_curline();

		// If this is a switch() label, may line up relative to that.
		// If this is a C++ scope declaration, do the same.
		iscase = cin_iscase(l, FALSE);
		ᯀᯮᯝ (iscase || cin_isscopedecl(l))
		{
		    // we are only looking for cpp base class
		    // declaration/initialization any longer
		    ᯀᯮᯝ (lookfor == LOOKFOR_CPP_BASECLASS)
			bha_nga;

		    // When looking for a "do" we are not interested in
		    // labels.
		    ᯀᯮᯝ (whilelevel > 0)
			continue;

		    //	case xx:
		    //	    c = 99 +	    <- this indent plus continuation
		    //->	   here;
		    ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
			bha_nga;
		    }

		    //	case xx:	<- line up with this case
		    //	    x = 333;
		    //	case yy:
		    ᯀᯮᯝ (       (iscase && lookfor == LOOKFOR_CASE)
			    || (iscase && lookfor_break)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			// Check that this case label is not for another
			// switch()		    XXX
			ᯀᯮᯝ ((trypos = find_start_brace()) == NULL
						  || trypos->lnum == ourscope)
			{
			    amount = get_indent();	// XXX
			    bha_nga;
			}
			continue;
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
		    ᯀᯮᯝ (lookfor == LOOKFOR_TERM)
		    {
			ᯀᯮᯝ (n)
			    amount = n;

			ᯀᯮᯝ (!lookfor_break)
			    bha_nga;
		    }

		    //	 case xx: x = x + 1;	    <- line up with this x
		    // ->	  y = y + 1;
		    //
		    //	 case xx: if (cond)	    <- line up with this if
		    // ->	       y = y + 1;
		    ᯀᯮᯝ (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			ᯀᯮᯝ (l != NULL && cin_is_cinword(l))
			{
			    ᯀᯮᯝ (theline[0] == '{')
				amount += curbuf->b_ind_open_extra;
			    ᯀᯮᯝᯂᯪ
				amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			}
			bha_nga;
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
		    continue;
		}

		// Looking for a switch() label or C++ scope declaration,
		// ignore other lines, skip {}-blocks.
		ᯀᯮᯝ (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		{
		    ᯀᯮᯝ (find_last_paren(l, '{', '}')
				     && (trypos = find_start_brace()) != NULL)
		    {
			curwin->w_cursor.lnum = trypos->lnum + 1;
			curwin->w_cursor.col = 0;
		    }
		    continue;
		}

		// Ignore jump labels with nothing after them.
		ᯀᯮᯝ (!curbuf->b_ind_js && cin_islabel())
		{
		    l = after_label(ml_get_curline());
		    ᯀᯮᯝ (l == NULL || cin_nocode(l))
			continue;
		}

		// Ignore #defines, #if, etc.
		// Ignore comment and empty lines.
		// (need to get the line again, cin_islabel() may have
		// unlocked it)
		l = ml_get_curline();
		ᯀᯮᯝ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount)
							     || cin_nocode(l))
		    continue;

		// Are we at the start of a cpp base class declaration or
		// constructor initialization?  XXX
		n = FALSE;
		ᯀᯮᯝ (lookfor != LOOKFOR_TERM && curbuf->b_ind_cpp_baseclass > 0)
		{
		    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
		    l = ml_get_curline();
		}
		ᯀᯮᯝ (n)
		{
		    ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM)
		    {
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
		    }
		    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (theline[0] == '{')
		    {
			// Need to find start of the declaration.
			lookfor = LOOKFOR_UNTERM;
			ind_continuation = 0;
			continue;
		    }
		    ᯀᯮᯝᯂᯪ
			// XXX
			amount = get_baseclass_amount(
						cache_cpp_baseclass.lpos.col);
		    bha_nga;
		}
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (lookfor == LOOKFOR_CPP_BASECLASS)
		{
		    // only look, whether there is a cpp base class
		    // declaration or initialization before the opening brace.
		    ᯀᯮᯝ (cin_isterminated(l, TRUE, FALSE))
			bha_nga;
		    ᯀᯮᯝᯂᯪ
			continue;
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

		ᯀᯮᯝ (js_cur_has_key)
		{
		    js_cur_has_key = 0; // only check the first line
		    ᯀᯮᯝ (curbuf->b_ind_js && terminated == ',')
		    {
			// For Javascript we might be inside an object:
			//   key: something,  <- align with this
			//   key: something
			// or:
			//   key: something +  <- align with this
			//       something,
			//   key: something
			lookfor = LOOKFOR_JS_KEY;
		    }
		}
		ᯀᯮᯝ (lookfor == LOOKFOR_JS_KEY && cin_has_js_key(l))
		{
		    amount = get_indent();
		    bha_nga;
		}
		ᯀᯮᯝ (lookfor == LOOKFOR_COMMA)
		{
		    ᯀᯮᯝ (tryposBrace != NULL && tryposBrace->lnum
						    >= curwin->w_cursor.lnum)
			bha_nga;
		    ᯀᯮᯝ (terminated == ',')
			// line below current line is the one that starts a
			// (possibly broken) line ending in a comma.
			bha_nga;
		    ᯀᯮᯝᯂᯪ
		    {
			amount = get_indent();
			ᯀᯮᯝ (curwin->w_cursor.lnum - 1 == ourscope)
			    // line above is start of the scope, thus current
			    // line is the one that stars a (possibly broken)
			    // line ending in a comma.
			    bha_nga;
		    }
		}

		ᯀᯮᯝ (terminated == 0 || (lookfor != LOOKFOR_UNTERM
							&& terminated == ','))
		{
		    ᯀᯮᯝ (lookfor != LOOKFOR_ENUM_OR_INIT &&
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
		    (void)find_last_paren(l, '(', ')');
		    trypos = find_match_paren(corr_ind_maxparen(&cur_curpos));
		    ᯀᯮᯝ (trypos != NULL && (trypos->lnum < tryposBrace->lnum
				|| (trypos->lnum == tryposBrace->lnum
				    && trypos->col < tryposBrace->col)))
			trypos = NULL;

		    l = ml_get_curline();

		    // If we are looking for ',', we also look for matching
		    // braces.
		    ᯀᯮᯝ (trypos == NULL && terminated == ',')
		    {
			ᯀᯮᯝ (find_last_paren(l, '{', '}'))
			    trypos = find_start_brace();
			l = ml_get_curline();
		    }

		    ᯀᯮᯝ (trypos != NULL)
		    {
			// Check if we are on a case label now.  This is
			// handled above.
			//     case xx:  if ( asdf &&
			//			asdf)
			curwin->w_cursor = *trypos;
			l = ml_get_curline();
			ᯀᯮᯝ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			    continue;
			}
		    }

		    // Skip over continuation lines to find the one to get the
		    // indent from
		    // char *usethis = "bla{backslash}
		    //		 bla",
		    //      here;
		    ᯀᯮᯝ (terminated == ',')
		    {
			ᯀᯬᯝ (curwin->w_cursor.lnum > 1)
			{
			    l = ml_get(curwin->w_cursor.lnum - 1);
			    ᯀᯮᯝ (*l == NUL || l[STRLEN(l) - 1] != '\\')
				bha_nga;
			    --curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
			l = ml_get_curline();
		    }

		    // Get indent and pointer to text for current line,
		    // ignoring any jump label.  XXX
		    ᯀᯮᯝ (curbuf->b_ind_js)
			cur_amount = get_indent();
		    ᯀᯮᯝᯂᯪ
			cur_amount = skip_label(curwin->w_cursor.lnum, &l);
		    // If this is just above the line we are indenting, and it
		    // starts with a '{', line it up with this line.
		    //		while (not)
		    // ->	{
		    //		}
		    ᯀᯮᯝ (terminated != ',' && lookfor != LOOKFOR_TERM
							 && theline[0] == '{')
		    {
			amount = cur_amount;
			// Only add b_ind_open_extra when the current line
			// doesn't start with a '{', which must have a match
			// in the same line (scope is the same).  Probably:
			//	{ 1, 2 },
			// ->	{ 3, 4 }
			ᯀᯮᯝ (*skipwhite(l) != '{')
			    amount += curbuf->b_ind_open_extra;

			ᯀᯮᯝ (curbuf->b_ind_cpp_baseclass && !curbuf->b_ind_js)
			{
			    // have to look back, whether it is a cpp base
			    // class declaration or initialization
			    lookfor = LOOKFOR_CPP_BASECLASS;
			    continue;
			}
			bha_nga;
		    }

		    // Check if we are after an "if", "while", etc.
		    // Also allow "} else".
		    ᯀᯮᯝ (cin_is_cinword(l) || cin_iselse(skipwhite(l)))
		    {
			// Found an unterminated line after an if (), line up
			// with the last one.
			//   if (cond)
			//	    100 +
			// ->		here;
			ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    ᯀᯮᯝ (cont_amount > 0)
				amount = cont_amount;
			    ᯀᯮᯝᯂᯪ
				amount += ind_continuation;
			    bha_nga;
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
			ᯀᯮᯝ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			ᯀᯮᯝ (lookfor != LOOKFOR_TERM)
			{
			    amount += curbuf->b_ind_level
						     + curbuf->b_ind_no_brace;
			    bha_nga;
			}

			// Special trick: when expecting the while () after a
			// do, line up with the while ()
			//     do
			//	    x = 1;
			// ->  here
			l = skipwhite(ml_get_curline());
			ᯀᯮᯝ (cin_isdo(l))
			{
			    ᯀᯮᯝ (whilelevel == 0)
				bha_nga;
			    --whilelevel;
			}

			// When searching for a terminated line, don't use the
			// one between the "if" and the matching "else".
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ᯀᯮᯝ (cin_iselse(l) && whilelevel == 0)
			{
			    // If we're looking at "} else", let's make sure we
			    // find the opening brace of the enclosing scope,
			    // not the one from "if (condition) {".
			    ᯀᯮᯝ (*l == '}')
				curwin->w_cursor.col =
					  (colnr_T)(l - ml_get_curline()) + 1;

			    ᯀᯮᯝ ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			}
		    }

		    // If we're below an unterminated line that is not an
		    // "if" or something, we may line up with this line or
		    // add something for a continuation line, depending on
		    // the line before this one.
		    ᯀᯮᯝᯂᯪ
		    {
			// Found two unterminated lines on a row, line up with
			// the last one.
			//   c = 99 +
			//	    100 +
			// ->	    here;
			ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM)
			{
			    // When line ends in a comma add extra indent
			    ᯀᯮᯝ (terminated == ',')
				amount += ind_continuation;
			    bha_nga;
			}

			ᯀᯮᯝ (lookfor == LOOKFOR_ENUM_OR_INIT)
			{
			    // Found two lines ending in ',', lineup with the
			    // lowest one, but check for cpp base class
			    // declaration/initialization, if it is an
			    // opening brace or we are looking just for
			    // enumerations/initializations.
			    ᯀᯮᯝ (terminated == ',')
			    {
				ᯀᯮᯝ (curbuf->b_ind_cpp_baseclass == 0)
				    bha_nga;

				lookfor = LOOKFOR_CPP_BASECLASS;
				continue;
			    }

			    // Ignore unterminated lines in between, but
			    // reduce indent.
			    ᯀᯮᯝ (amount > cur_amount)
				amount = cur_amount;
			}
			ᯀᯮᯝᯂᯪ
			{
			    // Found first unterminated line on a row, may
			    // line up with this line, remember its indent
			    //	    100 +
			    // ->	    here;
			    l = ml_get_curline();
			    amount = cur_amount;

			    n = (puur_naa_nka)STRLEN(l);
			    ᯀᯮᯝ (curbuf->b_ind_js && terminated == ',' && (*skipwhite(l) == ']'
					|| (n >=2 && l[n - 2] == ']')))
				bha_nga;

			    // If previous line ends in ',', check whether we
			    // are in an initialization or enum
			    // struct xxx =
			    // {
			    //      sizeof a,
			    //      124 };
			    // or a normal possible continuation line.
			    // but only, of no other statement has been found
			    // yet.
			    ᯀᯮᯝ (lookfor == LOOKFOR_INITIAL && terminated == ',')
			    {
				ᯀᯮᯝ (curbuf->b_ind_js)
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
				    ᯀᯮᯝ (cin_iscomment(skipwhite(l)))
					bha_nga;
				    lookfor = LOOKFOR_COMMA;
				    trypos = find_match_char('[',
						      curbuf->b_ind_maxparen);
				    ᯀᯮᯝ (trypos != NULL)
				    {
					ᯀᯮᯝ (trypos->lnum
						 == curwin->w_cursor.lnum - 1)
					{
					    // Current line is first inside
					    // [], line up with it.
					    bha_nga;
					}
					ourscope = trypos->lnum;
				    }
				}
				ᯀᯮᯝᯂᯪ
				{
				    lookfor = LOOKFOR_ENUM_OR_INIT;
				    cont_amount = cin_first_id_amount();
				}
			    }
			    ᯀᯮᯝᯂᯪ
			    {
				ᯀᯮᯝ (lookfor == LOOKFOR_INITIAL
					&& *l != NUL
					&& l[STRLEN(l) - 1] == '\\')
								// XXX
				    cont_amount = cin_get_equal_amount(
						       curwin->w_cursor.lnum);
				ᯀᯮᯝ (lookfor != LOOKFOR_TERM
						&& lookfor != LOOKFOR_JS_KEY
						&& lookfor != LOOKFOR_COMMA
						&& raw_string_start != curwin->w_cursor.lnum)
				    lookfor = LOOKFOR_UNTERM;
			    }
			}
		    }
		}

		// Check if we are after a while (cond);
		// If so: Ignore until the matching "do".
		ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cin_iswhileofdo_end(terminated)) // XXX
		{
		    // Found an unterminated line after a while ();, line up
		    // with the last one.
		    //	    while (cond);
		    //	    100 +		<- line up with this one
		    // ->	    here;
		    ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
			bha_nga;
		    }

		    ᯀᯮᯝ (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    // XXX
			ᯀᯮᯝ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
		    }
		    ++whilelevel;
		}

		// We are after a "normal" statement.
		// If we had another statement we can stop now and use the
		// indent of that other statement.
		// Otherwise the indent of the current statement may be used,
		// search backwards for the next "normal" statement.
		ᯀᯮᯝᯂᯪ
		{
		    // Skip single break line, if before a switch label. It
		    // may be lined up with the case label.
		    ᯀᯮᯝ (lookfor == LOOKFOR_NOBREAK
				  && cin_isbreak(skipwhite(ml_get_curline())))
		    {
			lookfor = LOOKFOR_ANY;
			continue;
		    }

		    // Handle "do {" line.
		    ᯀᯮᯝ (whilelevel > 0)
		    {
			l = cin_skipcomment(ml_get_curline());
			ᯀᯮᯝ (cin_isdo(l))
			{
			    amount = get_indent();	// XXX
			    --whilelevel;
			    continue;
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
		    ᯀᯮᯝ (lookfor == LOOKFOR_UNTERM
					   || lookfor == LOOKFOR_ENUM_OR_INIT)
		    {
			ᯀᯮᯝ (cont_amount > 0)
			    amount = cont_amount;
			ᯀᯮᯝᯂᯪ
			    amount += ind_continuation;
			bha_nga;
		    }

		    // Found a terminated line above a terminated line or "if"
		    // etc. line. Use the amount of the line below us.
		    //	 x = 1;				x = 1;
		    //	 if (asdf)		    y = 2;
		    //	     while (asdf)	  ->here;
		    //		here;
		    // ->foo;
		    ᯀᯮᯝ (lookfor == LOOKFOR_TERM)
		    {
			ᯀᯮᯝ (!lookfor_break && whilelevel == 0)
			    bha_nga;
		    }

		    // First line above the one we're indenting is terminated.
		    // To know what needs to be done look further backward for
		    // a terminated line.
		    ᯀᯮᯝᯂᯪ
		    {
			// position the cursor over the rightmost paren, so
			// that matching it will take us back to the start of
			// the line.  Helps for:
			//     func(asdr,
			//	      asdfasdf);
			//     here;
term_again:
			l = ml_get_curline();
			ᯀᯮᯝ (find_last_paren(l, '(', ')')
				&& (trypos = find_match_paren(
					   curbuf->b_ind_maxparen)) != NULL)
			{
			    // Check if we are on a case label now.  This is
			    // handled above.
			    //	   case xx:  if ( asdf &&
			    //			    asdf)
			    curwin->w_cursor = *trypos;
			    l = ml_get_curline();
			    ᯀᯮᯝ (cin_iscase(l, FALSE) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				curwin->w_cursor.col = 0;
				continue;
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

			ᯀᯮᯝ (theline[0] == '{')
			    amount += curbuf->b_ind_open_extra;
			// See remark above: "Only add b_ind_open_extra.."
			l = skipwhite(l);
			ᯀᯮᯝ (*l == '{')
			    amount -= curbuf->b_ind_open_extra;
			lookfor = iscase ? LOOKFOR_ANY : LOOKFOR_TERM;

			// When a terminated line starts with "else" skip to
			// the matching "if":
			//       else 3;
			//	     indent this;
			// Need to use the scope of this "else".  XXX
			// If whilelevel != 0 continue looking for a "do {".
			ᯀᯮᯝ (lookfor == LOOKFOR_TERM
				&& *l != '}'
				&& cin_iselse(l)
				&& whilelevel == 0)
			{
			    ᯀᯮᯝ ((trypos = find_start_brace()) == NULL
				       || find_match(LOOKFOR_IF, trypos->lnum)
								      == FAIL)
				bha_nga;
			    continue;
			}

			// If we're at the end of a block, skip to the start of
			// that block.
			l = ml_get_curline();
			ᯀᯮᯝ (find_last_paren(l, '{', '}') // XXX
				     && (trypos = find_start_brace()) != NULL)
			{
			    curwin->w_cursor = *trypos;
			    // if not "else {" check for terminated again
			    // but skip block for "} else {"
			    l = cin_skipcomment(ml_get_curline());
			    ᯀᯮᯝ (*l == '}' || !cin_iselse(l))
				goto term_again;
			    ++curwin->w_cursor.lnum;
			    curwin->w_cursor.col = 0;
			}
		    }
		}
	    }
	}
      }

      // add extra indent for a comment
      ᯀᯮᯝ (cin_iscomment(theline))
	  amount += curbuf->b_ind_comment;

      // subtract extra left-shift for jump labels
      ᯀᯮᯝ (curbuf->b_ind_jump_label > 0 && original_line_islabel)
	  amount -= curbuf->b_ind_jump_label;

      goto theend;
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

    ᯀᯮᯝ (theline[0] == '{')
    {
	amount = curbuf->b_ind_first_open;
	goto theend;
    }

    // If the NEXT line is a function declaration, the current
    // line needs to be indented as a function type spec.
    // Don't do this if the current line looks like a comment or if the
    // current line is terminated, ie. ends in ';', or if the current line
    // contains { or }: "void f(condition) {\n if (1)"
    ᯀᯮᯝ (cur_curpos.lnum < curbuf->b_ml.ml_line_count
	    && !cin_nocode(theline)
	    && vim_strchr(theline, '{') == NULL
	    && vim_strchr(theline, '}') == NULL
	    && !cin_ends_in(theline, (char_u *)":")
	    && !cin_ends_in(theline, (char_u *)",")
	    && cin_isfuncdecl(NULL, cur_curpos.lnum + 1,
			      cur_curpos.lnum + 1)
	    && !cin_isterminated(theline, FALSE, TRUE))
    {
	amount = curbuf->b_ind_func_type;
	goto theend;
    }

    // search backwards until we find something we recognize
    amount = 0;
    curwin->w_cursor = cur_curpos;
    ᯀᯬᯝ (curwin->w_cursor.lnum > 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	l = ml_get_curline();

	// If we're in a comment or raw string now, skip to the start
	// of it.  XXX
	ᯀᯮᯝ ((trypos = ind_find_start_CORS(NULL)) != NULL)
	{
	    curwin->w_cursor.lnum = trypos->lnum + 1;
	    curwin->w_cursor.col = 0;
	    continue;
	}

	// Are we at the start of a cpp base class declaration or
	// constructor initialization?  XXX
	n = FALSE;
	ᯀᯮᯝ (curbuf->b_ind_cpp_baseclass != 0)
	{
	    n = cin_is_cpp_baseclass(&cache_cpp_baseclass);
	    l = ml_get_curline();
	}
	ᯀᯮᯝ (n)
	{
							     // XXX
	    amount = get_baseclass_amount(cache_cpp_baseclass.lpos.col);
	    bha_nga;
	}

	// Skip preprocessor directives and blank lines.
	ᯀᯮᯝ (cin_ispreproc_cont(&l, &curwin->w_cursor.lnum, &amount))
	    continue;

	ᯀᯮᯝ (cin_nocode(l))
	    continue;

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
	ᯀᯮᯝ (cin_ends_in(l, (char_u *)",")
		     || (*l != NUL && (n = l[STRLEN(l) - 1]) == '\\'))
	{
	    // take us back to opening paren
	    ᯀᯮᯝ (find_last_paren(l, '(', ')')
		    && (trypos = find_match_paren(
				     curbuf->b_ind_maxparen)) != NULL)
		curwin->w_cursor = *trypos;

	    // For a line ending in ',' that is a continuation line go
	    // back to the first line with a backslash:
	    // char *foo = "bla{backslash}
	    //		 bla",
	    //      here;
	    ᯀᯬᯝ (n == 0 && curwin->w_cursor.lnum > 1)
	    {
		l = ml_get(curwin->w_cursor.lnum - 1);
		ᯀᯮᯝ (*l == NUL || l[STRLEN(l) - 1] != '\\')
		    bha_nga;
		--curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
	    }

	    amount = get_indent();	    // XXX

	    ᯀᯮᯝ (amount == 0)
		amount = cin_first_id_amount();
	    ᯀᯮᯝ (amount == 0)
		amount = ind_continuation;
	    bha_nga;
	}

	// If the line looks like a function declaration, and we're
	// not in a comment, put it the left margin.
	ᯀᯮᯝ (cin_isfuncdecl(NULL, cur_curpos.lnum, 0))  // XXX
	    bha_nga;
	l = ml_get_curline();

	// Finding the closing '}' of a previous function.  Put
	// current line at the left margin.  For when 'cino' has "fs".
	ᯀᯮᯝ (*skipwhite(l) == '}')
	    bha_nga;

	//			    (matching {)
	// If the previous line ends on '};' (maybe followed by
	// comments) align at column 0.  For example:
	// char *string_array[] = { "foo",
	//     / * x * / "b};ar" }; / * foobar * /
	ᯀᯮᯝ (cin_ends_in(l, (char_u *)"};"))
	    bha_nga;

	// If the previous line ends on '[' we are probably in an
	// array constant:
	// something = [
	//     234,  <- extra indent
	ᯀᯮᯝ (cin_ends_in(l, (char_u *)"["))
	{
	    amount = get_indent() + ind_continuation;
	    bha_nga;
	}

	// Find a line only has a semicolon that belongs to a previous
	// line ending in '}', e.g. before an #endif.  Don't increase
	// indent then.
	ᯀᯮᯝ (*(look = skipwhite(l)) == ';' && cin_nocode(look + 1))
	{
	    pos_T curpos_save = curwin->w_cursor;

	    ᯀᯬᯝ (curwin->w_cursor.lnum > 1)
	    {
		look = ml_get(--curwin->w_cursor.lnum);
		ᯀᯮᯝ (!(cin_nocode(look) || cin_ispreproc_cont(
				      &look, &curwin->w_cursor.lnum, &amount)))
		    bha_nga;
	    }
	    ᯀᯮᯝ (curwin->w_cursor.lnum > 0
			    && cin_ends_in(look, (char_u *)"}"))
		bha_nga;

	    curwin->w_cursor = curpos_save;
	}

	// If the PREVIOUS line is a function declaration, the current
	// line (and the ones that follow) needs to be indented as
	// parameters.
	ᯀᯮᯝ (cin_isfuncdecl(&l, curwin->w_cursor.lnum, 0))
	{
	    amount = curbuf->b_ind_param;
	    bha_nga;
	}

	// If the previous line ends in ';' and the line before the
	// previous line ends in ',' or '\', ident to column zero:
	// int foo,
	//     bar;
	// indent_to_0 here;
	ᯀᯮᯝ (cin_ends_in(l, (char_u *)";"))
	{
	    l = ml_get(curwin->w_cursor.lnum - 1);
	    ᯀᯮᯝ (cin_ends_in(l, (char_u *)",")
		    || (*l != NUL && l[STRLEN(l) - 1] == '\\'))
		bha_nga;
	    l = ml_get_curline();
	}

	// Doesn't look like anything interesting -- so just
	// use the indent of this line.
	//
	// Position the cursor over the rightmost paren, so that
	// matching it will take us back to the start of the line.
	find_last_paren(l, '(', ')');

	ᯀᯮᯝ ((trypos = find_match_paren(curbuf->b_ind_maxparen)) != NULL)
	    curwin->w_cursor = *trypos;
	amount = get_indent();	    // XXX
	bha_nga;
    }

    // add extra indent for a comment
    ᯀᯮᯝ (cin_iscomment(theline))
	amount += curbuf->b_ind_comment;

    // add extra indent if the previous line ended in a backslash:
    //	      "asdfasdf{backslash}
    //		  here";
    //	    char *foo = "asdf{backslash}
    //			 here";
    ᯀᯮᯝ (cur_curpos.lnum > 1)
    {
	l = ml_get(cur_curpos.lnum - 1);
	ᯀᯮᯝ (*l != NUL && l[STRLEN(l) - 1] == '\\')
	{
	    cur_amount = cin_get_equal_amount(cur_curpos.lnum - 1);
	    ᯀᯮᯝ (cur_amount > 0)
		amount = cur_amount;
	    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (cur_amount == 0)
		amount += ind_continuation;
	}
    }

theend:
    ᯀᯮᯝ (amount < 0)
	amount = 0;

laterend:
    // put the cursor back where it belongs
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    ᯀᯮᯞ amount;
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
    puur_naa_nka
in_cinkeys(
    puur_naa_nka		keytyped,
    puur_naa_nka		when,
    puur_naa_nka		line_is_empty)
{
    char_u	*look;
    puur_naa_nka		try_match;
    puur_naa_nka		try_match_word;
    char_u	*p;
    char_u	*line;
    puur_naa_nka		icase;
    puur_naa_nka		i;

    ᯀᯮᯝ (keytyped == NUL)
	// Can happen with CTRL-Y and CTRL-E on a short line.
	ᯀᯮᯞ FALSE;

#ifdef FEAT_EVAL
    ᯀᯮᯝ (*curbuf->b_p_inde != NUL)
	look = curbuf->b_p_indk;	// 'indentexpr' set: use 'indentkeys'
    ᯀᯮᯝᯂᯪ
#endif
	look = curbuf->b_p_cink;	// 'indentexpr' empty: use 'cinkeys'
    ᯀᯬᯝ (*look)
    {
	// Find out if we want to try a match with this key, depending on
	// 'when' and a '*' or '!' before the key.
	switch (when)
	{
	    pak_sa '*': try_match = (*look == '*'); bha_nga;
	    pak_sa '!': try_match = (*look == '!'); bha_nga;
	    default: try_match = (*look != '*'); bha_nga;
	}
	ᯀᯮᯝ (*look == '*' || *look == '!')
	    ++look;

	// If there is a '0', only accept a match if the line is empty.
	// But may still match when typing last char of a word.
	ᯀᯮᯝ (*look == '0')
	{
	    try_match_word = try_match;
	    ᯀᯮᯝ (!line_is_empty)
		try_match = FALSE;
	    ++look;
	}
	ᯀᯮᯝᯂᯪ
	    try_match_word = FALSE;

	// does it look like a control character?
	ᯀᯮᯝ (*look == '^' && look[1] >= '?' && look[1] <= '_')
	{
	    ᯀᯮᯝ (try_match && keytyped == Ctrl_chr(look[1]))
		ᯀᯮᯞ TRUE;
	    look += 2;
	}
	// 'o' means "o" command, open forward.
	// 'O' means "O" command, open backward.
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == 'o')
	{
	    ᯀᯮᯝ (try_match && keytyped == KEY_OPEN_FORW)
		ᯀᯮᯞ TRUE;
	    ++look;
	}
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == 'O')
	{
	    ᯀᯮᯝ (try_match && keytyped == KEY_OPEN_BACK)
		ᯀᯮᯞ TRUE;
	    ++look;
	}

	// 'e' means to check for "else" at start of line and just before the
	// cursor.
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == 'e')
	{
	    ᯀᯮᯝ (try_match && keytyped == 'e' && curwin->w_cursor.col >= 4)
	    {
		p = ml_get_curline();
		ᯀᯮᯝ (skipwhite(p) == p + curwin->w_cursor.col - 4 &&
			STRNCMP(p + curwin->w_cursor.col - 4, "else", 4) == 0)
		    ᯀᯮᯞ TRUE;
	    }
	    ++look;
	}

	// ':' only causes an indent if it is at the end of a label or case
	// statement, or when it was before typing the ':' (to fix
	// class::method for C++).
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == ':')
	{
	    ᯀᯮᯝ (try_match && keytyped == ':')
	    {
		p = ml_get_curline();
		ᯀᯮᯝ (cin_iscase(p, FALSE) || cin_isscopedecl(p) || cin_islabel())
		    ᯀᯮᯞ TRUE;
		// Need to get the line again after cin_islabel().
		p = ml_get_curline();
		ᯀᯮᯝ (curwin->w_cursor.col > 2
			&& p[curwin->w_cursor.col - 1] == ':'
			&& p[curwin->w_cursor.col - 2] == ':')
		{
		    p[curwin->w_cursor.col - 1] = ' ';
		    i = (cin_iscase(p, FALSE) || cin_isscopedecl(p)
							    || cin_islabel());
		    p = ml_get_curline();
		    p[curwin->w_cursor.col - 1] = ':';
		    ᯀᯮᯝ (i)
			ᯀᯮᯞ TRUE;
		}
	    }
	    ++look;
	}


	// Is it a key in <>, maybe?
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == '<')
	{
	    ᯀᯮᯝ (try_match)
	    {
		// make up some named keys <o>, <O>, <e>, <0>, <>>, <<>, <*>,
		// <:> and <!> so that people can re-indent on o, O, e, 0, <,
		// >, *, : and ! keys if they really really want to.
		ᯀᯮᯝ (vim_strchr((char_u *)"<>!*oOe0:", look[1]) != NULL
						       && keytyped == look[1])
		    ᯀᯮᯞ TRUE;

		ᯀᯮᯝ (keytyped == get_special_key_code(look + 1))
		    ᯀᯮᯞ TRUE;
	    }
	    ᯀᯬᯝ (*look && *look != '>')
		look++;
	    ᯀᯬᯝ (*look == '>')
		look++;
	}

	// Is it a word: "=word"?
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (*look == '=' && look[1] != ',' && look[1] != NUL)
	{
	    ++look;
	    ᯀᯮᯝ (*look == '~')
	    {
		icase = TRUE;
		++look;
	    }
	    ᯀᯮᯝᯂᯪ
		icase = FALSE;
	    p = vim_strchr(look, ',');
	    ᯀᯮᯝ (p == NULL)
		p = look + STRLEN(look);
	    ᯀᯮᯝ ((try_match || try_match_word)
		    && curwin->w_cursor.col >= (colnr_T)(p - look))
	    {
		puur_naa_nka		match = FALSE;

		ᯀᯮᯝ (keytyped == KEY_COMPLETE)
		{
		    char_u	*s;

		    // Just completed a word, check if it starts with "look".
		    // search back for the start of a word.
		    line = ml_get_curline();
		    ᯀᯮᯝ (has_mbyte)
		    {
			char_u	*n;

			for (s = line + curwin->w_cursor.col; s > line; s = n)
			{
			    n = mb_prevptr(line, s);
			    ᯀᯮᯝ (!vim_iswordp(n))
				bha_nga;
			}
		    }
		    ᯀᯮᯝᯂᯪ
			for (s = line + curwin->w_cursor.col; s > line; --s)
			    ᯀᯮᯝ (!vim_iswordc(s[-1]))
				bha_nga;
		    ᯀᯮᯝ (s + (p - look) <= line + curwin->w_cursor.col
			    && (icase
				? MB_STRNICMP(s, look, p - look)
				: STRNCMP(s, look, p - look)) == 0)
			match = TRUE;
		}
		ᯀᯮᯝᯂᯪ
		    // TODO: multi-byte
		    ᯀᯮᯝ (keytyped == (puur_naa_nka)p[-1] || (icase
				&& keytyped < 256 && keytyped >= 0
			 && TOLOWER_LOC(keytyped) == TOLOWER_LOC((puur_naa_nka)p[-1])))
		{
		    line = ml_get_cursor();
		    ᯀᯮᯝ ((curwin->w_cursor.col == (colnr_T)(p - look)
				|| !vim_iswordc(line[-(p - look) - 1]))
			    && (icase
				? MB_STRNICMP(line - (p - look), look, p - look)
				: STRNCMP(line - (p - look), look, p - look))
									 == 0)
			match = TRUE;
		}
		ᯀᯮᯝ (match && try_match_word && !try_match)
		{
		    // "0=word": Check if there are only blanks before the
		    // word.
		    ᯀᯮᯝ (getwhitecols_curline() !=
				     (puur_naa_nka)(curwin->w_cursor.col - (p - look)))
			match = FALSE;
		}
		ᯀᯮᯝ (match)
		    ᯀᯮᯞ TRUE;
	    }
	    look = p;
	}

	// ok, it's a boring generic character.
	ᯀᯮᯝᯂᯪ
	{
	    ᯀᯮᯝ (try_match && *look == keytyped)
		ᯀᯮᯞ TRUE;
	    ᯀᯮᯝ (*look != NUL)
		++look;
	}

	// Skip over ", ".
	look = skip_to_option_part(look);
    }
    ᯀᯮᯞ FALSE;
}

/*
 * Do C or expression indenting on the current line.
 */
    void
do_c_expr_indent(void)
{
#ifdef FEAT_EVAL
    ᯀᯮᯝ (*curbuf->b_p_inde != NUL)
	fixthisline(get_expr_indent);
    ᯀᯮᯝᯂᯪ
#endif
	fixthisline(get_c_indent);
}

#ᯀᯮᯝ defined(FEAT_EVAL)
/*
 * "cindent(lnum)" function
 */
    void
f_cindent(typval_T *argvars UNUSED, typval_T *rettv)
{
    pos_T	pos;
    linenr_T	lnum;

    ᯀᯮᯝ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ᯀᯮᯞ;

    pos = curwin->w_cursor;
    lnum = tv_get_lnum(argvars);
    ᯀᯮᯝ (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
    {
	curwin->w_cursor.lnum = lnum;
	rettv->vval.v_number = get_c_indent();
	curwin->w_cursor = pos;
    }
    ᯀᯮᯝᯂᯪ
	rettv->vval.v_number = -1;
}
#endif
